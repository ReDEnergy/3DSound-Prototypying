#pragma once
#include <list>
#include <string>
#include <vector>
#include <unordered_map>

#include <include/dll_export.h>

#include <CSound/CSoundForward.h>

namespace pugi
{
	class xml_document;
};

class AudioDevice
{
	public:
		AudioDevice();

	public:
		char deviceName[64];
		char deviceID[64];
		char realTimeModule[64];
		uint supportedChannels;
		int isOutput;
		uint index;
		uint nrActiveChannels;
		int mapping[8];
		bool changed;
};

struct CsoundInstrumentOption {
	std::string name;
	unsigned int value;
};

class DLLExport CSoundManager
{
	protected:
		CSoundManager();
		~CSoundManager();

	public:
		void LoadConfig();
		void SaveConfigFile();
		void Clear();
		CSoundSynthesizer* CreateScore();
		CSoundSynthesizer* GetScore(const char *name) const;

		// Visual programming tags
		std::list<std::string> GetPropertyTypes() const;
		void RegisterType(const char *name);

		const std::vector<AudioDevice*>& GetOutputDevices() const;

		AudioDevice* GetActiveDac() const;
		int GetActiveDacID() const;
		const char* GetCsOptionsRender() const;
		const char* GetInstrumentOptionsRender() const;
		unsigned int GetInstrumentOption(const char* propertyName) const;

		void SetActiveDac(uint dacID);
		void SetCsOptionsParameter(const char* property, const char* value);
		void SetCsInstrumentOption(const char * property, unsigned int value);

	private:
		void LoadScoreConfig();
		void LoadTemplates();
		void LoadInstruments();
		void LoadScores();

		void RenderCsOptions();
		void RenderInstrumentOptions();

	public:
		EntityStorage<CSoundSynthesizer> *scores;
		EntityStorage<CSoundComponent> *components;
		EntityStorage<CSoundInstrument> *instruments;
		EntityStorage<CSoundInstrumentBlock> *blocks;

	private:
		pugi::xml_document *configXML;

		int activeDacID;
		unsigned int controlSamples;
		std::unordered_map<std::string, char> propertyTypes;
		std::vector<AudioDevice*> outputDevices;

		std::string csOptionsRender;
		std::string csInstrOptionsRender;
		std::unordered_map<std::string, std::string> csOptions;
		std::unordered_map<std::string, CsoundInstrumentOption> instrumentOptions;
};
