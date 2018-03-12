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
	resources_held = new int[n_resources];
	resources_claimed = new int[n_resources];
	num_resources = n_resources;
	delay = 0;
}

Task::~Task()
{
//	delete resources_held;
//	delete resources_claimed;
}

// Sanity check for out of bounds array access
bool Task::sanityCheck(int i) {
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

void Task::setTimeTerminated(int i)
{
	time_terminated = i;
}

void Task::setTimeCreated(int i)
{
	time_created = i;
}

void Task::block()
{
	blocked = true;
}

void Task::unblock()
{
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


// Get methods
int Task::getResourceHeld(int i)
{
	assert (sanityCheck(i));
	return resources_held[i];
}

int Task::getResourceClaim(int i)
{
	assert (sanityCheck(i));
	return resources_claimed[i];
}

int Task::getId()
{
	return id;
}

int Task::getDelay()
{
	return delay;
}

int Task::getTimeCreated()
{
	return time_created;
}

int Task::getTimeTerminated() const
{
	return time_terminated;
}

bool Task::isBlocked()
{
	return blocked;
}

bool Task::isAborted() const
{
	return aborted;
}

int Task::getTimeBlocked()
{
	return time_blocked;
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
