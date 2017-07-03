#pragma once
#include <unordered_map>

#include <include/dll_export.h>
#include <include/pugixml.h>

#include <CSound/CSoundConfig.h>
#include <CSound/CSoundForward.h>
#include <CSound/Templates/CSoundList.h>

class DLLExport CSoundPlayer
{
	public:
		CSoundPlayer();
		CSoundPlayer(CSoundSynthesizer *score);
		CSoundPlayer(const CSoundPlayer & player);
		~CSoundPlayer();

		bool LoadCSD(std::string file, std::string location = CsoundManagerNS::ROOT);
		bool LoadSynthesizer(CSoundSynthesizer * score, std::string location = CsoundManagerNS::ResourcePath::GENERATED_CSD);
		// Loads a CSD file using CsoundManagerNS::ResourcePath::CSD_DATABASE as lookup path
		bool LoadCSDFromDB(std::string file);
		bool Reload();
		void Clean();

		void Play();
		bool IsPlaying() const;
		void SetPlaybackState(bool value);

		void Pause();
		void TogglePause();
		void Stop();

		void SetPlaybackTime(float time);
		void GetPlaybackTime() const;
		void SendEvent(char eventType, int nrParams, double * params) const;

		std::string GetCSDFile(bool fullPath = true) const;
		CSoundSynthesizer* GetSynthesizer() const;
		Csound* GetCsoundHandle() const;

		void InitControlChannels();
		void SetControl(const char* channelName, float value, bool forceUpdate = false);

	private:
		bool LoadCSDFile(const char * file);

	private:
		bool isPlaying;
		CSoundSynthesizer *score;
		std::string synthFile;
		std::string csdFile;

		void* ThreadID;
		std::unordered_map<std::string, MYFLT*> channels;
		std::unordered_map<std::string, int> invalidChannels;
		CsoundPerformanceThread* perfThread;

		Csound *csound;
};