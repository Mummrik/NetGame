using System;
using System.Collections.Generic;
using System.Threading.Tasks;
using FlaxEngine;

namespace Game
{
    public class RpcManager
    {
        private Dictionary<PacketType, Action<Network, NetworkMessage>> m_RpcList;

        public RpcManager()
        {
            m_RpcList = new Dictionary<PacketType, Action<Network, NetworkMessage>>();
            m_RpcList.Add(PacketType.Disconnect, Disconnect);
            m_RpcList.Add(PacketType.HandShake, HandShake);
            m_RpcList.Add(PacketType.Ping, Ping);
        }

        public void Invoke(Network network, NetworkMessage msg)
        {
            PacketType type = msg.GetPacketType();
            if (m_RpcList.ContainsKey(type))
            {
                if (msg.GetSequenceId() > 0)
                {
                    network.SendAck(msg.GetSequenceId());
                }
                Task.Run(() => m_RpcList[type].Invoke(network, msg)).Wait();
            }

            //Debug.Log($"[UDP] peerId: {network.GetId()}, Received type: {type}, SequenceId: {msg.GetSequenceId()}");
        }

        private void Disconnect(Network network, NetworkMessage msg)
        {
            //network.Disconnect();
        }

        private void HandShake(Network network, NetworkMessage msg)
        {
            uint id = msg.ReadUInt();
            ulong time = msg.ReadULong();
            network.SetId(id);

            msg = new NetworkMessage(PacketType.HandShake);
            msg.Write(time);
            network.Send(msg);
        }

        private void Ping(Network network, NetworkMessage msg)
        {
            ulong latency = msg.ReadULong();
            ulong time = msg.ReadULong();
            network.latency = latency;
            //Debug.Log($"Latency: {network.latency}");

            msg = new NetworkMessage(PacketType.Ping);
            msg.Write(time);
            network.Send(msg);
        }
    }
}
