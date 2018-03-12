/*
 * data_types.h
 *
 *  Created on: Mar 10, 2018
 *      Author: matt
 */

#ifndef INCLUDE_DATA_TYPES_H_
#define INCLUDE_DATA_TYPES_H_

typedef enum action_type
{
	INITIATE,
	REQUEST,
	RELEASE,
	TERMINATE
} action_t;

// This class represents one of the actions as listed above.
// Actions are dispatched by the resource manager, and side effects
// are applied to Tasks. Actions are immutable.
class Action
{
	action_t type;
	int task_id, delay, resource_id, amount;
public:
	Action(action_t typ, int t_id, int d, int res_id, int amt);
	Action(const Action &action);

	action_t getType() const;
	int getTaskId() const;
	int getStartTime() const;
	int getResourceId() const;
	int getAmount() const;
};

// Tasks are the "running" blocks of work that request, claim, and otherwise
// consume resources.
class Task {
	int* resources_held;
	int* resources_claimed;
	int id, time_created, time_blocked, time_terminated, num_resources, delay;
	bool blocked;
	bool sanityCheck(int i);
public:
	Task(int n_resources, int i);
	~Task();
	void setResourceHeld(int i, int amount);
	void setResourceClaimed(int i, int amount);
	void setDelay(int i);
	void setTimeTerminated(int i);
	void setTimeCreated(int i);
	void block();
	void incrementTimeBlocked();
	void unblock();
	int getResourceHeld(int i);
	int getResourceClaim(int i);
	int getId();
	int getDelay();
	int getTimeCreated();
	int getTimeTerminated() const;
	int getTimeBlocked();

	void grantResources(int i, int amount);
	void releaseResources(int i, int amount);
};

// The ResourceManager class is the parent class for Optimistic and Banker resource
// managers. The class knows the total resources claimed, how many are available,
// and the total number of tasks.
class ResourceManager {
	int* total_resources;
	int* resources_available;
	int* resources_claimed;
	int num_tasks, num_resources, cycle;

	virtual void dispatchInitiate(const Action &action, Task& task);
	virtual void dispatchRequest(const Action &action, Task& task);
	virtual void dispatchRelease(const Action &action, Task& task);
	virtual void dispatchTerminate(const Action &action, Task& task);
public:
	ResourceManager(int n_resources, int tasks, int* resources_initial);
	virtual ~ResourceManager();
	void reset();
	void incrementCycle();
	int getCycle();
	virtual void dispatchAction(const Action &action, Task& task);
};

// OptimisticResourceManager dispatches actions on tasks.
class OptimisticResourceManager : public ResourceManager
{
	void dispatchInitiate(const Action &action, Task& task);
	void dispatchRequest(const Action &action, Task& task);
	void dispatchRelease(const Action &action, Task& task);
	void dispatchTerminate(const Action &action, Task& task);

public:
	OptimisticResourceManager(int num_resources, int tasks, int* resources_initial);
	void dispatchAction(const Action &action, Task& task);
};

// BankerResourceManager likewise dispatches actions on tasks.
class BankerResourceManager : public ResourceManager
{

};


#endif /* INCLUDE_DATA_TYPES_H_ */
