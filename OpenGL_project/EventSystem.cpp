#include "EventSystem.h"
//Helpers
bool EventSystem::isAlreadyRegistered(EVENTID eventId, Listener* client)
{
	bool alredyRegistered = false;

	using iter = std::multimap<EVENTID, Listener*>::iterator;

	//Range iterator
	std::pair<iter, iter> range;

	//Find all the clients able to receive this event
	range = database.equal_range(eventId);

	//Go through the list of clients
	for (iter it = range.first; it != range.second; it++)
	{
		if ((*it).second == client)
		{
			alredyRegistered = true;
			break;
		}
	}

	return alredyRegistered;
}

void EventSystem::DispatchEvent(Event* event)
{
	using iter = std::multimap<EVENTID, Listener*>::iterator;

	//Range iterator
	std::pair<iter, iter> range;

	//Get all the clients who respond to this event
	range = database.equal_range(event->getEventId());

	//For every client
	for (iter it = range.first; it != range.second; it++)
	{
		//Handle the event
		(*it).second->HandleEvent(event);
	}
}

//Functions
EventSystem* EventSystem::Instance()
{
	static EventSystem instance;
	return &instance;
}

void EventSystem::RegisterClient(EVENTID event, Listener* client)
{
	//If the client is null, or already registered
	if (!client || isAlreadyRegistered(event, client))
	{
		return;
	}

	//Insert the client into the database
	database.insert(make_pair(event, client));
}

void EventSystem::UnregisterClient(EVENTID event, Listener* client)
{
	using iter = std::multimap<EVENTID, Listener*>::iterator;

	//Range iterator
	std::pair<iter, iter> range;

	//Find all the clients able to receive this event
	range = database.equal_range(event);

	//For every client who can respond to this event
	for (iter it = range.first; it != range.second; it++)
	{
		//if this is the guy we remove
		if ((*it).second == client)
		{
			//Erase the bastard and quit
			it = database.erase(it);
			break;
		}
	}
}

void EventSystem::UnregisterAll(Listener* client)
{
	//Iterator
	std::multimap<EVENTID, Listener*>::iterator iter = database.begin();

	//For every entry in the database
	while (iter != database.end())
	{
		//If this is the choosen one
		if ((*iter).second == client)
		{
			//Destroy him
			iter = database.erase(iter);
		}
		else
		{
			//Move on
			iter++;
		}
	}
}

void EventSystem::SendEvent(EVENTID eventId, void* data)
{
	//Create the events
	Event newEvent(eventId, data);
	//Queue it up
	currentEvents.push_back(newEvent);
}

void EventSystem::ProcessEvents()
{
	//For every event
	while (currentEvents.size())
	{
		//Dispatch the event
		DispatchEvent(&currentEvents.front());
		//Remove it from the list
		currentEvents.pop_front();
	}
}

void EventSystem::ClearEvents()
{
	//Clear events
	currentEvents.clear();
}

void EventSystem::Shutdown()
{
	database.clear();
	currentEvents.clear();
}
