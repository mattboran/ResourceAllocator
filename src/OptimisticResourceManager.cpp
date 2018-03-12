/*
 * OptimisticResourceManager.cpp
 *
 *  Created on: Mar 10, 2018
 *      Author: matt
 */
#include <assert.h>
#include <iostream>
#include "data_types.h"

OptimisticResourceManager::OptimisticResourceManager(int num_resources, int tasks, int* resources_initial)
{
	ResourceManager(num_resources, tasks, resources_initial);
}

// Inspired by React-Redux's Action Creators and Actions - handle the action
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

	task.setTimeCreated(getCycle());
	task.setResourceClaimed(action.getResourceId(), action.getAmount());
}

void OptimisticResourceManager::dispatchRequest(const Action &action, Task& task)
{
	assert (task.getId() == action.getTaskId());
	int requested_resource_id = action.getResourceId();
	int amount_requested = action.getAmount();

	if (resources_available[requested_resource_id] < amount_requested)
	{
		task.block();
		std::cout << "At cycle " << getCycle() << " - " << getCycle() + 1 <<
				" Task # " << task.getId() << " could not be granted its resource!\n";
	}
	else
	{
		int held_resources = task.getResourceHeld(requested_resource_id);
		task.grantResources(requested_resource_id, amount_requested);
		resources_available[requested_resource_id] -= amount_requested;
		std::cout << "At cycle " << getCycle() << " - " << getCycle() + 1 <<
				" Task # " << task.getId() + 1 << " was granted " << amount_requested <<
				" of resource " << requested_resource_id + 1 << ". It now holds " <<
				task.getResourceHeld(requested_resource_id) << " of that resource.\n";
	}
}

void OptimisticResourceManager::dispatchRelease(const Action &action, Task& task)
{
	assert (task.getId() == action.getTaskId());

	int released_resource_id = action.getResourceId();
	int amount_released = action.getAmount();

	task.releaseResources(released_resource_id, amount_released);
	resources_available[released_resource_id] += amount_released;

	std::cout << "At cycle " << getCycle() << " - " << getCycle() + 1 <<
			" Task # " << task.getId() + 1 << " is releasing " << amount_released <<
			" of resource " << released_resource_id + 1 << ". It now holds " <<
			task.getResourceHeld(released_resource_id) << " of that resource.\n";

}

void OptimisticResourceManager::dispatchTerminate(const Action &action, Task& task)
{
	assert (task.getId() == action.getTaskId());
	assert (task.getTimeTerminated() < 0);

	task.setTimeTerminated(getCycle());
}
