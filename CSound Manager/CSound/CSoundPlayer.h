#pragma once
#include <unordered_map>

#include <include/dll_export.h>
#include <include/pugixml.h>

#include <CSound/CSoundForward.h>
#include <CSound/CSoundEntity.h>
#include <CSound/Templates/CSoundList.h>

class DLLExport CSoundPlayer
{
	public:
		CSoundPlayer(CSoundSynthesizer *score);
		~CSoundPlayer();

		bool Init();
		bool Reload();
		void Clean();

		void Play();
		bool IsPlaying();
		void Pause();
		void Stop();
		void SetPlaybackTime(float time);
		void SendEvent(char eventType, int nrParams, double * params) const;

		void InitControlChannels();
		void SetControl(const char* channelName, float value, bool forceUpdate = false);

	private:
		CSoundSynthesizer *score;

		void* ThreadID;
		unordered_map<string, MYFLT*> channels;
		unordered_map<string, int> invalidChannels;
		CsoundPerformanceThread* perfThread;

		Csound *csound;
		bool PERF_STATUS;
};