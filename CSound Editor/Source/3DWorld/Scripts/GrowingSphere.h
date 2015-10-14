#pragma once

#include <vector>

#include <include/glm.h>
#include <Event/EventListener.h>


class CSoundComponentProperty;
class GameObject;
template <class T>
class TimerEvent;

class GrowingSphere
	: public EventListener
{
	public:
		GrowingSphere();
		virtual ~GrowingSphere() {};

		void Init();
		void Start();
		void Stop();

	private:
		void ClearEvents();
		void Update();
		void Reset();
		void OnEvent(EventType eventID, void *data);
		void OnEvent(const string& eventID, void *data);

	private:
		GameObject* camera;
		GameObject *visibleSphere;

		float expandingSpeed;
};