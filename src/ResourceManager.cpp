#include "data_types.h"
#include <assert.h>

// Constructor and Destructor for ResourceManager

ResourceManager::ResourceManager(int n_resources, int tasks, int* resources_initial)
{
	num_tasks = tasks;
	num_resources = n_resources;
	cycle = 0;
	total_resources = new int[n_resources];
	resources_available = new int[n_resources];
	resources_claimed = new int[n_resources];

	for (int i = 0; i < n_resources; i++)
	{
		total_resources[i] = resources_initial[i];
		resources_available[i] = resources_initial[i];
		resources_claimed[i] = 0;
	}
}

void ResourceManager::reset()
{
	for (int i = 0; i < num_resources; i++)
	{
		resources_available[i] = total_resources[i];
		resources_claimed[i] = 0;
	}
}

bool ResourceManager::sanityCheck(int i)
{
	return (i >= 0) && (i < num_resources);
}

void ResourceManager::incrementResourcesAvailable(int i, int amount)
{
	assert (sanityCheck(i));
	resources_available[i] += amount;
}

void ResourceManager::decrementResourcesAvailable(int i, int amount)
{
	assert (sanityCheck(i));
	resources_available[i] -= amount;
}

void ResourceManager::incrementCycle()
{
	cycle++;
}

int ResourceManager::getResourcesAvailable(int i)
{
	assert(sanityCheck(i));
	return resources_available[i];
}

int ResourceManager::getCycle()
{
	return cycle;
}
