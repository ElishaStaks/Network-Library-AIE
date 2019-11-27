//*
// Description: Server class which handles all networking messages going to the clients 
// and clients addresses within the server.
// Author: Elisha Anagnostakis
// Date: 09/05/19
//*

#include <map>
#include "GameObject.h"
#include <iostream>

class Server
{
public:
	Server();
	virtual ~Server();

	virtual void StartUp();
	virtual void ShutDown() = 0;
	void HandleNetworkMessages();
	void SendNewClientID(RakNet::SystemAddress & address);

	std::map<int, GameObject> m_gameObjects;
	// peer to peer interface.  
	RakNet::RakPeerInterface* pPeerInterface = nullptr;
	// Client unique ID when it first connects to the server.
	int nextClientID = 1;
};