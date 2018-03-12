#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "data_types.h"

using namespace std;

typedef vector<Task> taskvec_t;
typedef vector<Action> actionvec_t;
typedef vector<actionvec_t> ActionContainer_t;

static action_t stringToActionType(const string &str);
static bool areAllTasksFinished(const taskvec_t &tasklist);

int main(int argc, char** argv)
{
	// Set up input stream and open the file
	string filename = "";
	ifstream input_file;

	if (argc < 2)
	{
		filename = "./data/input-01.txt";
	}
	else
	{
		filename = argv[1];
	}

	input_file.open(filename.c_str());
	if (!input_file)
	{
		cerr << "Unable to open " << filename << " for input. Terminating!\n";
		return 1;
	}

	// Read in the file, get number of tasks and number of resources (and amount of each)
	int num_tasks = 0, num_resources = 0;
	int* resources_available = nullptr;
	taskvec_t task_list;
	// Multidimensional vector: vector of vectors. Equivalent to Action**
	// action_contianer[0] contains the actions for task 1, [1] for task 2, etc...
	ActionContainer_t action_container;

	input_file >> num_tasks;
	input_file >> num_resources;
	resources_available = new int[num_resources];

	for(int i = 0; i < num_tasks; i++)
	{
		actionvec_t task_actions;
		action_container.push_back(task_actions);
		task_list.push_back(Task(num_resources, i));
	}

	for (int i = 0; i < num_resources; i++)
	{
		input_file >> resources_available[i];

	}

	// Read in the actions
	string action_type;
	action_t type;
	int task_id = 0, delay = 0, resource_id = 0, amount = 0;
	while(input_file >> action_type)
	{
		type = stringToActionType(action_type);
		input_file >> task_id;
		input_file >> delay;
		input_file >> resource_id;
		input_file >> amount;
		task_id--;

		Action action(type, task_id, delay, resource_id - 1, amount);
		action_container[task_id].push_back(action);
	}

	input_file.close();


	// Main loop for OptimisticResourceManager
	OptimisticResourceManager optimistic_manager =
			OptimisticResourceManager(num_resources, num_tasks, resources_available);
	int current_cycle = 0;
	while (!areAllTasksFinished(task_list))
	{
		current_cycle = optimistic_manager.getCycle();
		cout << "Cycle " << current_cycle << " - " << current_cycle + 1 << "\n";
		// Process each task's action for this cycle
		for (int i = 0; i < num_tasks; i++)
		{
			if (task_list[i].isAborted())
			{
				continue;
			}

			optimistic_manager.dispatchAction(action_container[i][0], task_list[i]);
			if (!task_list[i].isBlocked())
			{
				action_container[i].erase(action_container[i].begin());
			}
		}
		optimistic_manager.incrementCycle();
	}

	// Main loop for Banker

	// cleanup
	delete resources_available;
	return 0;
}

static action_t stringToActionType(const string &str){
	action_t retVal;
	if (str.compare("initiate") == 0)
	{
		retVal = INITIATE;
	}
	else if (str.compare("request") == 0)
	{
		retVal = REQUEST;
	}
	else if (str.compare("release") == 0)
	{
		retVal = RELEASE;
	}
	else
	{
		retVal = TERMINATE;
	}
	return retVal;
}

static bool areAllTasksFinished(const taskvec_t &tasklist)
{
	bool retVal = true;
	for (int i = 0; i < tasklist.size(); i++)
	{
		if (tasklist[i].getTimeTerminated() < 0 && !tasklist[i].isAborted())
		{
			retVal = false;
		}
	}
	return retVal;
}
