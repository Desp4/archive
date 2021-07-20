using System;

namespace PictoAudio
{
    class Program
    {
        static ProcessorSettings DefaultSettings()
        {
            ProcessorSettings settings;
            settings.curveType = CurveType.Hilbert;
            settings.colorMethod = ColorMethod.BlackWhite;
            settings.wavSettings = new WAVSettings(44100, 2);
            settings.lowerFrequency = 200;
            settings.higherFrequency = 15000;
            settings.compressImage = false;
            settings.compressPixelCount = 0;
            return settings;
        }

        static void Main(string[] args)
        {
            // default settings only because don't really care
            PicProcessor processor = new PicProcessor();
            if (args.Length != 0)
                processor.Initialize(args[0], DefaultSettings());

            while ((processor.State & ProcState.Initialized) != ProcState.Initialized)
                processor.Initialize(Console.ReadLine(), DefaultSettings());

            processor.SoundFromVideo();
        }
    }
}
