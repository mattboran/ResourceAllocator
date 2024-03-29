#include "data_types.h"
#include <assert.h>

// Constructor and Destructor for Task
Task::Task(int n_resources, int i)
{
	id = i;
	time_created = -1;
	time_terminated = -1;
	blocked = false;
	aborted = false;
	time_blocked = 0;
	// Hello memory leak
	resources_held = new int[n_resources];
	resources_claimed = new int[n_resources];
	num_resources = n_resources;
	delay = 0;
	blocked_since = -1;
	action_ptr = nullptr;
}

// TODO: fix memory leaks. shared_ptrs or proper C???
Task::~Task()
{

//	delete resources_held;
//	delete resources_claimed;
}

// Sanity check for out of bounds array access
bool Task::sanityCheck(int i) const {
	return (i >= 0) && (i < num_resources);
}

// Set methods
void Task::setResourceHeld(int i, int amount)
{
	assert (sanityCheck(i));
	resources_held[i] = amount;
}

void Task::setResourceClaimed(int i, int amount)
{
	assert (sanityCheck(i));
	resources_claimed[i] = amount;
}

void Task::setDelay(int i)
{
	delay = i;
}

void Task::incrementDelay()
{
	delay++;
}

void Task::setTimeTerminated(int i)
{
	time_terminated = i;
}

void Task::setTimeCreated(int i)
{
	time_created = i;
}

void Task::setBlockedSince(int i)
{
	blocked_since = i;
}

void Task::block()
{
	blocked = true;
}

void Task::unblock()
{
	blocked_since = -1;
	blocked = false;
}

void Task::abort()
{
	aborted = true;
}

void Task::incrementTimeBlocked()
{
	time_blocked++;
}

void Task::bindActionPointer(Action &action)
{
	action_ptr = &action;
}

// Get methods
int Task::getResourceHeld(int i) const
{
	assert (sanityCheck(i));
	return resources_held[i];
}

int Task::getResourceClaim(int i) const
{
	assert (sanityCheck(i));
	return resources_claimed[i];
}

int Task::getId() const
{
	return id;
}

int Task::getDelay() const
{
	return delay;
}

int Task::getTimeCreated() const
{
	return time_created;
}

int Task::getTimeTerminated() const
{
	return time_terminated;
}

bool Task::isBlocked() const
{
	return blocked;
}

bool Task::isAborted() const
{
	return aborted;
}

bool Task::isDoneOrAborted() const
{
	return getTimeTerminated() >= 0;
}
int Task::getTimeBlocked() const
{
	return time_blocked;
}

int Task::getBlockedSince() const
{
	return blocked_since;
}

Action* Task::getActionPointer() const
{
	return action_ptr;
}

// Additional setting for increment/decrement
void Task::grantResources(int i, int amount)
{
	assert (sanityCheck(i));
	resources_held[i] += amount;
	unblock();
}

void Task::releaseResources(int i, int amount)
{
	assert (sanityCheck(i));
	resources_held[i] -= amount;
}
