using System;
using System.Drawing;
using System.IO;
using System.Threading.Tasks;
using Accord.Video.FFMPEG;

namespace PictoAudio
{
    class PicProcessor
    {
        ProcessorSettings settings;
        Bitmap bitmap;
        SFC sfc;
        SFCColorData colorSFCData;
        VideoFileReader videoReader;
        string filePath;

        public ProcState State { get; private set; }

        public PicProcessor() { }

        // can handle multiple calls but may leave some garbage behind
        public void Initialize(string path, ProcessorSettings procSettings)
        {
            settings = procSettings;
            int w = 0, h = 0;
            try
            {
                // try opening as image first
                bitmap = new Bitmap(path);

                // if resizing
                if (settings.compressImage)
                    Utils.Graphics.OptimalResize(ref bitmap, settings.compressPixelCount);

                w = bitmap.Width;
                h = bitmap.Height;
                State = ProcState.Initialized | ProcState.Image;
            }
            catch (ArgumentException)
            {
                videoReader = new VideoFileReader();
                try
                {
                    // try opening a video
                    videoReader.Open(path);

                    // if resizing
                    double scaleFactor = 1;
                    if (settings.compressImage)
                        scaleFactor = Utils.Graphics.OptimalResizeScale(
                            videoReader.Width, videoReader.Height, settings.compressPixelCount);

                    w = (int)(videoReader.Width * scaleFactor);
                    h = (int)(videoReader.Height * scaleFactor);
                    State = ProcState.Initialized | ProcState.Video;
                }
                catch
                {
                    Console.WriteLine("Could not open " + path);
                    return;
                }
            }

            GenerateCurve(w, h);
            filePath = path;
        }

        public void SoundFromVideo()
        {
            if ((State & ProcState.Initialized) != ProcState.Initialized ||
                (State & ProcState.Converted) != ProcState.Converted ||
                (State & ProcState.Video) != ProcState.Video)
                return;

            WaveIO.WAV wavFile = new WaveIO.WAV(settings.wavSettings.sampleRate, settings.wavSettings.channels);
            // in a channel
            uint samplesPerFrame = (uint)(wavFile.Format.dwSamplesPerSec / (double)videoReader.FrameRate);
            wavFile.Data.dataArray = new short[wavFile.Format.wChannels * samplesPerFrame * videoReader.FrameCount];

            // choose an algorithm here
            switch (settings.colorMethod)
            {
                case ColorMethod.BlackWhite:
                    colorSFCData = new SFCBlackWhiteColor(sfc, wavFile.Format.dwSamplesPerSec, wavFile.Format.wChannels,
                        settings.lowerFrequency, settings.higherFrequency);
                    break;
            }

            colorSFCData.CalculateSinTable(samplesPerFrame, wavFile.Format.wChannels);

            // calculate sound
            for (int frame = 0; frame < videoReader.FrameCount; frame++)
            {
                Bitmap currFrame = videoReader.ReadVideoFrame(frame);
                if (settings.compressImage)
                    Utils.Graphics.OptimalResize(ref currFrame, settings.compressPixelCount);

                colorSFCData.CalculateColor(sfc, currFrame);

                long frameSampleChunk = samplesPerFrame * wavFile.Format.wChannels * frame;
                Parallel.For(0, samplesPerFrame, i =>
                {
                    short sample = colorSFCData.InterpretSinTable(i);
                    for (int channel = 0; channel < wavFile.Format.wChannels; channel++)
                        wavFile.Data.dataArray[frameSampleChunk + wavFile.Format.wChannels * i + channel] = sample;
                });
            }

            // calculate data chunk size
            wavFile.Data.dwChunkSize = (uint)(wavFile.Data.dataArray.Length * (wavFile.Format.wBitsPerSample / 8));

            string safePath = Utils.MyIO.GetSafeName(Path.GetDirectoryName(filePath),
                Path.GetFileNameWithoutExtension(filePath), ".wav");
            wavFile.Save(safePath);
        }

        public void SoundFromPicture(uint length)
        {
            if ((State & ProcState.Initialized) != ProcState.Initialized ||
                (State & ProcState.Converted) != ProcState.Converted ||
                (State & ProcState.Image) != ProcState.Image)
                return;

            WaveIO.WAV wavFile = new WaveIO.WAV(settings.wavSettings.sampleRate, settings.wavSettings.channels);

            uint samplesPerChannel = length * wavFile.Format.dwSamplesPerSec;
            wavFile.Data.dataArray = new short[samplesPerChannel * wavFile.Format.wChannels];

            // choose an algorithm here
            switch(settings.colorMethod)
            {
                case ColorMethod.BlackWhite:
                    colorSFCData = new SFCBlackWhiteColor(sfc, wavFile.Format.dwSamplesPerSec, wavFile.Format.wChannels,
                        settings.lowerFrequency, settings.higherFrequency);
                    break;
            }

            colorSFCData.CalculateColor(sfc, bitmap);

            // calculate sound
            Parallel.For(0, samplesPerChannel, i =>
            {
                short sample = colorSFCData.Interpret(wavFile.Format.wChannels * i);
                for (int channel = 0; channel < wavFile.Format.wChannels; channel++)
                    wavFile.Data.dataArray[wavFile.Format.wChannels * i + channel] = sample;
            });

            // calculate data chunk size
            wavFile.Data.dwChunkSize = (uint)(wavFile.Data.dataArray.Length * (wavFile.Format.wBitsPerSample / 8));

            string safePath = Utils.MyIO.GetSafeName(Path.GetDirectoryName(filePath),
                Path.GetFileNameWithoutExtension(filePath), ".wav");
            wavFile.Save(safePath);
        }   

        void GenerateCurve(int width, int height)
        {
            if ((State & ProcState.Initialized) != ProcState.Initialized)
                return;

            // only hilbert at this point
            switch(settings.curveType)
            {
                case CurveType.Hilbert:
                    sfc = new Hilbert(width, height);
                    break;
            }

            State |= ProcState.Converted;
        }
    }
}
