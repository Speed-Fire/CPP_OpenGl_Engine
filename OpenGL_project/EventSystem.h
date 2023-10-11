#pragma once

#include <list>
#include <map>

#include "Event.h"
#include "Listener.h"

/*
	Система обработки событий.
	
	Конструкция создания элемента для добавления в систему обработки:

		Listener* handler = new MethodEventHandler<_TYPE_>(_TYPE_ _Object_, &_TYPE_::_method);


	Чтобы отправить параметр в событие сделайте так:
		int k = 5;	// ваш параметр
		void* voidPtr = &k;

		...->SendEvent(_EVENT_NAME_, voidPtr);

	Для доступа к параметру события используйте static_cast<_TYPE_>(event->getParameter()).



*///

class EventSystem
{
private:
	//Database of clients and their events
	std::multimap<EVENTID, Listener*> database;

	//List of events to be processed
	std::list<Event> currentEvents;

	//Helpers
	bool isAlreadyRegistered(EVENTID eventId, Listener* client);

	void DispatchEvent(Event* event);

	//Singleton, deny access to constructors/destructors
	//constructor
	EventSystem() {}
	//destructor
	~EventSystem() { this->Shutdown(); }

	//Copy constructor
	EventSystem(const EventSystem& rhs) {}

	//Assignment operator
	EventSystem& operator=(const EventSystem& rhs) {}
public:
	//Returns an instance of the Event system
	static EventSystem* Instance();

	//Registers a client to start listening for an event
	void RegisterClient(EVENTID event, Listener* client);

	//Unregisters a client from a specific event
	void UnregisterClient(EVENTID event, Listener* client);

	//Unregister a client from all events
	void UnregisterAll(Listener* client);

	//Send an event
	void SendEvent(EVENTID eventId, void* data = 0);

	//Process all events
	void ProcessEvents();

	//Clear events
	void ClearEvents();

	//Shutdown the Event system
	void Shutdown();
};