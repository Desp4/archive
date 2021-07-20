using System;
using System.Drawing;
using System.IO;

namespace PictoAudio.Utils
{
    static class MyMath
    {
        public static double Lerp(int a, int b, double lerp)
        {
            return a + (b - a) * lerp;
        }
    }

    static class Graphics
    {
        /// <summary>
        /// Returns RGB color from HSV.
        /// </summary>
        /// <param name="hue">Hue value in range [0; 360].</param>
        /// <param name="saturation">Saturation value in range [0; 1].</param>
        /// <param name="value">Value(Blackness) value in range [0; 1].</param>
        /// <returns>System.Drawing.Color</returns>
        public static Color ColorFromHSV(double hue, double saturation, double value)
        {
            int hueSegment = Convert.ToInt32(Math.Floor(hue / 60)) % 6;
            double hueLocal = hue / 60 - Math.Floor(hue / 60);

            value *= 255;
            int ch1 = Convert.ToInt32(value);
            int ch2 = Convert.ToInt32(value * (1 - saturation));
            int ch3 = Convert.ToInt32(value * (1 - hueLocal * saturation));
            int ch4 = Convert.ToInt32(value * (1 - (1 - hueLocal) * saturation));

            if (hueSegment == 0)
                return Color.FromArgb(255, ch1, ch4, ch2);
            if (hueSegment == 1)
                return Color.FromArgb(255, ch3, ch1, ch2);
            if (hueSegment == 2)
                return Color.FromArgb(255, ch2, ch1, ch4);
            if (hueSegment == 3)
                return Color.FromArgb(255, ch2, ch3, ch1);
            if (hueSegment == 4)
                return Color.FromArgb(255, ch4, ch2, ch1);
            return Color.FromArgb(255, ch1, ch2, ch3);
        }

        public static void OptimalResize(ref Bitmap bitmap, long maxPixelCount)
        {
            double scaleFactor = Math.Sqrt((double)maxPixelCount / (bitmap.Height * bitmap.Width));
            if (scaleFactor < 1)
                return;

            bitmap = new Bitmap(bitmap, new Size(
                (int)(bitmap.Width * scaleFactor), (int)(bitmap.Height * scaleFactor)));
        }

        public static double OptimalResizeScale(int width, int height, long maxPixelCount)
        {
            if (width * height < maxPixelCount)
                return 1;
            return Math.Sqrt((double)maxPixelCount / (height * width));
        }
    }

    static class MyIO
    {
        public static string GetSafeName(string path)
        {
            return GetSafeName(Path.GetDirectoryName(path), 
                Path.GetFileNameWithoutExtension(path), Path.GetExtension(path));
        }

        public static string GetSafeName(string directory, string name, string extension)
        {
            string fullPath = directory + "\\" + name + extension;
            int attempt = 1;
            while (File.Exists(fullPath))
            {
                fullPath = directory + "\\" + name + " (" + attempt.ToString() + ")" + extension;
                attempt++;
            }

            return fullPath;
        }
    }
}