#pragma once

#include <list>
#include <unordered_map>
#include <string>

#include <Event/EventType.h>
#include <Event/EventListener.h>

using namespace std;

class TimedEvent
{
	public:
		TimedEvent(EventListener *E, EventType Event, Object *data, float delaySeconds);
		bool Update();

	private:
		EventListener *event;
		EventType type;
		Object *data;
		float triggerTime;
};

class DLLExport EventSystem
{
	protected:
		EventSystem();
		~EventSystem();

	public:
		void Subscribe(EventListener *E, const string &eventID);
		void EmitSync(string eventID, Object *data);

		void Subscribe(EventType Event, EventListener *E);
		void EmitSync(EventType Event, Object *data);

		void Update();
		void TriggerEvent(EventListener *E, EventType Event, Object *data, float delaySeconds);
		void Clear();

	private:
		unordered_map<string, list<EventListener*>> listeners;
		list<TimedEvent*> timedEvents;
		list<EventListener*> listenersEnum[EventType::SIZE];
};