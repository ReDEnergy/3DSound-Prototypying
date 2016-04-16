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

class SoundTriggerList;
struct SweepingPlaneConfig;

class SweepingPlaneScript
	: public EventListener
{

	public:
		SweepingPlaneScript();
		virtual ~SweepingPlaneScript() {};

		void Init();

	private:
		void Start(SweepingPlaneConfig *config);
		void Stop();
		void Resume();
		void Reset();
		void Update();
		void TriggerFeedbackTick();
		void OnTriggerSound(CSound3DSource *source);

		void OnEvent(EventType eventID, void *data);
		void OnEvent(const string& eventID, void *data);

	private:

		SweepingPlaneConfig *config;

		GameObject* camera;
		GameObject *virtualPlane;
		GameObject *visiblePlane;
		TimerEvent<string> *scanPauseEvent;

		SceneIntersection *computeIntersection;
		SoundTriggerList *triggerList;

		float scanningAzimuth;
		CSound3DSource *feedbackTick;
		int nrFeedbackTicks;
		int nrTicksReleased;
		float sweepingTime;
};