#pragma once

#include <vector>

#include <include/glm.h>
#include <Event/EventListener.h>


class CSoundComponentProperty;
class CSound3DSource;
template <class T>
class TimerEvent;
class Camera;

struct HeadphoneTestConfig
{
	HeadphoneTestConfig()
	{
		waitForInput = true;
		randomTest = true;
		prepareTime = 2;
		samplePlaybackDuration = 1000;
		sampleInterval = 2;
		randomIterations = 1;
		testName = "";
		azimuthValues = { -60, 0, 60 };
		elevationValues = { -60, 0, 60 };
	}

	bool waitForInput;
	bool randomTest;
	float prepareTime;
	float samplePlaybackDuration;
	float sampleInterval;
	unsigned int randomIterations;
	vector<float> azimuthValues;
	vector<float> elevationValues;
	string testName;
};

struct AnswerEntry
{
	glm::vec2 source;
	glm::vec2 answer;
	float responseTime; // seconds
	bool correct;
};

class HeadphoneTestScript
	: public EventListener
{
	public:
		HeadphoneTestScript();
		virtual ~HeadphoneTestScript() {};

		void Start(HeadphoneTestConfig config);
		bool VerifyAnswer(float azimuth, float elevation);
		void Stop();

		void SetSoundFile(const char* soundFile);
		void ResetOptions();

	private:
		glm::vec3 ComputePosition(float azimuth, float elevation);
		void ClearEvents();
		void PlayCurrentSample();
		void WaitForNextSample();
		void Update();
		void SaveTestAnswers() const;
		void ExportTestResultsAsCSV() const;
		void OnEvent(const string& eventID, void *data);

	private:
		CSound3DSource *gameObj;
		unsigned int timelinePos;
		vector<glm::vec3> positions;
		vector<AnswerEntry> answers;
		TimerEvent<string> *playbackEvent;
		TimerEvent<string> *waitEvent;
		TimerEvent<string> *prepareEvent;

		HeadphoneTestConfig config;
		bool started;
		double startTimeCurrentTest;
		Camera *sceneCamera;
};