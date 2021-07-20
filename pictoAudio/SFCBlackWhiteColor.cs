using System.Drawing;

namespace PictoAudio
{
    class SFCBlackWhiteColor : SFCColorData
    {
        public SFCBlackWhiteColor(SFC curve, uint sampleRate, ushort channels, 
            int lowerFreq, int upperFreq) :
            base(curve, sampleRate, channels, lowerFreq, upperFreq)
        { }

        public override void CalculateColor(SFC curve, Bitmap bitmap)
        {
            if (curve.Size != ColorValues.Length)
                ColorValues = new double[curve.Size];

            for (int i = 0; i < curve.Size; i++)
                ColorValues[i] = bitmap.GetPixel(curve[i].W, curve[i].H).GetBrightness();
        }
    }
}
