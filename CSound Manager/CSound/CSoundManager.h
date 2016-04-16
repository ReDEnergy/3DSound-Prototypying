#pragma once
#include <string>
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
		int supportedChannels;
		int isOutput;
		uint index;
		uint nrActiveChannels;
		int mapping[8];
		bool changed;
};

struct CsoundInstrumentOption {
	string name;
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
		list<string> GetPropertyTypes() const;
		void RegisterType(const char *name);

		const vector<AudioDevice*>& GetOutputDevices() const;

		AudioDevice* GetActiveDac() const;
		uint GetActiveDacID() const;
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

		uint activeDacID;
		unsigned int controlSamples;
		unordered_map<string, char> propertyTypes;
		vector<AudioDevice*> outputDevices;

		string csOptionsRender;
		string csInstrOptionsRender;
		unordered_map<string, string> csOptions;
		unordered_map<string, CsoundInstrumentOption> instrumentOptions;
};