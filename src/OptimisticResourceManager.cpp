/*
 * OptimisticResourceManager.cpp
 *
 *  Created on: Mar 10, 2018
 *      Author: matt
 */
#include "data_types.h"

OptimisticResourceManager::OptimisticResourceManager(int num_resources, int tasks, int* resources_initial)
{
	ResourceManager(num_resources, tasks, resources_initial);
}

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

}

void OptimisticResourceManager::dispatchRequest(const Action &action, Task& task)
{

}

void OptimisticResourceManager::dispatchRelease(const Action &action, Task& task)
{

}

void OptimisticResourceManager::dispatchTerminate(const Action &action, Task& task)
{

}
