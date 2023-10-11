#pragma once

#include<string>

#define EVENTID std::string

class Event
{
private:
	EVENTID eventId;
	void* parameter;
public:
	Event(EVENTID eventId, void* parameter = 0)
	{
		this->eventId = eventId;
		this->parameter = parameter;
	}

	~Event() {}

	//Accessors
	inline EVENTID getEventId() const { return this->eventId; }
	inline void* getParameter() { return this->parameter; }
};