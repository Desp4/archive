using System;

namespace PictoAudio
{
    class Hilbert : SFC
    {
        int curvePos;

        public Hilbert(int w, int h) : base(w, h) { }

        public override void Generate(int w, int h)
        {
            if (w < 1 || h < 1)
            {
                throw new ArgumentException("Parameters are not positive.");
            }

            Curve = new VectorXY[w * h];
            curvePos = 0;
            if (w < h)
                HilbertMain(0, 0, 0, h, w, 0);
            else
                HilbertMain(0, 0, w, 0, 0, h);
        }

        // Credits to https://github.com/jakubcerveny/gilbert/blob/master/gilbert2d.py
        void HilbertMain(int x, int y, int ax, int ay, int bx, int by)
        {
            int w = Math.Abs(ax + ay);
            int h = Math.Abs(bx + by);

            // major direction
            int dax = Math.Sign(ax);
            int day = Math.Sign(ay);

            // orthogonal direction
            int dbx = Math.Sign(bx);
            int dby = Math.Sign(by);

            if (h == 1)
            {
                // trivial row
                for(; w > 0; w--)
                {
                    Curve[curvePos] = new VectorXY(x, y);
                    curvePos++;
                    x += dax;
                    y += day;
                }
                return;
            }
            if (w == 1)
            {

                for (; h > 0; h--)
                {
                    Curve[curvePos] = new VectorXY(x, y);
                    curvePos++;
                    x += dbx;
                    y += dby;
                }
                return;
            }

            int ax2 = ax / 2;
            int ay2 = ay / 2;

            int bx2 = bx / 2;
            int by2 = by / 2;

            int w2 = Math.Abs(ax2 + ay2);
            int h2 = Math.Abs(bx2 + by2);

            if (2 * w > 3 * h)
            {
                if (w2 % 2 != 0 && w > 2)
                {

                    ax2 += dax;
                    ay2 += day;
                }


                HilbertMain(x, y, ax2, ay2, bx, by);
                HilbertMain(x + ax2, y + ay2, ax - ax2, ay - ay2, bx, by);
            }
            else
            {
                if (h2 % 2 != 0 && h > 2)
                {

                    bx2 += dbx;
                    by2 += dby;
                }

                // standart case : one step up, one along horizontal, one step down
                HilbertMain(x, y, bx2, by2, ax2, ay2);
                HilbertMain(x + bx2, y + by2, ax, ay, bx - bx2, by - by2);
                HilbertMain(x + (ax - dax) + (bx2 - dbx), y + (ay - day) + (by2 - dby),
                    -bx2, -by2, -(ax - ax2), -(ay - ay2));
            }
        }
    }
}
