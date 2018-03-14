/*
 * OptimisticResourceManager.cpp
 *
 *  Created on: Mar 10, 2018
 *      Author: matt
 */
#include <algorithm>
#include <assert.h>
#include <iostream>
#include "data_types.h"

using namespace std;

static bool compareTasksForSort(const Task &a, const Task &b);

OptimisticResourceManager::OptimisticResourceManager(int num_resources, int tasks, int* resources_initial) :
	ResourceManager(num_resources, tasks, resources_initial) {}

// For each cycle, for each task, dispatch the appropriate action
void OptimisticResourceManager::dispatchAction(Task& task)
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

void OptimisticResourceManager::dispatchInitiate(const Action &action, Task& task)
{
	assert (task.getId() == action.getTaskId());

	int resource_id = action.getResourceId();

	task.setTimeCreated(getCycle());
	task.setResourceClaimed(action.getResourceId(), action.getAmount());
	std::cout << "At cycle " << getCycle() << " - " << getCycle() + 1 <<
					" Task # " << task.getId() + 1 << " was initialized with claim "
					<< action.getAmount() << " of resource " << resource_id + 1 << "\n";
}

void OptimisticResourceManager::dispatchRequest(const Action &action, Task& task)
{
	assert (task.getId() == action.getTaskId());
	int requested_resource_id = action.getResourceId();
	int amount_requested = action.getAmount();

	if (task.getDelay() < action.getDelay())
	{
		task.incrementDelay();
		std::cout << "Task # " << task.getId() + 1 << " is computing (" << task.getDelay() <<
				" of " << action.getDelay() << " cycles).\n";
	}
	else
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

void OptimisticResourceManager::dispatchRelease(const Action &action, Task& task)
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

void OptimisticResourceManager::dispatchTerminate(const Action &action, Task& task)
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

// Checks for deadlock, and aborts the lowest process in the case that deadlock is found
bool OptimisticResourceManager::handleDeadlock(vector<Task> &tasklist)
{
	bool ret_val = false;
	// Sort the tasklist by ID so we can kill off the 1st process that's deadlocking, then 2nd, etc
	// Then check if there's deadlock
	int resource = 0;
	std::sort(tasklist.begin(), tasklist.end(), compareTasksForSort);
	if (detectDeadlock(tasklist))
	{
		ret_val = true;
		for (auto it = tasklist.begin(); it != tasklist.end(); it++)
		{
			if (!it->isDoneOrAborted())
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
				it->abort();
				it->unblock();
				break;
			}
		}
	}
	return ret_val;
}

bool OptimisticResourceManager::detectDeadlock(vector<Task> &tasklist)
{
	for (auto it = tasklist.begin(); it != tasklist.end(); it++)
	{
		if (!it->isBlocked() && !it->isDoneOrAborted())
		{
			return false;
		}
	}
	return true;
}

bool OptimisticResourceManager::canSatisfyAnyRequest( taskvec_t &tasklist)
{
	bool ret_val = false;

	int* new_resources = new int[getNumResources()];
	for (int i = 0; i < getNumResources(); i++)
	{
		new_resources[i] = getResourcesAvailable(i) + getResourcesChanged(i);
	}

	int current_request = 0;

	for (unsigned int i = 0; i < tasklist.size(); i++)
	{
		if (tasklist[i].isDoneOrAborted())
		{
			continue;
		}
		const Action current_action = *tasklist[i].getActionPointer();
		current_request = current_action.getAmount();
		if (new_resources[current_action.getResourceId()] >= current_request)
		{
			ret_val = true;
			break;
		}
	}

	delete new_resources;
	return ret_val;
}

static bool compareTasksForSort(const Task &a, const Task &b)
{
	return a.getId() < b.getId();
}
