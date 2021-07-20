namespace PictoAudio
{
    struct VectorXY
    {
        public VectorXY(int x, int y)
        {
            W = x;
            H = y;
        }

        public int W { get; private set; }
        public int H { get; private set; }
    }

    abstract class SFC
    {
        public VectorXY[] Curve { get; protected set; }
        public int Size { get => Curve.Length; }

        public VectorXY this[int i]
        {
            get => Curve[i];
        }

        public void Dispose()
        {
            Curve = new VectorXY[0];
        }

        public static explicit operator VectorXY[](SFC sfc)
        {
            return sfc.Curve;
        }

        protected SFC(int w, int h)
        {
            Generate(w, h);
        }

        public abstract void Generate(int w, int h);
    }
}
