/*
 * OptimisticResourceManager.cpp
 *
 *  Created on: Mar 10, 2018
 *      Author: matt
 */
#include <assert.h>
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
	incrementCycle();
}

void OptimisticResourceManager::dispatchRequest(const Action &action, Task& task)
{
	assert (task.getId() == action.getTaskId());
	int resource_id = action.getResourceId();

//	if (resource_id < )

	incrementCycle();
}

void OptimisticResourceManager::dispatchRelease(const Action &action, Task& task)
{
	assert (task.getId() == action.getTaskId());

	incrementCycle();
}

void OptimisticResourceManager::dispatchTerminate(const Action &action, Task& task)
{
	assert (task.getId() == action.getTaskId());

	task.setTimeTerminated(getCycle());
}
