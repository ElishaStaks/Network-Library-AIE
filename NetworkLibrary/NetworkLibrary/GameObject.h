//*
// Description: This Gameobject class gives the gameobject attributes which then is broadcasted and read by the server. 
// Author: Elisha Anagnostakis
// Date: 09/05/19
//*

#include <glm.hpp>
#include "GameMessages.h"

// Struct that holds the clients position in the world and their colour.
// Each client will have an instance of the GameObject class that is controlled by that client. When the
// client moves or changes the GameObject, the client will send information about the GameObject to
// the server to be sent to all the other connected clients.
struct GameObjectData {
	glm::vec2 position;
	glm::vec4 colour;
};

class GameObject
{
public:
	GameObject();

	void Write(RakNet::RakPeerInterface* pPeerInterface, const RakNet::SystemAddress& address, bool broadcast);
	void Read(RakNet::Packet* packet);
	static GameObject* CreateGameObject();

	glm::vec2 SetLocalPos(glm::vec2 pos);
	glm::vec2 GetLocalPos();
	static glm::vec4 GetColour(int id);

	int SetID(int id);
	int& GetID();

	GameObjectData& GetData();

	friend std::ostream& operator<<(std::ostream& stream, const GameObject& go);

private:
	GameObjectData data;
	glm::vec2 localPosition;
	int id = 0;
};