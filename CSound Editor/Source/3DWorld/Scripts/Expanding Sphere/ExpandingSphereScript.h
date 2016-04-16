#pragma once
#include <vector>

#include <include/glm.h>
#include <Event/EventListener.h>

class AudioSource;
class CSoundComponentProperty;
class GameObject;
class Transform;
class SceneIntersection;
template <class T>
class TimerEvent;

struct ExpandingSphereConfig;

class ExpandingSphereScript
	: public EventListener
{

	public:
		ExpandingSphereScript();
		virtual ~ExpandingSphereScript() {};

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
		ExpandingSphereConfig *config;

		GameObject* camera;
		GameObject *visibleSphere;
		float sphereScale;
		TimerEvent<string> *scanPauseEvent;
		AudioSource* startSound;

		SceneIntersection *computeIntersection;
};