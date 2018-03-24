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
	int claim = task.getResourceClaim(resource_id);
	int available = getResourcesAvailable(resource_id);

	if (claim > available)
	{
		task.abort();
		task.setTimeTerminated(getCycle());
		std::cout << "Banker aborts task # " << task.getId() + 1 << " before run begins: \n";
		std::cout << "\tclaim for resource " << resource_id + 1 << " (" << claim << ") exceeds number of units present: "
				  << available << ".\n";
		return;
	}
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
		int resource = 0;
		for (int i = 0; i < getNumResources(); i++)
		{
			resource = task.getResourceHeld(i);
			if (resource > 0)
			{
				task.releaseResources(i, resource);
				incrementResourcesAvailable(i, resource);
			}
		}
		task.setTimeTerminated(getCycle());
		std::cout << "Task # " << task.getId() + 1 << " request exceeded claim. Aborted!\n";
		return;
	}

	if (task.getDelay() < action.getDelay())
	{
		task.incrementDelay();
		std::cout << "Task # " << task.getId() + 1 << " is delayed (" << task.getDelay() <<
				" of " << action.getDelay() << " cycles).\n";
	}else
	{
		if (getResourcesAvailable(requested_resource_id) < task.getResourceClaim(requested_resource_id) - task.getResourceHeld(requested_resource_id))
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

// For a release, if there's a delay, increment the delay counter until delay == action.delay
// Otherwise, set the amount to be decreased at the end of the cycle by calling releaseResources
// This gets committed at the end of the cycle by commit_resources
void BankerResourceManager::dispatchRelease(const Action &action, Task& task)
{
	assert (task.getId() == action.getTaskId());
	int released_resource_id = action.getResourceId();
	int amount_released = action.getAmount();

	if (task.getDelay() < action.getDelay())
	{
		task.incrementDelay();
		std::cout << "Task # " << task.getId() + 1 << " is computing (" << task.getDelay() <<
						" of " << action.getDelay() << " cycles).\n";
	}
	else
	{
		task.unblock();
		task.setDelay(0);
		task.releaseResources(released_resource_id, amount_released);
		incrementResourcesAvailable(released_resource_id, amount_released);

		std::cout << " Task # " << task.getId() + 1 << " is releasing " << amount_released <<
				" of resource " << released_resource_id + 1 << ". It now holds " <<
				task.getResourceHeld(released_resource_id) << " of that resource.\n";
	}

}

// First, make sure the action isn't already aborted
// Then. process delay in the same way the other actions handle delay
// Then, if delay counter == action.delay, terminate the process by setting the terminated time to now
void BankerResourceManager::dispatchTerminate(const Action &action, Task& task)
{
	assert (task.getId() == action.getTaskId());
	assert (!task.isDoneOrAborted());
	if (task.getDelay() < action.getDelay())
	{
		task.incrementDelay();
		std::cout << "Task # " << task.getId() + 1 << " is computing (" << task.getDelay() <<
						" of " << action.getDelay() << " cycles).\n";
	}
	else
	{
		task.setDelay(0);
		task.setTimeTerminated(getCycle());
		std::cout << " Task # " << task.getId() + 1 << " is terminated. \n";
	}
}

