using System.IO;

namespace PictoAudio.WaveIO
{
    class WAV
    {
        public Internal.WAVHeader Header { get; private set; }
        public Internal.WAVFormatChunk Format { get; private set; }
        public Internal.WAVDataChunk Data { get; private set; }

        // add params to pass to constructors later if needed
        public WAV(ushort sampleRate, ushort channels)
        {
            Header = new Internal.WAVHeader();
            Format = new Internal.WAVFormatChunk(sampleRate, channels);
            Data = new Internal.WAVDataChunk();
        }

        public void Save(string path)
        {
            FileStream fileStream = new FileStream(path, FileMode.Create);

            BinaryWriter writer = new BinaryWriter(fileStream);

            // header
            writer.Write(Header.sGroupID.ToCharArray());
            writer.Write(Header.dwFileLength);
            writer.Write(Header.sRiffType.ToCharArray());

            // format chunk
            writer.Write(Format.sChunkID.ToCharArray());
            writer.Write(Format.dwChunkSize);
            writer.Write(Format.wFormatTag);
            writer.Write(Format.wChannels);
            writer.Write(Format.dwSamplesPerSec);
            writer.Write(Format.dwAvgBytesPerSec);
            writer.Write(Format.wBlockAlign);
            writer.Write(Format.wBitsPerSample);

            // data chunk
            writer.Write(Data.sChunkID.ToCharArray());
            writer.Write(Data.dwChunkSize);
            foreach (short dataSample in Data.dataArray)
                writer.Write(dataSample);

            writer.Seek(4, SeekOrigin.Begin);
            uint filesize = (uint)writer.BaseStream.Length;
            writer.Write(filesize - 8);

            writer.Close();
            fileStream.Close();
        }
    }
}
