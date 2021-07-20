using System;

namespace PictoAudio
{
    [Flags]
    enum ProcState
    {
        Empty = 0,
        Initialized = 1,
        Converted = 2,
        Image = 4,
        Video = 8
    }

    struct ProcessorSettings
    {
        public CurveType curveType;
        public ColorMethod colorMethod;
        public WAVSettings wavSettings;
        public int lowerFrequency;
        public int higherFrequency;
        public bool compressImage;
        public long compressPixelCount;
    }

    struct WAVSettings
    {
        public ushort sampleRate;
        public ushort channels;

        public WAVSettings(ushort samplePerSecond, ushort channelCount)
        {
            sampleRate = samplePerSecond;
            channels = channelCount;
        }
    }

    enum CurveType
    {
        Hilbert
    }

    enum ColorMethod
    {
        BlackWhite
    }
}