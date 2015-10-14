#pragma once

#include <vector>

#include <include/glm.h>
#include <Event/EventListener.h>


class CSoundComponentProperty;
class CSound3DSource;
template <class T>
class TimerEvent;

class HrtfRecorder
	: public EventListener
{
	public:
		HrtfRecorder();
		virtual ~HrtfRecorder() {};
		void Start();
		void Stop();

	private:
		void ClearEvents();
		void Update();
		void OnEvent(const string& eventID, void *data);

	private:
		CSound3DSource *gameObj;
		bool forceUpdate;
		unsigned int timelinePos;
		vector<glm::vec3> positions;
		TimerEvent<string> *playbackEvent;
		TimerEvent<string> *waitEvent;
};