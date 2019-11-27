//*
// Description: This Client class handles the clients functions such as the clients gameobject, updates, connection and messages 
// which is then broadcasted to the server.
// Author: Elisha Anagnostakis
// Date: 09/05/19
//*

//#include <mat4x4.hpp>
#include <RakPeerInterface.h>
#include <unordered_map>
#include "GameObject.h"

class Client {
public:

	Client();
	virtual ~Client();

	virtual bool Startup();
	virtual void shutdown() = 0;

	virtual void update(float deltaTime) = 0;
	virtual void draw() = 0;

	// @brief takes care of all network messages and returns void
	void HandleNetworkMessages();

	void OnSetClientIDPacket(RakNet::Packet* packet);

	// @brief serializes the gameobject and then sends the GameObject to the server
	void SendClientGameObject();

	// @brief reads the Client ID and makes sure that the client ID
	// does not match our ID, we can ignore the rest of the packet if it is.
	// Otherwise, we can read in the GameObject data from the packet, and do something with it
	void OnRecievedClientPacketData(RakNet::Packet* packetData);

protected:

	glm::mat4	                             m_viewMatrix;
	glm::mat4	                             m_projectionMatrix;
	RakNet::RakPeerInterface*                m_pPeerInterface;            // @brief peer to peer interface.
	const char*                              IP = "127.0.0.1";            // @brief IP address.
	GameObject                               m_myGameObject;              // @brief holds new client ID.
    std::unordered_map<int, GameObject*>     m_otherClientGameObjects;    // @brief holds the GameObject data for all the other clients.
};