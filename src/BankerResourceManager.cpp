/*
 * BankerResourceManager.cpp
 *
 *  Created on: Mar 22, 2018
 *      Author: matt
 */
#include <algorithm>
#include <assert.h>
#include <iostream>
#include "data_types.h"

using namespace std;

BankerResourceManager::BankerResourceManager(int num_resources, int tasks, int* resources_initial):
	ResourceManager(num_resources, tasks, resources_initial){}

// For each cycle, for each task, dispatch the appropriate action
void BankerResourceManager::dispatchAction(Task &task)
{
	const Action action = *task.getActionPointer();
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

// Set the int at resource_claimed[id] to the value given by action
void BankerResourceManager::dispatchInitiate(const Action &action, Task& task)
{
	assert (task.getId() == action.getTaskId());

	int resource_id = action.getResourceId();

	task.setTimeCreated(getCycle());
	task.setResourceClaimed(action.getResourceId(), action.getAmount());
	std::cout << "At cycle " << getCycle() << " - " << getCycle() + 1 <<
					" Task # " << task.getId() + 1 << " was initialized with claim "
					<< action.getAmount() << " of resource " << resource_id + 1 << "\n";
}

// For a request, first check if the request exceeds the claim of the process
// If there's a delay, increment the delay counter until delay == action.delay
// Otherwise, if request cannot be granted, block process. If it's already blocked, increase wait time
// If request can be granted, grant the resources
void BankerResourceManager::dispatchRequest(const Action &action, Task& task)
{
	assert (task.getId() == action.getTaskId());

	int requested_resource_id = action.getResourceId();
	int amount_requested = action.getAmount();

	// First check for error: if requested amount + current held > claim, throw error and terminate
	if (task.getResourceClaim(requested_resource_id) < amount_requested + task.getResourceHeld(requested_resource_id))
	{
		task.abort();
		task.setTimeTerminated(getCycle());
		std::cout << "Task # " << task.getId() + 1 << " request exceeded claim. Aborted!\n";
		return;
	}

	if (task.getDelay() < action.getDelay())
	{
		task.incrementDelay();
		std::cout << "Task # " << task.getId() + 1 << " is computing (" << task.getDelay() <<
				" of " << action.getDelay() << " cycles).\n";
	}else
	{
		if (getResourcesAvailable(requested_resource_id) < amount_requested)
		{
			if (!task.isBlocked())
			{
				task.block();
				task.setBlockedSince(getCycle());
			}
			std::cout << " Task # " << task.getId() + 1<< " could not be granted its resource!\n";
		}
		else
		{
			task.unblock();
			task.setDelay(0);
			task.grantResources(requested_resource_id, amount_requested);
			decrementResourcesAvailable(requested_resource_id, amount_requested);
			std::cout << " Task # " << task.getId() + 1 << " was granted " << amount_requested <<
					" of resource " << requested_resource_id + 1 << ". It now holds " <<
					task.getResourceHeld(requested_resource_id) << " of that resource.\n";
		}
	}

}
//
