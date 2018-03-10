#include "data_types.h"

// Constructor and Destructor for ResourceManager

ResourceManager::ResourceManager(int num_resources, int tasks, int* resources_initial)
{
	num_tasks = tasks;
	total_resources = new int[num_resources];
	resources_available = new int[num_resources];
	resources_claimed = new int[num_resources];

	for (int i = 0; i < num_resources; i++)
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

