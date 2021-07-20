namespace PictoAudio.WaveIO.Internal
{
    class WAVHeader
    {
        public string sGroupID = "RIFF"; // RIFF
        public string sRiffType = "WAVE"; // WAVE

        public uint dwFileLength; // file size minus 8(header)

        public WAVHeader()
        {
            dwFileLength = 0;                    
        }
    }

    class WAVFormatChunk
    {
        public string sChunkID = "fmt "; // four bytes "fmt "
        public ushort wFormatTag = 1; // always 1 for WAV(indicates PCM)

        public uint dwChunkSize; // header length
        public ushort wChannels; // number of channels
        public uint dwSamplesPerSec; // frequency in Hz
        public uint dwAvgBytesPerSec; // number of frames per second
        public ushort wBlockAlign; // sample frame size
        public ushort wBitsPerSample; // bit depth

        public WAVFormatChunk(ushort sampleRate, ushort channels)
        {            
            dwChunkSize = 16;
            wChannels = channels;
            dwSamplesPerSec = sampleRate;
            wBitsPerSample = 16;
            wBlockAlign = (ushort)(wChannels * (wBitsPerSample / 8));
            dwAvgBytesPerSec = dwSamplesPerSec * wBlockAlign;
        }
    }

    class WAVDataChunk
    {
        public string sChunkID = "data"; // "data"

        public uint dwChunkSize; // header length
        public short[] dataArray; // 16-bit audio

        public short this[int i]
        {
            get => dataArray[i];
        }

        public WAVDataChunk()
        {
            dataArray = new short[0];
            dwChunkSize = 0;
        }
    }
}
