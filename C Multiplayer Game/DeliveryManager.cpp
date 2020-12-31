#include "Networks.h"
#include "DeliveryManager.h"

//Server
Delivery* DeliveryManager::writeSequenceNumber(OutputMemoryStream& packet)
{
	Delivery* ret = new Delivery();

	packet << nextSequenceNumber;

	ret->sequenceNumber = nextSequenceNumber;
	ret->dispatchTime = Time.time;

	nextSequenceNumber++;

	currDeliveries.push_back(ret);

	return ret;
}

//Client
bool DeliveryManager::processSequenceNumber(const InputMemoryStream& packet)
{
	bool ret = false;

	uint32 sequence;
	packet >> sequence;

	if (sequence >= nextSequenceExpected)
	{
		LOG("Received packet: %d", nextSequenceExpected);

		pendingACK.push_back(sequence);
		ret = true;
		nextSequenceExpected = sequence + 1;
	}
	else
	{
		LOG("Waiting packet: %d", nextSequenceExpected);
	}

	return ret;
}

//Client
bool DeliveryManager::hasSequenceNumbersPendingAck() const
{
	bool ret = false;

	if (pendingACK.size() > 0)
	{
		ret = true;
	}

	return ret;
}

//Client
void DeliveryManager::writeSequenceNumbersPendingAck(OutputMemoryStream& packet)
{
	for (std::list<uint32>::iterator ACK_it = pendingACK.begin(); ACK_it != pendingACK.end(); ACK_it++)
	{
		uint32 ack = (*ACK_it);
		packet << ack;
	}

	pendingACK.clear();
}

//Server
void DeliveryManager::processAckdSequenceNumbers(const InputMemoryStream& packet)
{
	std::vector<Delivery*> to_erase;

	while (packet.RemainingByteCount() > 0)
	{
		uint32 sequenceNumber = 0;
		packet >> sequenceNumber;

		for (std::list<Delivery*>::iterator dv_it = currDeliveries.begin(); dv_it != currDeliveries.end(); ++dv_it)
		{
			if ((*dv_it)->sequenceNumber == sequenceNumber)
			{
				(*dv_it)->delegate->onDeliverySuccess(this);
				to_erase.push_back((*dv_it));
				DLOG("Last sequence ACK %d", sequenceNumber);
			}
		}
	}

	for (int i = 0; i < to_erase.size(); ++i)
	{
		DLOG("Removed packet %d", to_erase[i]->sequenceNumber);

		currDeliveries.remove(to_erase[i]);
	}

	to_erase.clear();
}

//Server
void DeliveryManager::processTimedOutPackets()
{
	std::vector<Delivery*> to_erase;

	for (std::list<Delivery*>::iterator dv_it = currDeliveries.begin(); dv_it != currDeliveries.end(); ++dv_it)
	{
		if ((*dv_it)->dispatchTime + DELIVERY_TIME_OUT < Time.time)
		{
			(*dv_it)->delegate->onDeliveryFailure(this);
			to_erase.push_back((*dv_it));
		}
	}

	for (int i = 0; i < to_erase.size(); ++i)
	{
		currDeliveries.remove(to_erase[i]);
	}

	to_erase.clear();
}

void DeliveryManager::forceSequenceNumber(uint32 num)
{
	nextSequenceNumber = num;
	DLOG("Forced numSequence %d", nextSequenceNumber);
}

//Server & Client
void DeliveryManager::clear()
{
	for (std::list<Delivery*>::iterator dv_it = currDeliveries.begin(); dv_it != currDeliveries.end(); ++dv_it)
	{
		(*dv_it) = nullptr;
	}

	nextSequenceExpected = 0;
	nextSequenceNumber = 0;

	currDeliveries.clear();
	pendingACK.clear();
}


//------------------------------------------------------ Delivery Delegates ----------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------

void DeliveryDelegateReplication::onDeliverySuccess(DeliveryManager* deliveryManager)
{

}

void DeliveryDelegateReplication::onDeliveryFailure(DeliveryManager* deliveryManager)
{
	if (used)
	{
		recreateCommands();
	}
}

void DeliveryDelegateReplication::recreateCommands()
{
	for (std::map<uint32, ReplicationAction>::iterator it_rc = deliveryReplicationCommands.begin(); it_rc != deliveryReplicationCommands.end(); ++it_rc)
	{
		switch (it_rc->second)
		{
		case ReplicationAction::Create:
			replicationManager->create(it_rc->first);
			break;
		case ReplicationAction::Update:
			replicationManager->update(it_rc->first);
			break;
		case ReplicationAction::Destroy:
			replicationManager->destroy(it_rc->first);
			break;
		default:
			break;
		}
	}

	deliveryReplicationCommands.clear();
}
