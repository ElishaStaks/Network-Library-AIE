#include "Client.h"
#include <glm.hpp>
#include <ext.hpp>
#include <iostream>
#include <string>
#include <MessageIdentifiers.h>
#include "GameMessages.h"

using glm::vec2;
using glm::vec4;
using glm::mat4;

using std::cout;
using std::endl;

Client::Client() {

}

Client::~Client() {
} 

bool Client::Startup() {

	// handles the initialisations to the clients connections
	m_pPeerInterface = RakNet::RakPeerInterface::GetInstance();
	// create a socket descriptor to describe this connection
	// no data needed, as we will be connecting to a server
	RakNet::SocketDescriptor sd;

	// now call startup - max of 1 connections (to the server)
	m_pPeerInterface->Startup(32, &sd, 1);

	std::string IP;
	std::cout << "Enter IP Address: ";
	std::getline(std::cin, IP);

	unsigned short PORT;
	std::cout << "Enter Port Address: ";
	std::cin >> PORT;

	std::cout << "Connecting to server at: " << IP << ", " << PORT << std::endl;

	// now call connect to attempt to connect to the given server
	RakNet::ConnectionAttemptResult res = m_pPeerInterface->Connect(IP.c_str(), PORT, nullptr, 0);

	// check to see if we connected, and if not, throw an error
	if (res != RakNet::CONNECTION_ATTEMPT_STARTED) {
		std::cout << "Unable to start connection, Error number: " << res << std::endl;
	}

	return true;
}

// @brief takes care of all network messages and returns void
void Client::HandleNetworkMessages()
{
	// chunk of data that will be sent over the network
	RakNet::Packet* packet;

	// recieves packet of data
	for (packet = m_pPeerInterface->Receive(); packet;
		m_pPeerInterface->DeallocatePacket(packet),
		packet = m_pPeerInterface->Receive()) 
	{
		switch (packet->data[0])
		{
		case ID_REMOTE_DISCONNECTION_NOTIFICATION:
			cout << "Another client has disconnected\n";
			break;
		case ID_REMOTE_CONNECTION_LOST:
			cout << "Another client has lost the connection\n";
			break;
		case ID_REMOTE_NEW_INCOMING_CONNECTION:
			cout << "Another client has connected\n";
			break;
		case ID_CONNECTION_REQUEST_ACCEPTED:
			cout << "Our connection request has been accepted\n";
			break;
		case ID_NO_FREE_INCOMING_CONNECTIONS:
			cout << "The server is full\n";
			break;
		case ID_DISCONNECTION_NOTIFICATION:
			cout << "We have been disconnected\n";
			break;
		case ID_CONNECTION_LOST:
			cout << "Connection lost\n";
			break;
		case ID_SERVER_TEXT_MESSAGE:
		{
			RakNet::BitStream bsIn(packet->data, packet->length, false);
			bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
			RakNet::RakString str;
			bsIn.Read(str);
			std::cout << str.C_String() << std::endl;
			break;
		}
		// ID_SERVER_SET_CLIENT_ID packet arrives
		case ID_SERVER_SET_CLIENT_ID:
			OnSetClientIDPacket(packet);
			break;

		// client receives a packet with the ID_CLIENT_CLIENT_DATA message
		case ID_CLIENT_CLIENT_DATA:
			OnRecievedClientPacketData(packet);
			break;

		default:
			cout << "Recieved a message with an unkown ID" << packet->data[0];
			break;
		}
	}
}

void Client::OnSetClientIDPacket(RakNet::Packet * packet)
{
	RakNet::BitStream bsIn(packet->data, packet->length, false);
	bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
	int id = -1;
	bsIn.Read(id);
	m_myGameObject.SetID(id);
	m_myGameObject.GetData().colour = GameObject::GetColour(m_myGameObject.GetID());
	std::cout << "Set my client ID to: " << m_myGameObject.GetID() << std::endl;
}

// @brief calls the gameobject write function to send the gameobject to the server
void Client::SendClientGameObject()
{
	m_myGameObject.Write(m_pPeerInterface, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
}

// @brief reads the Client ID and makes sure that the client ID
// does not match our ID, we can ignore the rest of the packet if it is.
// Otherwise, we can read in the GameObject data from the packet, and do something with it
void Client::OnRecievedClientPacketData(RakNet::Packet * packetData)
{
	RakNet::BitStream bsIn(packetData->data, packetData->length, false);
	bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
	int clientID = -1;
	bsIn.Read(clientID);

	// If the clientID does not match our ID, we need to update
	// our client GameObject information.
	if (clientID != m_myGameObject.GetID()) {
		GameObject clientData;
		clientData.Read(packetData);

		if (m_otherClientGameObjects.count(clientID) == 0)
		{
			m_otherClientGameObjects[clientID] = GameObject::CreateGameObject();
			clientData.GetLocalPos() = clientData.GetData().position;
		}
		else
		{
			clientData.GetLocalPos() = m_otherClientGameObjects[clientID]->GetLocalPos();
		}
		*m_otherClientGameObjects[clientID] = clientData;
	}
}