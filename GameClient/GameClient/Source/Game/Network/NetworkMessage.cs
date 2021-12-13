using System;
using System.Collections.Generic;
using System.Text;
using FlaxEngine;

namespace Game
{
    public enum PacketType : ushort
    {
        Disconnect,
        HandShake,
        Acknowledge,
        Ping,
        MAX_SIZE    // Has to be last
    }

    public class NetworkMessage : IDisposable
    {
        private List<byte> m_Data;
        private uint m_SequenceId = 0;
        private PacketType m_Type;
        private int m_Index = 0;
        private int m_Size = 0;

        public NetworkMessage(in PacketType type)
        {
            m_Data = new List<byte>();
            m_Type = type;
            Write((ushort)m_Type);
        }

        public NetworkMessage(in byte[] data)
        {
            m_Data = new List<byte>(data);
            m_Size = (ushort)data.Length;
            m_SequenceId = ReadUInt();
            m_Type = (PacketType)ReadUShort();
        }

        public PacketType GetPacketType() => m_Type;
        public List<byte> GetData() => m_Data;
        public int GetSize() => m_Size;
        public uint GetSequenceId() => m_SequenceId;

        public void AddSequenceId(uint id)
        {
            m_SequenceId = id;

            m_Data.InsertRange(0, BitConverter.GetBytes(m_SequenceId));

            m_Size = m_Data.Count;
            m_Index = m_Size;
        }
        public void Write(byte value)
        {
            m_Data.Add(value);
            ++m_Index;
            m_Size = m_Data.Count;
        }

        public void Write(byte[] values)
        {
            foreach (byte value in values)
            {
                Write(value);
            }
        }

        public void Write(short value) => Write(BitConverter.GetBytes(value));
        public void Write(ushort value) => Write(BitConverter.GetBytes(value));
        public void Write(int value) => Write(BitConverter.GetBytes(value));
        public void Write(uint value) => Write(BitConverter.GetBytes(value));
        public void Write(long value) => Write(BitConverter.GetBytes(value));
        public void Write(ulong value) => Write(BitConverter.GetBytes(value));
        public void Write(bool value) => Write(BitConverter.GetBytes(value));
        public void Write(float value) => Write(BitConverter.GetBytes(value));
        public void Write(double value) => Write(BitConverter.GetBytes(value));
        public void Write(Guid value) => Write(value.ToByteArray());

        public void Write(string value)
        {
            byte[] str = Encoding.Unicode.GetBytes(value);
            Write(str.Length);
            Write(str);
        }
        public void Write(Vector3 value)
        {
            Write(BitConverter.GetBytes(value.X));
            Write(BitConverter.GetBytes(value.Y));
            Write(BitConverter.GetBytes(value.Z));
        }
        public void Write(Vector2 value)
        {
            Write(BitConverter.GetBytes(value.X));
            Write(BitConverter.GetBytes(value.Y));
        }
        public void Write(Quaternion value)
        {
            Write(BitConverter.GetBytes(value.X));
            Write(BitConverter.GetBytes(value.Y));
            Write(BitConverter.GetBytes(value.Z));
            Write(BitConverter.GetBytes(value.W));
        }

        public byte ReadByte()
        {
            return m_Data[m_Index++];
        }
        public byte[] ReadBytes(int length)
        {
            byte[] bytes = new byte[length];
            Array.Copy(m_Data.ToArray(), m_Index, bytes, default, length);
            m_Index += (ushort)length;
            return bytes;
        }
        public short ReadShort() => BitConverter.ToInt16(ReadBytes(sizeof(short)), default);
        public ushort ReadUShort() => BitConverter.ToUInt16(ReadBytes(sizeof(ushort)), default);
        public int ReadInt() => BitConverter.ToInt32(ReadBytes(sizeof(int)), default);
        public uint ReadUInt() => BitConverter.ToUInt32(ReadBytes(sizeof(uint)), default);
        public long ReadLong() => BitConverter.ToInt64(ReadBytes(sizeof(long)), default);
        public ulong ReadULong() => BitConverter.ToUInt64(ReadBytes(sizeof(ulong)), default);
        public bool ReadBool() => BitConverter.ToBoolean(ReadBytes(sizeof(bool)), default);
        public float ReadFloat() => BitConverter.ToSingle(ReadBytes(sizeof(float)), default);
        public double ReadDouble() => BitConverter.ToDouble(ReadBytes(sizeof(double)), default);
        public Guid ReadGuid() => new Guid(ReadBytes(16));
        public string ReadString() => Encoding.Unicode.GetString(ReadBytes(sizeof(int)));
        public Vector3 ReadVector3() => new Vector3(ReadFloat(), ReadFloat(), ReadFloat());
        public Vector2 ReadVector2() => new Vector2(ReadFloat(), ReadFloat());
        public Quaternion ReadQuaternion() => new Quaternion(ReadFloat(), ReadFloat(), ReadFloat(), ReadFloat());

        private bool isDisposed;
        protected virtual void Dispose(bool disposing)
        {
            if (isDisposed)
                return;

            if (disposing)
            {
                m_Data = null;
                m_Type = 0;
                m_Index = 0;
                m_Size = 0;
            }
            isDisposed = true;
        }
        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }
    }
}
