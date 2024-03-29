#include <algorithm>
#include <iostream>
#include <fstream>
#include <math.h>
#include <string>
#include <vector>
#include "data_types.h"

using namespace std;

static action_t stringToActionType(const string &str);
static bool areAllTasksFinished(const taskvec_t &tasklist);
static void printTaskStats(const taskvec_t &tasklist);
static bool compareTasksForSort( const Task &a, const Task &b);


int main(int argc, char** argv)
{
	// Set up input stream and open the file
	string filename = "";
	ifstream input_file;

	if (argc < 2)
	{
		filename = "./data/input-13.txt";
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
	taskvec_t task_list, banker_task_list;
	// Multidimensional vector: vector of vectors. Equivalent to Action**
	// action_contianer[0] contains the actions for task 1, [1] for task 2, etc...
	ActionContainer_t action_container, banker_action_container;

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
	banker_action_container = action_container;
	input_file.close();

	// Bind action pointers
	for (int i = 0; i < num_tasks; i++)
	{
		task_list[i].bindActionPointer(action_container[i][0]);
	}
	banker_task_list = task_list;

	// Main loop for OptimisticResourceManager
	OptimisticResourceManager optimistic_manager =
			OptimisticResourceManager(num_resources, num_tasks, resources_available);

	int current_cycle = 0;
	bool * blocked_processes = new bool[num_tasks];

	while (!areAllTasksFinished(task_list))
	{
		current_cycle = optimistic_manager.getCycle();
#ifdef DEBUG
		cout << "Cycle " << current_cycle << " - " << current_cycle + 1 << "\n";
#endif
		// Process each task's action for this cycle. Start with blocked processes (sort by when it was blocked in queue = FIFO)
		stable_sort(task_list.begin(), task_list.end(), compareTasksForSort);

		// For each task, if it's already terminated or aborted, go to the next one
		// Otherwise, dispatch the action and handle the blocked processes (by updating time blocked)
		// If a task was successfully dispatched and the delay time has elapsed (or was 0),
		// Clear that action out of the action container and get a new action pointer for that task (next action)
		for (int i = 0; i < num_tasks; i++)
		{
			Task* current_task = &task_list[i];
			int task_id = current_task->getId();

			if (current_task->isDoneOrAborted())
				continue;

			optimistic_manager.dispatchAction(*current_task);
			if (current_task->isBlocked())
			{
				current_task->incrementTimeBlocked();
			}
			else if(current_task->getDelay() == 0)
			{
				action_container[task_id].erase(action_container[task_id].begin());
				current_task->bindActionPointer(action_container[task_id][0]);
			}
		}

		// Loop in this cycle while no request can be satisfied
		// HandleDeadlock terminates a process if it finds deadlock.
		while (optimistic_manager.handleDeadlock(task_list)
				&& !areAllTasksFinished(task_list))
		{
			if (optimistic_manager.canSatisfyAnyRequest(task_list))
				break;
		}
		optimistic_manager.commitReleasedResources();
		optimistic_manager.incrementCycle();
	}
	stable_sort(task_list.begin(), task_list.end(), compareTasksForSort);
	cout << "\n\tFIFO\n";
	printTaskStats(task_list);

	// Main loop for OptimisticResourceManager
	BankerResourceManager banker_manager =
			BankerResourceManager(num_resources, num_tasks, resources_available);
	action_container = banker_action_container;
	task_list = banker_task_list;

	// Bind action pointers
	for (int i = 0; i < num_tasks; i++)
	{
		task_list[i].bindActionPointer(action_container[i][0]);
	}

	while (!areAllTasksFinished(task_list))
	{
		current_cycle = banker_manager.getCycle();
#ifdef DEBUG
		cout << "Cycle " << current_cycle << " - " << current_cycle + 1 << "\n";
#endif
		// Process each task's action for this cycle. Start with blocked processes
		//stable_sort(task_list.begin(), task_list.end(), compareTasksForSort);

		// Process each task's action for this cycle. Start with blocked processes (sort by when it was blocked in queue = FIFO)
		stable_sort(task_list.begin(), task_list.end(), compareTasksForSort);
		for (int i = 0; i < num_tasks; i++)
		{
			Task* current_task = &task_list[i];
			int task_id = current_task->getId();

			if (current_task->isDoneOrAborted())
				continue;

			banker_manager.dispatchAction(*current_task);
			if (current_task->isBlocked())
			{
				current_task->incrementTimeBlocked();
			}
			else if(current_task->getDelay() == 0)
			{
				action_container[task_id].erase(action_container[task_id].begin());
				current_task->bindActionPointer(action_container[task_id][0]);
			}
		}
		banker_manager.commitReleasedResources();
		banker_manager.incrementCycle();
	}

	stable_sort(task_list.begin(), task_list.end(), compareTasksForSort);
	cout << "\n\tBanker\n";
	printTaskStats(task_list);

	// cleanup
	delete blocked_processes;
	delete resources_available;
	return 0;
}

static action_t stringToActionType(const string &str){
	action_t ret_val;
	if (str.compare("initiate") == 0)
	{
		ret_val = INITIATE;
	}
	else if (str.compare("request") == 0)
	{
		ret_val = REQUEST;
	}
	else if (str.compare("release") == 0)
	{
		ret_val = RELEASE;
	}
	else
	{
		ret_val = TERMINATE;
	}
	return ret_val;
}

// See if there's any process that's not either done or aborted
static bool areAllTasksFinished(const taskvec_t &tasklist)
{
	bool ret_val = true;
	for (unsigned int i = 0; i < tasklist.size(); i++)
	{
		if (!tasklist[i].isDoneOrAborted())
		{
			ret_val = false;
		}
	}
	return ret_val;
}

// Print output info based on the info in each task (in tasklist)
static void printTaskStats(const taskvec_t &tasklist)
{
	int total_cycles = 0;
	int total_blocked_percent = 0;
	int total_blocked_cycles = 0;

	for (unsigned int i = 0; i < tasklist.size(); i++)
	{
		if (tasklist[i].isAborted())
		{
			cout << "Task # " << i + 1 << "\taborted\n";
			continue;
		}
		int task_cycles = tasklist[i].getTimeTerminated();
		int blocked_cycles = tasklist[i].getTimeBlocked();
		int blocked_percent = floor((float)blocked_cycles/(float)task_cycles*100.f + 0.5f);

		cout << "Task # " << i + 1
				<< "\t" << task_cycles << "\t"
				<< blocked_cycles << "\t" << blocked_percent << "%\n";

		total_blocked_cycles += blocked_cycles;
		total_cycles += task_cycles;
	}
	total_blocked_percent = floor((float)total_blocked_cycles/(float)total_cycles*100.f + 0.5f);
	cout << "Total \t\t" << total_cycles << "\t" << total_blocked_cycles << "\t" << total_blocked_percent << "%\n\n";
}

// If a task is blocked, it should come first. These blocked processes should be sorted
// by the time they were blocked (FIFO). This is used in the OptimisticResourceManager loop
static bool compareTasksForSort(const Task &a, const Task &b)
{
	if (a.isBlocked() && !b.isBlocked() )
	{
		return true;
	}
	if (!a.isBlocked() && b.isBlocked())
	{
		return false;
	}
	if (a.isBlocked() && b.isBlocked())
	{
		if (a.getBlockedSince() == b.getBlockedSince())
		{
			return a.getId() < b.getId();
		}
		return a.getBlockedSince() < b.getBlockedSince();
	}
	return a.getId() < b.getId();
}

