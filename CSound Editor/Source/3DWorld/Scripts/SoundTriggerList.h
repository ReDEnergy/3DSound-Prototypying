#pragma once
#include <vector>
#include <list>

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

	private:
		void TriggerAction(CSound3DSource * obj);

	private:
		CSound3DSource* activeSource;
		float lastTriggerTime;
		list<CSound3DSource*> triggerList;
		TimerEvent<string> *triggerEvent;
};