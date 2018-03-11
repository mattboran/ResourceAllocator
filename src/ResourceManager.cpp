#include "data_types.h"

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

ResourceManager::~ResourceManager()
{
	delete total_resources;
	delete resources_available;
	delete resources_claimed;
}

void ResourceManager::incrementCycle()
{
	cycle++;
}

int ResourceManager::getCycle()
{
	return cycle;
}
