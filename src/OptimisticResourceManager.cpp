/*
 * OptimisticResourceManager.cpp
 *
 *  Created on: Mar 10, 2018
 *      Author: matt
 */
#include <assert.h>
#include <iostream>
#include "data_types.h"

using namespace std;

OptimisticResourceManager::OptimisticResourceManager(int num_resources, int tasks, int* resources_initial) :
	ResourceManager(num_resources, tasks, resources_initial) {}

// For each cycle, for each task, dispatch the appropriate action
void OptimisticResourceManager::dispatchAction(const Action &action, Task& task)
{
	switch(action.getType())
	{
	case(INITIATE):
			dispatchInitiate(action, task);
			break;
	case(REQUEST):
			dispatchRequest(action, task);
			break;
	case(RELEASE):
			dispatchRelease(action, task);
			break;
	case(TERMINATE):
			dispatchTerminate(action, task);
			break;
	}
}

void OptimisticResourceManager::dispatchInitiate(const Action &action, Task& task)
{
	assert (task.getId() == action.getTaskId());

	int resource_id = action.getResourceId();

	task.setTimeCreated(getCycle());
	task.setResourceClaimed(action.getResourceId(), action.getAmount());
	std::cout << "At cycle " << getCycle() << " - " << getCycle() + 1 <<
					" Task # " << task.getId() + 1 << " was initialized with claim "
					<< action.getAmount() << " of resource " << resource_id + 1 <<
					". It now claims " << task.getResourceClaim(resource_id) << " of that resource.\n";
}

void OptimisticResourceManager::dispatchRequest(const Action &action, Task& task)
{
	assert (task.getId() == action.getTaskId());
	int requested_resource_id = action.getResourceId();
	int amount_requested = action.getAmount();

	if (task.getDelay() < action.getDelay())
	{
		task.incrementDelay();
	}
	else
	{
		if (getResourcesAvailable(requested_resource_id) < amount_requested)
		{
			task.block();
			std::cout << "At cycle " << getCycle() << " - " << getCycle() + 1 <<
					" Task # " << task.getId() << " could not be granted its resource!\n";
		}
		else
		{
			task.grantResources(requested_resource_id, amount_requested);
			decrementResourcesAvailable(requested_resource_id, amount_requested);
			std::cout << "At cycle " << getCycle() << " - " << getCycle() + 1 <<
					" Task # " << task.getId() + 1 << " was granted " << amount_requested <<
					" of resource " << requested_resource_id + 1 << ". It now holds " <<
					task.getResourceHeld(requested_resource_id) << " of that resource.\n";
		}
	}
}

void OptimisticResourceManager::dispatchRelease(const Action &action, Task& task)
{
	assert (task.getId() == action.getTaskId());

	int released_resource_id = action.getResourceId();
	int amount_released = action.getAmount();

	if (task.getDelay() < action.getDelay())
	{
		task.incrementDelay();
	}
	else
	{
		task.setDelay(0);
		task.releaseResources(released_resource_id, amount_released);
		incrementResourcesAvailable(released_resource_id, amount_released);

		std::cout << "At cycle " << getCycle() << " - " << getCycle() + 1 <<
				" Task # " << task.getId() + 1 << " is releasing " << amount_released <<
				" of resource " << released_resource_id + 1 << ". It now holds " <<
				task.getResourceHeld(released_resource_id) << " of that resource.\n";
	}

}

void OptimisticResourceManager::dispatchTerminate(const Action &action, Task& task)
{
	assert (task.getId() == action.getTaskId());
	assert (task.getTimeTerminated() < 0);
	if (task.getDelay() < action.getDelay())
	{
		task.incrementDelay();
	}
	else
	{
		task.setDelay(0);
		task.setTimeTerminated(getCycle());
		std::cout << "At cycle " << getCycle() <<
					" Task # " << task.getId() + 1 << " is terminated. \n";
	}
}

// Checks for deadlock, and aborts the lowest process in the case that deadlock is found
void OptimisticResourceManager::handleDeadlock(vector<Task> &tasklist)
{
	// First check if there's deadlock
	int resource = 0;
	if (detectDeadlock(tasklist))
	{
		for (auto it = tasklist.begin(); it != tasklist.end(); it++)
		{
			if (!it->isAborted())
			{
				it->setTimeTerminated(getCycle());
				std::cout << "Task # " << it->getId() + 1 << " was aborted due to deadlock.\n";
				std::cout << "Releasing ";
				for (int i = 0; i < getNumResources(); i++)
				{
					resource = it->getResourceHeld(i);
					if (resource > 0)
					{
						std::cout << resource << " of resource " << i + 1 << "; ";
						it->releaseResources(i, resource);
						incrementResourcesAvailable(i, resource);
					}
				}
				std::cout << ".\n";
				break;
			}
		}
	}
}

bool OptimisticResourceManager::detectDeadlock(vector<Task> &tasklist)
{
	for (auto it = tasklist.begin(); it != tasklist.end(); it++)
	{
		if (!it->isBlocked())
		{
			return false;
		}
	}
	return true;
}
