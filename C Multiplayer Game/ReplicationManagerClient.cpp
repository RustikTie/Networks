#include "Networks.h"
#include "ReplicationManagerClient.h"

// TODO(you): World state replication lab session


void ReplicationManagerClient::read(const InputMemoryStream & packet)
{
	while (packet.RemainingByteCount() > sizeof(uint32))
	{
		uint32 networkId;
		packet >> networkId;

		ReplicationAction action;
		packet >> action;

		if (action == ReplicationAction::Create)
		{
			GameObject* object = nullptr;

			object = App->modLinkingContext->getNetworkGameObject(networkId);

			if (object)
			{

				App->modLinkingContext->unregisterNetworkGameObject(object);
				Destroy(object);
			}

			object = Instantiate();

			App->modLinkingContext->registerNetworkGameObjectWithNetworkId(object, networkId);

			std::string texture;

			packet >> object->position.x;
			packet >> object->position.y;
			packet >> object->angle;
			packet >> object->size.x;
			packet >> object->size.y;
			packet >> texture;

			object->sprite = App->modRender->addSprite(object);
			object->sprite->order = 5;
			if (texture!="null")
				object->sprite->texture = App->modTextures->loadTexture(texture.c_str());

			object->sprite->order = 100;

			packet >> object->explosion;

			if (object->explosion)
			{
				object->animation = App->modRender->addAnimation(object);
				object->animation->clip = App->modResources->explosionClip;
			}
			packet >> object->lifebar;
			 
			if (object->lifebar)
			{
				packet >> object->size.x;
				object->size.y = 5.0f;
				object->sprite->order = 5;
				object->sprite->pivot = vec2{ 0.0f, 0.5f };
				packet >> object->sprite->color.x;
				packet >> object->sprite->color.y;
				packet >> object->sprite->color.z;
				packet >> object->sprite->color.a;

			}

		/*	if (!object->explosion && !object->lifebar && texture!= "laser.png") {
				if(object->behaviour == nullptr)
					object->behaviour = App->modBehaviour->addSpaceship(object);
			}*/

			if (texture == "spacecraft1.png") {
				//already added?
				//if (object->behaviour == nullptr)
				//	object->behaviour = App->modBehaviour->addSpaceship(object);
			}
			

		}
		else if (action == ReplicationAction::Update)
		{
			GameObject* object = nullptr;
			object = App->modLinkingContext->getNetworkGameObject(networkId);

			float pos_x = 0.0f;
			float pos_y = 0.0f;
			float angle = 0.0f;
			float sizex = 0.0f;
			float sizey = 5.0f;

			packet >> pos_x;
			packet >> pos_y;
			packet >> angle;

			packet >> object->explosion;

			packet >> object->lifebar;

			if (object)
			{

				object->position.x = pos_x;
				object->position.y = pos_y;
				object->angle = angle;
				if (object->lifebar)
				{
					packet >> sizex;
					object->size.x = sizex;
					object->size.y = sizey;
					packet >> object->sprite->color.x;
					packet >> object->sprite->color.y;
					packet >> object->sprite->color.z;
					packet >> object->sprite->color.a;
				
				}
			}
		}
		else
		{
			GameObject* object = nullptr;
			object = App->modLinkingContext->getNetworkGameObject(networkId);

			if (object)
			{
				App->modLinkingContext->unregisterNetworkGameObject(object);

				Destroy(object);
			}
		}
	}
}
