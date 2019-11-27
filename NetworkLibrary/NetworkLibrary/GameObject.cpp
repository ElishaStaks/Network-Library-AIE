#include "GameObject.h"
#include <iostream>

// Gameobjects data.
GameObject::GameObject()
{
	data.position = glm::vec2(0);
	data.colour = glm::vec4(1, 1, 1, 1);
	id = 0;
}

// Different cosmetic colours for the gameobjects.
glm::vec4 colours[] = {
	glm::vec4(0.5, 0.5, 0.5, 1),
	glm::vec4(1, 0, 0, 1),
	glm::vec4(0, 1, 0, 1),
	glm::vec4(0, 0, 1, 1),
	glm::vec4(1, 1, 0, 1),
	glm::vec4(1, 0, 1, 1),
	glm::vec4(0, 1, 1, 1),
	glm::vec4(0, 0, 0, 1),
};

glm::vec4 GameObject::GetColour(int id)
{
	return colours[id & 7];
}

// Serializes the gameobject and then sends the GameObject to the server
void GameObject::Write(RakNet::RakPeerInterface* pPeerInterface, const RakNet::SystemAddress& address, bool broadcast)
{
	RakNet::BitStream bs;
	bs.Write((RakNet::MessageID)GameMessages::ID_CLIENT_CLIENT_DATA);
	bs.Write(id);
	bs.Write((char*)&data, sizeof(GameObjectData));
	pPeerInterface->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, address, broadcast);
}

void GameObject::Read(RakNet::Packet* packet)
{
	RakNet::BitStream bsIn(packet->data, packet->length, false);
	bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
	bsIn.Read(id);
	bsIn.Read((char*)&data, sizeof(GameObjectData));
}

GameObject* GameObject::CreateGameObject() {
	GameObject* m_player = new GameObject();
	return m_player;
}

glm::vec2 GameObject::SetLocalPos(glm::vec2 pos)
{
	return localPosition = pos;
}

glm::vec2 GameObject::GetLocalPos()
{
	return localPosition;
}


int GameObject::SetID(int id)
{
	return this->id = id;
}

int& GameObject::GetID()
{
	return id;
}

GameObjectData& GameObject::GetData()
{
	return data;
}

std::ostream& operator<<(std::ostream& stream, const GameObject& go)
{
	stream << go.id;
	return stream;
}