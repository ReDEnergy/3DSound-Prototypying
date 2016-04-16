#pragma once
#include <vector>
#include <list>

using namespace std;

#include <include/glm.h>
#include <Event/EventListener.h>
#include <Component/ObjectInput.h>

class GameObject;
class CSound3DSource;
class SceneIntersection;
template <class T>
class TimerEvent;

class SoundTriggerList;
struct ImpactSoundConfig;

class ImpactSoundScript
	: public EventListener,
	public ObjectInput
{

	public:
		ImpactSoundScript();
		virtual ~ImpactSoundScript() {};

		void Init();

	private:
		void Start(ImpactSoundConfig *config);
		void Stop();
		void Reset();
		void Update();

		void OnKeyPress(int key, int mods);
		void OnEvent(EventType eventID, void *data);
		void OnEvent(const string& eventID, void *data);

	private:
		bool isRunning;
		ImpactSoundConfig *config;
};