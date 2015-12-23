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
		CSoundPlayer(CSoundScore *score);
		~CSoundPlayer();

		bool Init();
		bool Reload();
		void Clean();

		void Play();
		bool IsPlaying();
		void Pause();
		void Stop();

		void InitControlChannels();
		void SetControl(const char* channelName, float value, bool forceUpdate = false);

	private:
		CSoundScore *score;

		void* ThreadID;
		unordered_map<string, MYFLT*> channels;
		unordered_map<string, int> invalidChannels;
		// TODO csnd6.lib missing
		CsoundPerformanceThread* perfThread;

	// Must be public benecause we need to access them from a separated thread
	public:
		Csound *csound;
		bool PERF_STATUS;
};