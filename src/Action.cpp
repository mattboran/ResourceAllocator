#include "data_types.h"

// Constructor, Copy Constructor, and get methods for Action

Action::Action(action_t typ, int d, int res_id, int amt)
{
	type = typ;
	delay = d;
	resource_id = res_id;
	amount = amt;
}

Action::Action(const Action &action)
{
	type = action.type;
	delay = action.delay;
	resource_id = action.resource_id;
	amount = action.amount;
}

action_t Action::getType() const
{
	return type;
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
