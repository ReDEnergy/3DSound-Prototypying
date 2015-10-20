#pragma once

#include <vector>

#include <include/glm.h>
#include <Event/EventListener.h>


class CSoundComponentProperty;
class GameObject;
class PlaneIntersection;
template <class T>
class TimerEvent;

class MovingPlane
	: public EventListener
{
	public:
		MovingPlane();
		virtual ~MovingPlane() {};

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
		GameObject *virtualPlane;
		GameObject *visiblePlane;

		PlaneIntersection *computeIntersection;

		float planeSpeed;
};