using System;
using System.Drawing;
using System.Threading.Tasks;

namespace PictoAudio
{
    // doesn't provide an interface for setting a curve manualy, create a new instance if you need that
    abstract class SFCColorData
    {
        protected int curveSize = 0;

        public double[] Frequencies { get; protected set; }
        public double[] ColorValues { get; protected set; }
        // don't need doubles - too large [sample, curvepos]
        public float[,] SinTable { get; protected set; }

        protected SFCColorData(SFC curve, uint sampleRate, ushort channels,  int lowerFreq, int upperFreq)
        {
            curveSize = curve.Size;
            CalculateFrequencies(curve.Size, sampleRate, channels, lowerFreq, upperFreq);
        }

        public void CalculateSinTable(uint sampleCount, int channelCount)
        {
            SinTable = new float[sampleCount, curveSize];
            Parallel.For(0, sampleCount, i =>
            {
                for (int curvePos = 0; curvePos < curveSize; curvePos++)
                    SinTable[i, curvePos] = (float)Math.Sin(Frequencies[curvePos] * i * channelCount);
            });
        }

        // 32760 max amplitude for 16bit sound
        public short Interpret(long index, int amplitude = 32760)
        {
            double sample = 0;

            for (int i = 0; i < curveSize; i++)
                sample += ColorValues[i] * Math.Sin(Frequencies[i] * index);

            sample = sample * amplitude / curveSize;

            return (short)sample;
        }

        public short InterpretSinTable(long index, int amplitude = 32760)
        {
            double sample = 0;

            for (int i = 0; i < curveSize; i++)
                sample += ColorValues[i] * SinTable[index, i];

            sample = sample * amplitude / curveSize;

            return (short)sample;
        }

        public abstract void CalculateColor(SFC curve, Bitmap bitmap);

        protected void CalculateFrequencies(int length, uint sampleRate, ushort channels, int lowerFreq, int upperFreq)
        {
            Frequencies = new double[length];

            double trigConst = 2 * Math.PI / (sampleRate * channels);
            for (int i = 0; i < length; i++)
                Frequencies[i] = trigConst * Utils.MyMath.Lerp(lowerFreq, upperFreq, (double)i / length);
        }
    }
}
