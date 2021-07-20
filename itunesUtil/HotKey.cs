using System;
using System.Windows.Forms;

namespace itunesUtil
{
    [Serializable]
    public class HotKey
    {
        public int mod;
        public Keys key;
        public int id;
        public int action;
    }
}
