#pragma once
//*
// Description: Allows us to send messages within the server to all clients
// Author: Elisha Anagnostakis
// Date: 09/05/19
//*

#include <MessageIdentifiers.h>
#include <RakPeerInterface.h>
#include <BitStream.h>
#include <thread>
#include <chrono>

// An enum that creates a unique ID we can use to label our packets coming from the server
// ID_USER_PACKET_ENUM value is the last ID that RakNet uses internally
enum GameMessages {
	ID_SERVER_TEXT_MESSAGE = ID_USER_PACKET_ENUM + 1,
	ID_SERVER_SET_CLIENT_ID,
	ID_CLIENT_CLIENT_DATA,
};

void SendClientPing(RakNet::RakPeerInterface* pPeerInterface);