#pragma once
#include <vector>

#include <include/glm.h>
#include <Event/EventListener.h>

class CSoundComponentProperty;
class GameObject;
class SceneIntersection;
template <class T>
class TimerEvent;

struct MovingPlaneConfig;

class MovingPlaneScript
	: public EventListener
{
	public:
		MovingPlaneScript();
		virtual ~MovingPlaneScript() {};

		void Init();

	private:
		void Start();
		void Stop();

		void Update();
		void Reset();

		void OnEvent(EventType eventID, void *data);
		void OnEvent(const string& eventID, void *data);
		void ClearEvents();

	private:
		MovingPlaneConfig *config;

		GameObject* camera;
		GameObject *virtualPlane;
		GameObject *visiblePlane;
		TimerEvent<string> *scanPauseEvent;

		SceneIntersection *computeIntersection;
};