#pragma once

#include"Listener.h"

template<class TObject>
class MethodEventHandler : public Listener
{
public:
	using TMethod = void(TObject::*)(Event*);

private:
	TObject* m_object;
	TMethod m_method;

public:
	MethodEventHandler(TObject* object, TMethod method)
		:
		m_object(object),
		m_method(method)
	{
		if (m_method == nullptr)
			throw "Method ref is null";
	}

	virtual void HandleEvent(Event* event) override final
	{
		(*m_object.*m_method)(event);
	}
};

template<class TObject>
Listener& createMethodEventHandler(TObject& object, void(TObject::* method)())
{
	return *new MethodEventHandler<TObject>(object, method);
}

#define METHOD_HANDLER(Object, Method) createMethodEventHandler(Object, Method)
