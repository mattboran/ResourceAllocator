#include "data_types.h"

// Constructor, Copy Constructor, and get methods for Action

Action::Action(action_t typ, int t_id, int d, int res_id, int amt)
{
	type = typ;
	task_id = t_id;
	delay = d;
	resource_id = res_id;
	amount = amt;
}

Action::Action(const Action &action)
{
	type = action.type;
	task_id = action.task_id;
	delay = action.delay;
	resource_id = action.resource_id;
	amount = action.amount;
}

// Get methods for Action. All are const because Actions are immutable
action_t Action::getType() const
{
	return type;
}

int Action::getTaskId() const
{
	return task_id;
}

int Action::getStartTime() const
{
	return delay;
}

int Action::getResourceId() const
{
	return resource_id;
}

int Action::getAmount() const
{
	return amount;
}
