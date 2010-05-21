/*
---------------------------------------------------------------------------------------
This source file is part of swgANH (Star Wars Galaxies - A New Hope - Server Emulator)
For more information, see http://www.swganh.org


Copyright (c) 2006 - 2010 The swgANH Team

---------------------------------------------------------------------------------------
*/

//common includes

#include "Item.h"
#include "ObjectController.h"
#include "ObjectControllerOpcodes.h"
#include "ObjectControllerCommandMap.h"
#include "PlayerObject.h"
#include "UIManager.h"
#include "VehicleController.h"
#include "VehicleControllerFactory.h"
#include "WorldConfig.h"
#include "WorldManager.h"
#include "MessageLib/MessageLib.h"
#include "LogManager/LogManager.h"
#include "DatabaseManager/Database.h"
#include "DatabaseManager/DatabaseResult.h"
#include "DatabaseManager/DataBinding.h"
#include "Common/Message.h"
#include "Common/MessageFactory.h"

void ObjectController::_handleMount(uint64 targetId,Message* message,ObjectControllerCmdProperties* cmdProperties)
{
	// The very idea with using ID's instead of object refs are that you can TEST them without using the object itself.
	// And some parameter validation...
	if (targetId == 0)
	{
		gLogger->log(LogManager::DEBUG,"ObjectController::_handleMount : Cannot find vehicle ID :(");
		return;
	}

	PlayerObject* player	= dynamic_cast<PlayerObject*>(mObject);

	if (player && player->getMount() && (player->getParentId() == 0))
	{
		// Do we have a valid target?
		if (!player->checkIfMounted())
		{
			// verify its player's mount
			MountObject* pet	= dynamic_cast<MountObject*>(gWorldManager->getObjectById(targetId));
			if (pet && (pet->owner() == player->getId()))
			{
				// get the mount Vehicle object by the id (Creature object id - 1 )

				if(VehicleController* vehicle = dynamic_cast<VehicleController*>(gWorldManager->getObjectById(pet->controller())))
				{
					//The /mount command can work up to 32m on live
          if(glm::distance(vehicle->body()->mPosition, player->mPosition) <= 32)	{
						vehicle->MountPlayer();
					}	else {
						gMessageLib->sendSystemMessage(player,L"Your target is too far away to mount.");
					}
				}
				else
				{
					gLogger->log(LogManager::DEBUG,"ObjectController::_handleMount : Cannot find vehicle");
				}
			}
		} else {
			gMessageLib->sendSystemMessage(player,L"You cannot mount this because you are already mounted.");
		}
	}
}

//===============================================================================================

void ObjectController::_handleDismount(uint64 targetId,Message* message,ObjectControllerCmdProperties* cmdProperties)
{
	// The very idea with using ID's instead of object refs are that you can TEST them without using the object itself.
	// And some parameter validation...

	PlayerObject*	player	= dynamic_cast<PlayerObject*>(mObject);

	if (player && player->getMount() && (player->getParentId() == 0))
	{
		if (player->checkIfMounted())
		{
			// verify its player's mount
			MountObject* pet = NULL;
			if (targetId == 0)
			{
				// No object targeted, assume the one we are riding.	- what else should we dismount ???
				pet	= player->getMount();
			}
			else
			{
				pet = dynamic_cast<MountObject*>(gWorldManager->getObjectById(targetId));
			}

			if (pet && (pet->owner() == player->getId()))
			{
				// get the pets controller for a swoop its the vehicle
				if(VehicleController* vehicle = dynamic_cast<VehicleController*>(gWorldManager->getObjectById(pet->controller())))
				{
					vehicle->DismountPlayer();
				}
			}
		}
		else
		{
			gMessageLib->sendSystemMessage(player,L"You are not mounted to perform this action.");
		}
	}
}

//===============================================================================================