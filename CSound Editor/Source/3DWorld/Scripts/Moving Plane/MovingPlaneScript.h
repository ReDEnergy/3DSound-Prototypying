#pragma once
#include <vector>
#include <list>

using namespace std;

#include <include/glm.h>
#include <Event/EventListener.h>

class GameObject;
class CSound3DSource;
class SceneIntersection;
template <class T>
class TimerEvent;

struct MovingPlaneConfig;
class AudioSource;
class SoundTriggerList;

class MovingPlaneScript
	: public EventListener
{
	public:
		MovingPlaneScript();
		virtual ~MovingPlaneScript() {};

		void Init();

	private:
		void Start(MovingPlaneConfig *config);
		void Stop();
		void Resume();
		void Reset();
		void Update();
		void TriggerFeedbackTick();
		void OnTriggerSound(CSound3DSource * source);

		void OnEvent(EventType eventID, void *data);
		void OnEvent(const string& eventID, void *data);

	private:

		MovingPlaneConfig *config;

		GameObject* camera;
		GameObject *virtualPlane;
		GameObject *visiblePlane;
		TimerEvent<string> *scanPauseEvent;

		SceneIntersection *computeIntersection;
		SoundTriggerList *triggerList;

		AudioSource* feedbackTick;
		int nrFeedbackTicks;
		int nrTicksReleased;
};