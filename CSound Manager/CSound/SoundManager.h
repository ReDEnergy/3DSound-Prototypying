#pragma once
#include <vector>
#include <string>

#include <include/dll_export.h>

#include <CSound/CSoundForward.h>

class AudioDevice;

class DLLExport SoundManager
{
	private:
		SoundManager();
		~SoundManager() {};

	public:

		static void Init();
		static CSoundManager* GetCSManager();

		static void SetGlobalOutputModel(const char* model);
		static void SetGlobalOutputModelIndex(unsigned int index);

		static CSoundInstrumentBlock* GetGlobalOutputModel();
		static unsigned int GetGlobalOutputModelIndex();
		static std::vector<std::string>& GetDefaultChannelNames();
		static const char* GetChannelMapping(AudioDevice *device, unsigned int channelCount);

	private:
		static std::vector<std::string> standardChannels;
		static std::vector<std::string> output4Channels;
		static std::vector<std::string> output8Channels;
		static CSoundManager* csManager;
		static CSoundInstrumentBlock *globalOutputModel;
		static uint globalOutputModelIndex;
};