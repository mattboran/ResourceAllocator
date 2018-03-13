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
	cycle_resources_changed = new int[n_resources];

	for (int i = 0; i < n_resources; i++)
	{
		total_resources[i] = resources_initial[i];
		resources_available[i] = resources_initial[i];
		resources_claimed[i] = 0;
		cycle_resources_changed[i] = 0;
	}
}

ResourceManager::~ResourceManager()
{
	delete total_resources;
	delete resources_available;
	delete resources_claimed;
	delete cycle_resources_changed;
}

void ResourceManager::reset()
{
	for (int i = 0; i < num_resources; i++)
	{
		resources_available[i] = total_resources[i];
		resources_claimed[i] = 0;
		cycle_resources_changed[i] = 0;
	}
}

// Used to track down segfaults/vector access out of bounds
bool ResourceManager::sanityCheck(int i)
{
	return (i >= 0) && (i < num_resources);
}

void ResourceManager::incrementResourcesAvailable(int i, int amount)
{
	assert (sanityCheck(i));
	cycle_resources_changed[i] += amount;
}

void ResourceManager::decrementResourcesAvailable(int i, int amount)
{
	assert (sanityCheck(i));
	resources_available[i] -= amount;
}

// At the end of each cycle, commit any increases/decreases made to resource availability
void ResourceManager::commitReleasedResources()
{
	for (int i = 0; i < num_resources; i++)
	{
		resources_available[i] += cycle_resources_changed[i];
		cycle_resources_changed[i] = 0;
	}
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

int ResourceManager::getResourcesChanged(int i)
{
	assert(sanityCheck(i));
	return cycle_resources_changed[i];
}

int ResourceManager::getCycle()
{
	return cycle;
}

int ResourceManager::getNumResources()
{
	return num_resources;
}
