#include "decoder.hpp"

// ffmpeg complains without this
#define __STDC_CONSTANT_MACROS

extern "C"
{
#include <libavutil/opt.h>
#include <libavutil/channel_layout.h>
#include <libswresample/swresample.h>
}

AudioDecoder::AudioDecoder() :
    swr(swr_alloc()),
    inFrame(av_frame_alloc())
{
    av_init_packet(&inPacket);
    inPacket.data = nullptr;
    inPacket.size = 0;
}

AudioDecoder::~AudioDecoder()
{
    dropFile();
    av_frame_free(&inFrame);
    swr_free(&swr);
}

int AudioDecoder::openFile(const FileName& path)
{
    int err = 0;
    dropFile();

    inFmtCtx = avformat_alloc_context();
    if (!ioCtx.initAVFmtCtx(path, inFmtCtx)) {
        fprintf(stderr, "Failed to initialize AVFormatContext.\n");
        return err;
    }
    if (err = avformat_open_input(&inFmtCtx, "", NULL, NULL)) {
        fprintf(stderr, "AVERROR %d: Could not open source file(file itself opened successfully).", err);
        return err;
    }
    // Get stream information
    if ((err = avformat_find_stream_info(inFmtCtx, nullptr)) < 0) {
        fprintf(stderr, "AVERROR %d: Could not find stream information.\n", err);
        return err;
    }

    // Find any audio stream
    if ((err = av_find_best_stream(inFmtCtx, AVMEDIA_TYPE_AUDIO, -1, -1, nullptr, 0)) < 0) {
        fprintf(stderr, "AVERROR %d: Could not find an audio stream.\n", err);
        return err;
    }
    inStreamIndex = err;

    // Init codecCtx
    AVCodecParameters* codecPars = inFmtCtx->streams[inStreamIndex]->codecpar;
    AVCodec* codec = avcodec_find_decoder(codecPars->codec_id);
    if (!codec) {
        fprintf(stderr, "AVERROR %d: Failed to find a decoder.\n", AVERROR(EINVAL));
        return AVERROR(EINVAL);
    }

    if (!(inCodecCtx = avcodec_alloc_context3(codec))) {
        fprintf(stderr, "AVERROR %d: Failed to allocate codec context.\n", AVERROR(ENOMEM));
        return AVERROR(ENOMEM);
    }
    if ((err = avcodec_parameters_to_context(inCodecCtx, codecPars)) < 0) {
        fprintf(stderr, "AVERROR %d: Failed to copy parameters to codec context.\n", err);
        return err;
    }
    if ((err = avcodec_open2(inCodecCtx, codec, nullptr)) < 0) {
        fprintf(stderr, "AVERROR %d: Failed to initialize codec context.\n", err);
        return err;
    }

    // Set swr input
    av_opt_set_channel_layout(swr, "in_channel_layout", inCodecCtx->channel_layout, 0);
    av_opt_set_int(swr, "in_sample_rate", inCodecCtx->sample_rate, 0);
    av_opt_set_sample_fmt(swr, "in_sample_fmt", inCodecCtx->sample_fmt, 0);
    // Try to init swr
    err = swr_init(swr);

    // Failed to init swr - no output options set, use defaults from the file
    if (err < 0)
    {
        if (!inCodecCtx->channel_layout)
            inCodecCtx->channel_layout = av_get_default_channel_layout(inCodecCtx->channels); // If no layout set default

        err = setOutputFmt({ inCodecCtx->channel_layout,
                             inCodecCtx->sample_rate,
                             inCodecCtx->sample_fmt });
    }

    return err;
}

// Free everything file related
void AudioDecoder::dropFile()
{
    newFrame = true;
    inStreamIndex = -1;

    if (inCodecCtx)
        avcodec_free_context(&inCodecCtx);
    if (inFmtCtx)
        avformat_close_input(&inFmtCtx);
    if (sampleBuff.data)
    {
        sampleBuff.numAlloc = 0;
        sampleBuff.numSamples = 0;
        av_freep(&sampleBuff.data);
    }
    // Might be unnecessary sometimes, not a problem
    av_packet_unref(&inPacket);
    // Close file
    ioCtx.closeIO();
}

int AudioDecoder::seek(double timestamp)
{
    int ret = 0;
    timestamp *= inFmtCtx->streams[inStreamIndex]->time_base.den / inFmtCtx->streams[inStreamIndex]->time_base.num;

    if ((ret = av_seek_frame(inFmtCtx, inStreamIndex, timestamp, AVSEEK_FLAG_ANY)) < 0)
        fprintf(stderr, "Seeking to %f failed. Error %d\n", timestamp, ret);
    else
    {
        avcodec_flush_buffers(inCodecCtx);
        newFrame = true;
    }       

    return ret;
}

int AudioDecoder::setOutputFmt(const OutputFmt& settings)
{
    int ret = 0;
    // Free the buffer if channels or sample format is changed as the buffer will no longer be valid
    if ((settings.chLayout != swrOutput.chLayout || settings.sampleFmt != swrOutput.sampleFmt) &&
        sampleBuff.data)
    {
        sampleBuff.numAlloc = 0;
        sampleBuff.numSamples = 0;
        av_freep(&sampleBuff.data);
    }

    swrOutput = settings;

    av_opt_set_channel_layout(swr, "out_channel_layout", swrOutput.chLayout, 0);
    av_opt_set_int(swr, "out_sample_rate", swrOutput.sampleRate, 0);
    av_opt_set_sample_fmt(swr, "out_sample_fmt", swrOutput.sampleFmt, 0);
    ret = swr_init(swr);

    return ret;
}

double AudioDecoder::duration()
{
    if (!inFmtCtx && inStreamIndex < 0)
        return -1.0;

    AVStream* stream = inFmtCtx->streams[inStreamIndex];
    return stream->duration * stream->time_base.num / stream->time_base.den;
}

int AudioDecoder::receiveSamples()
{
    // Feels like a mess sadly.
    int ret = 0;
    // If not done with the frame jump to it. The only goto don't worry :)
    // neFrame is only set true on init, successful seek and open file.
    if (!newFrame)
        goto frameProcessing;
    
    newFrame = false;
    // read_frame error - exit immediately
    while (!(ret = av_read_frame(inFmtCtx, &inPacket)))
    {
        // Process only the needed stream
        if (inPacket.stream_index == inStreamIndex)
        {
            // On negative error fall through to the unref and return the error
            if (!(ret = avcodec_send_packet(inCodecCtx, &inPacket)))
            {
/*Goto->*/  frameProcessing:
                ret = avcodec_receive_frame(inCodecCtx, inFrame);
                // No error - process further
                if (ret >= 0)
                {
                    ret = processFrame();
                    av_frame_unref(inFrame);

                    // On positive - samples have been read, the job is done
                    if (ret >= 0)
                        break;

                } // EAGAIN and EOF are fine, change ret to 0 to read the next frame
                else if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
                    ret = 0;
            }
        }
        // In loop only on successful read_frame - need to unref the packet
        av_packet_unref(&inPacket);

        // If inner branches return an error - exit
        if (ret < 0)
            break;
    }
    return ret;
}

int AudioDecoder::processFrame()
{
    int outSamples = swr_get_out_samples(swr, inFrame->nb_samples);
    // If buffer is too small or not allocated, re/allocate
    if (!sampleBuff.data || (sampleBuff.numAlloc < outSamples))
    {
        if (sampleBuff.data)
            av_freep(&sampleBuff.data);

        int err = av_samples_alloc_array_and_samples(
            &sampleBuff.data,
            NULL,
            av_get_channel_layout_nb_channels(swrOutput.chLayout),
            outSamples,
            swrOutput.sampleFmt,
            0);

        sampleBuff.numAlloc = outSamples;
        if (err < 0)
            return err;
    }

    outSamples = swr_convert(
        swr,
        sampleBuff.data,
        sampleBuff.numAlloc,
        const_cast<const uint8_t**>(inFrame->extended_data),
        inFrame->nb_samples);

    return sampleBuff.numSamples = outSamples;
}
