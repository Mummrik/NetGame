using System;
using System.Collections.Generic;
using System.Net;
using System.Net.Sockets;
using System.Threading;
using System.Threading.Tasks;
using FlaxEngine;

namespace Game
{
    /// <summary>
    /// Network Script.
    /// </summary>
    public class Network : Script
    {
        public string host = "127.0.0.1";
        public ushort port = 7171;

        private RpcManager m_Rpc;
        private uint m_Id;
        private static UdpClient m_UdpClient;
        private IPEndPoint m_EndPoint;
        private List<uint> m_RecentMsgs = new List<uint>();
        private int m_ReceivedBytes;
        private int m_SentBytes;

        public ulong latency;

        public uint GetId() => m_Id;
        public void SetId(uint id)
        {
            if (m_Id != 0)
                return;

            m_Id = id;
        }
        public override void OnAwake()
        {
            m_Rpc = new RpcManager();

            m_EndPoint = new IPEndPoint(IPAddress.Parse(host), port);
            m_UdpClient = new UdpClient();
            m_UdpClient.Connect(m_EndPoint);

            m_UdpClient.BeginReceive(Listen, m_UdpClient);

            NetworkMessage msg = new NetworkMessage(PacketType.HandShake);
            Send(msg);
        }

        public override void OnDestroy()
        {
            Task.Run(() =>
            {
                NetworkMessage msg = new NetworkMessage(PacketType.Disconnect);
                msg.Write(m_Id);
                Send(msg);
            }).Wait();
        }

        private void Listen(IAsyncResult ar)
        {
            byte[] data = m_UdpClient.EndReceive(ar, ref m_EndPoint);

            if (data.Length > 0)
            {
                m_ReceivedBytes += data.Length;
                Debug.Log($"Recv total: {m_ReceivedBytes} bytes | {(m_ReceivedBytes * 0.008f):0.00}k/bit || Sent total: {m_SentBytes} bytes | {(m_SentBytes * 0.008f):0.00}k/bit");
                NetworkMessage msg = new NetworkMessage(data);
                //Debug.Log($"Recv Type: {msg.GetPacketType()} Sequence: {msg.GetSequenceId()}");
                if (m_RecentMsgs.Contains(msg.GetSequenceId()))
                {
                    SendAck(msg.GetSequenceId());
                }
                else
                {
                    m_Rpc.Invoke(this, msg);
                }
                msg.Dispose();
            }

            m_UdpClient.BeginReceive(Listen, m_UdpClient);
        }

        public void Send(in NetworkMessage msg, bool reliable = false)
        {
            if (reliable)
            {
                // TODO: Store msg
            }

            if (msg.GetSequenceId() == 0)
            {
                msg.AddSequenceId(0);
            }
            List<byte> data = msg.GetData();
            m_SentBytes += data.Count;
            Task.Run(() => { m_UdpClient.BeginSend(data.ToArray(), data.Count, (ar) => m_UdpClient.EndSend(ar), m_UdpClient); });
        }

        public void SendAck(uint sequence_id)
        {
            if (!m_RecentMsgs.Contains(sequence_id))
            {
                m_RecentMsgs.Add(sequence_id);
            }

            NetworkMessage msg = new NetworkMessage(PacketType.Acknowledge);
            msg.AddSequenceId(sequence_id);
            Send(msg);
        }

        public void Disconnect()
        {
            m_UdpClient.Client.Shutdown(SocketShutdown.Both);
            m_UdpClient.Close();
        }
    }
}
