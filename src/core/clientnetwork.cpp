#include "clientnetwork.h"


ClientNetwork::ClientNetwork()
	: Network()
{
	
	m_status = NET_CLOSE;
}

ClientNetwork::~ClientNetwork()
{
}

void ClientNetwork::serverConnect( char* hostname, int req_port )
{
	m_peer ->Connect(hostname, req_port, 0, 0);
	
}

void ClientNetwork::serverDisconnect()
{
	DEBUG("closing connection");
	m_peer->CloseConnection(m_server_address,true);
	// warten, damit die disconnect meldung ankommt
#ifdef WIN32
	Sleep( 30 );
#else
	usleep(30000 );
#endif
	m_status = NET_CLOSE;
}




void ClientNetwork::update()
{
	static int nr=0;
	Packet* packet = m_peer->Receive();
	unsigned char id;
	while (packet !=0)
	{
		nr ++;
		DEBUG5("packets allocated: %i",nr);
		id = getPacketIdentifier(packet);
		
		if (id <  ID_USER_PACKET_ENUM)
		{
			switch(id)
			{
				case ID_ALREADY_CONNECTED:
				case ID_CONNECTION_REQUEST_ACCEPTED:
					m_status = NET_CONNECTED;
					while (!m_received_packets.empty())
						m_received_packets.pop();
					m_server_address = packet->systemAddress;
					DEBUG("connection accepted");
					break;
					
				case ID_CONNECTION_ATTEMPT_FAILED:
					DEBUG("connection refused");
					m_status =NET_REJECTED;
					m_server_address = UNASSIGNED_SYSTEM_ADDRESS;
					break;
					
				case ID_NO_FREE_INCOMING_CONNECTIONS:
					DEBUG("no free slots on server");
					m_status =NET_SLOTS_FULL;
					m_server_address = UNASSIGNED_SYSTEM_ADDRESS;
					break;
				
				case ID_CONNECTION_LOST:
				case ID_DISCONNECTION_NOTIFICATION:
					DEBUG("connection timed out");
					m_status =NET_TIMEOUT;
					m_server_address = UNASSIGNED_SYSTEM_ADDRESS;
					break;
					
				default:
					DEBUG("unknown id: %i",id);
					break;
			}
			
			deallocatePacket(packet);
		}
		else
		{
			
			DEBUG5("packet received");
			m_received_packets.push(packet);

		}
		packet = m_peer->Receive();
	}
}


int ClientNetwork::numberSlotMessages()
{
	return m_received_packets.size();
}


void ClientNetwork::popSlotMessage( Packet* &data)
{
	data = m_received_packets.front();
	m_received_packets.pop();
}

void ClientNetwork::pushSlotMessage( RakNet::BitStream * data,PacketPriority prio,PacketReliability reliability)
{
	m_peer->Send(data,prio,reliability , 0,m_server_address, false);
}

NetStatus ClientNetwork::getSlotStatus()
{
	return m_status;
}

void ClientNetwork::deallocatePacket(Packet* packet)
{
	static int nr =0;
	nr++;
	DEBUG5("packets deallocated: %i",nr);
	m_peer->DeallocatePacket(packet);
}

