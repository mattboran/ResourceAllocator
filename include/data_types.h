/*
 * data_types.h
 *
 *  Created on: Mar 10, 2018
 *      Author: matt
 */

#ifndef INCLUDE_DATA_TYPES_H_
#define INCLUDE_DATA_TYPES_H_

#include <vector>

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
	int getDelay() const;
	int getResourceId() const;
	int getAmount() const;
};

// Tasks are the "running" blocks of work that request, claim, and otherwise
// consume resources.
class Task {
	int* resources_held;
	int* resources_claimed;
	int id, time_created, time_blocked, time_terminated, num_resources, delay, blocked_since;
	bool blocked, aborted;
	Action* action_ptr;
	bool sanityCheck(int i) const;
public:
	Task(int n_resources, int i);
	~Task();
	void setResourceHeld(int i, int amount);
	void setResourceClaimed(int i, int amount);
	void setDelay(int i);
	void incrementDelay();
	void setTimeTerminated(int i);
	void setTimeCreated(int i);
	void setBlockedSince(int i);
	void block();
	void incrementTimeBlocked();
	void unblock();
	void abort();
	void bindActionPointer(Action &action);
	int getResourceHeld(int i) const;
	int getResourceClaim(int i) const;
	int getId() const;
	int getDelay() const;
	int getTimeCreated() const;
	int getTimeTerminated() const;
	bool isBlocked() const;
	bool isAborted() const;
	bool isDoneOrAborted() const;
	int getTimeBlocked() const;
	int getBlockedSince() const;
	Action* getActionPointer() const;

	void grantResources(int i, int amount);
	void releaseResources(int i, int amount);
};

typedef std::vector<Task> taskvec_t;
typedef std::vector<Action> actionvec_t;
typedef std::vector<actionvec_t> ActionContainer_t;

// The ResourceManager class is the parent class for Optimistic and Banker resource
// managers. The class knows the total resources claimed, how many are available,
// and the total number of tasks.
class ResourceManager {
	int* total_resources;
	int* resources_available;
	int* resources_claimed;
	int* cycle_resources_changed;
	int num_tasks, num_resources, cycle;

	virtual void dispatchInitiate(const Action &action, Task& task) = 0;
	virtual void dispatchRequest(const Action &action, Task& task) = 0;
	virtual void dispatchRelease(const Action &action, Task& task) = 0;
	virtual void dispatchTerminate(const Action &action, Task& task) = 0;
public:
	ResourceManager(int n_resources, int tasks, int* resources_initial);
	virtual ~ResourceManager();
	bool sanityCheck(int i);
	void reset();
	void incrementCycle();
	void incrementResourcesAvailable(int i, int amount);
	void decrementResourcesAvailable(int i, int amount);
	void commitReleasedResources();
	int getCycle();
	int getResourcesAvailable(int i);
	int getResourcesChanged(int i);
	int getNumResources();
	virtual void dispatchAction(Task& task) = 0;
};

// OptimisticResourceManager dispatches actions on tasks.
class OptimisticResourceManager : public ResourceManager
{
	void dispatchInitiate(const Action &action, Task& task);
	void dispatchRequest(const Action &action, Task& task);
	void dispatchRelease(const Action &action, Task& task);
	void dispatchTerminate(const Action &action, Task& task);
	bool detectDeadlock(taskvec_t &tasklist);
public:
	OptimisticResourceManager(int num_resources, int tasks, int* resources_initial);
	~OptimisticResourceManager() = default;
	void dispatchAction(Task& task);
	bool handleDeadlock(taskvec_t &tasklist);
	bool canSatisfyAnyRequest(taskvec_t &tasklist);
};

// BankerResourceManager likewise dispatches actions on tasks.
class BankerResourceManager : public ResourceManager
{
	void dispatchInitiate(const Action &action, Task& task);
	void dispatchRequest(const Action &action, Task& task);
	void dispatchRelease(const Action &action, Task& task);
	void dispatchTerminate(const Action &action, Task& task);
	bool detectDeadlock(taskvec_t &tasklist);
public:
	BankerResourceManager(int num_resources, int tasks, int* resources_initial);
	~BankerResourceManager() = default;
	void dispatchAction(Task& task);
	bool handleDeadlock(taskvec_t &tasklist);
	bool canSatisfyAnyRequest(taskvec_t &tasklist);
};

#endif /* INCLUDE_DATA_TYPES_H_ */
