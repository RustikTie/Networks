#include "Networks.h"
#include "ReplicationManagerServer.h"



void ReplicationManagerServer::create(uint32 networkId)
{
	replicationCommands[networkId] = ReplicationAction::Create;
}


void ReplicationManagerServer::update(uint32 networkId)
{
	std::map<uint32, ReplicationAction>::iterator iterator = replicationCommands.find(networkId);
	if (iterator == replicationCommands.end())
	{
		replicationCommands[networkId] = ReplicationAction::Update;
	}
}


void ReplicationManagerServer::destroy(uint32 networkId)
{
	replicationCommands[networkId] = ReplicationAction::Destroy;
}


void ReplicationManagerServer::write(OutputMemoryStream& packet, Delivery* delivery)
{
	delivery->delegate->replicationManager = this;

	for (std::map<uint32, ReplicationAction>::iterator it_rc = replicationCommands.begin(); it_rc != replicationCommands.end(); ++it_rc)
	{
		if (it_rc->second == ReplicationAction::Create)
		{
			GameObject* object = nullptr;
			object = App->modLinkingContext->getNetworkGameObject(it_rc->first);

			if (object)
			{
				packet << it_rc->first;
				packet << it_rc->second;

				packet << object->position.x;
				packet << object->position.y;
				packet << object->angle;
				packet << object->size.x;
				packet << object->size.y;

				std::string texture = "null";

				if (object->sprite->texture != nullptr)
				{
					texture = object->sprite->texture->filename ;

				}

				packet << texture;
				packet << object->explosion;
				packet << object->lifebar;

				if (object->lifebar) {
					//lifebar->size = vec2{ lifeRatio * 80.0f, 5.0f };
					float size = object->size.x;
					packet << size;
					float colorx = object->sprite->color.x;
					float colory = object->sprite->color.y;
					float colorz = object->sprite->color.z;
					float colora = object->sprite->color.a;
					packet << colorx;
					packet << colory;
					packet << colorz;
					packet << colora;
				}
			}
		}
		else if (it_rc->second == ReplicationAction::Update)
		{
			GameObject* object = nullptr;
			object = App->modLinkingContext->getNetworkGameObject(it_rc->first);

			if (object)
			{
				packet << it_rc->first;
				packet << it_rc->second;

				packet << object->position.x;
				packet << object->position.y;
				packet << object->angle;
				
				packet << object->explosion;
				packet << object->lifebar;

				if (object->lifebar) {
					//lifebar->size = vec2{ lifeRatio * 80.0f, 5.0f };
					float size = object->size.x;
					packet << size;
					float colorx = object->sprite->color.x;
					float colory = object->sprite->color.y;
					float colorz = object->sprite->color.z;
					float colora = object->sprite->color.a;
					packet << colorx;
					packet << colory;
					packet << colorz;
					packet << colora;
				}
			}

		}
		else if (it_rc->second == ReplicationAction::Destroy)
		{
			packet << it_rc->first;
			packet << it_rc->second;
		}

		if (delivery != nullptr)
		{
			delivery->delegate->deliveryReplicationCommands[it_rc->first] = it_rc->second;
			delivery->delegate->used = true;
		}
	}

	replicationCommands.clear();
}
