#pragma once

#include <Event/EventListener.h>


using namespace std;

class UIEvents
	: public EventListener
{
	public:
		UIEvents();
		~UIEvents();

	public:
		void OnEvent(const string& eventID, void *data);
};
