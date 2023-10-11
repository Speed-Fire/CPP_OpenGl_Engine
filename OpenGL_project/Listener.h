#pragma once

#include "Event.h"

class Listener
{
private:

public:
	Listener() {}
	virtual ~Listener() {}

	//Handles the event passed to it
	virtual void HandleEvent(Event* event) = 0;
};