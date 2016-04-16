#pragma once
#include <vector>
#include <list>
#include <functional>

using namespace std;

#include <include/glm.h>
#include <Event/EventListener.h>

class CSound3DSource;
template <class T>
class TimerEvent;

class SoundTriggerList
{
	public:
		SoundTriggerList();

		void Add(CSound3DSource *obj);
		bool IsTracked(CSound3DSource *obj);
		void Update();
		void OnTrigger(function<void(CSound3DSource*)>);

	private:
		void TriggerAction(CSound3DSource * obj);

	private:
		float triggerInterval;
		float lastTriggerTime;

		CSound3DSource* activeSource;
		list<CSound3DSource*> triggerList;
		TimerEvent<string> *triggerEvent;
		function<void(CSound3DSource*)> listener;
};