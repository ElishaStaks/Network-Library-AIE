#include "Server.h"
#include <string>

Server::Server()
{
	m_shouldRun.store(true);
}


Server::~Server()
{
}

void Server::StartUp()
{
	//Startup the server, and start it listening to clients
	std::cout << "Starting up the server..." << std::endl;
	
	unsigned short PORT;
	std::cout << "Enter Port Address: ";
	std::cin >> PORT;

	//Initialize the Raknet peer interface first
	pPeerInterface = RakNet::RakPeerInterface::GetInstance();
	//Create a socket descriptor to describe this connection
	RakNet::SocketDescriptor sd(PORT, 0);
	//Now call startup - max of 32 connections, on the assigned port
	pPeerInterface->Startup(32, &sd, 1);
	pPeerInterface->SetMaximumIncomingConnections(32);
	// HandleNetworkMessages();

	m_packetThread = std::thread(&Server::HandleNetworkMessages);
}

void Server::ShutDown()
{
	m_shouldRun.store(false);
	m_packetThread.join();
}

void Server::Run()
{
	std::cin.get();
	ShutDown();
}

// @brief Checks to see which case is true in the switch statment and sends to the console the correct 
// message based on the packet data.
void Server::HandleNetworkMessages()
{
	// Chunk of data that will be sent over the network.
	RakNet::Packet* packet = nullptr;
	while (m_shouldRun.load()) {
		// Recieves packet of data.
		for (packet = pPeerInterface->Receive(); packet;
			pPeerInterface->DeallocatePacket(packet),
			packet = pPeerInterface->Receive())
		{
			switch (packet->data[0])
			{
				// If a new client connects to the server.
			case ID_NEW_INCOMING_CONNECTION:
				std::cout << "A connection is successful :)\n" << std::endl;
				SendNewClientID(packet->systemAddress);
				break;

				// If connection has disconnected.
			case ID_DISCONNECTION_NOTIFICATION:
				std::cout << "A client has disconnected :(\n" << std::endl;;
				break;

				// If connection is lost.
			case ID_CONNECTION_LOST:
				std::cout << "A connection has been lost :(\n" << std::endl;;
				break;

				// Reads the packet data into a BitStream, and then broadcasts it back to all other clients.
			case ID_CLIENT_CLIENT_DATA:
			{
				RakNet::BitStream bs(packet->data, packet->length, false);
				pPeerInterface->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0,
					packet->systemAddress, true);
				// Read the packet and store in our list
				GameObject clientData;
				clientData.Read(packet);
				m_gameObjects[clientData.GetID()] = clientData;
				break;
			}
			// Default message to the server
			default:
				std::cout << "Recieved a connection with an unknown ID: " << packet->data[0];
				break;
			}
		}
	}
}

// @brief Send the client ID back to the client when it first connects.
void Server::SendNewClientID(RakNet::SystemAddress & address)
{
	RakNet::BitStream bs;
	bs.Write((RakNet::MessageID)GameMessages::ID_SERVER_SET_CLIENT_ID);
	bs.Write(nextClientID);
	nextClientID++;
	pPeerInterface->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, address, false);

	// Send the existing gameobjects to the new client
	for (auto it = m_gameObjects.begin(); it != m_gameObjects.end(); it++)
	{
		GameObject obj = it->second;
		obj.Write(pPeerInterface, address, false);
	}
	// Send us to all other clients
	int ID = nextClientID - 1;
	GameObject obj;
	obj.GetData().position = glm::vec2(0);
	obj.GetData().colour = GameObject::GetColour(ID);
	obj.GetID() = ID;
	obj.Write(pPeerInterface, address, true);
}