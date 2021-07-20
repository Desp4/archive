using System.IO;
using System.Runtime.Serialization.Formatters.Binary;

namespace itunesUtil
{
    public static class Serializator
    {
        private static BinaryFormatter binFmt = new BinaryFormatter();

        public static void Serialize(string pathOrFileName, object objToSerialise)
        {
            using (Stream stream = File.Open(pathOrFileName, FileMode.Create))
            {
                binFmt.Serialize(stream, objToSerialise);
            }
        }

        public static T Deserialize<T>(string pathOrFileName)
        {
            using (Stream stream = File.Open(pathOrFileName, FileMode.Open))
            {
                return (T)binFmt.Deserialize(stream);
            }
        }
    }
}
