#pragma once
#include <string>
#include <unordered_map>

#include <include/dll_export.h>

#include <CSound/CSoundForward.h>


struct AudioDevice
{
	char deviceName[64];
	char deviceID[64];
	char realTimeModule[64];
	int supportedChannels;
	int isOutput;
};

class DLLExport CSoundManager
{
	protected:
		CSoundManager();
		~CSoundManager();

	public:
		void LoadConfig();
		void Clear();
		CSoundScore* CreateScore();
		CSoundScore* GetScore(const char *name) const;

		// Visual programming tags
		list<string> GetPropertyTypes() const;
		void RegisterType(const char *name);

		const vector<AudioDevice*>& GetOutputDevices() const;

		const char* GetCsOptionsRender() const;
		const char* GetInstrumentOptionsRender() const;

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
		EntityStorage<CSoundScore> *scores;
		EntityStorage<CSoundComponent> *components;
		EntityStorage<CSoundInstrument> *instruments;
		EntityStorage<CSoundInstrumentBlock> *blocks;

	private:
		unsigned int controlSamples;
		unordered_map<string, char> propertyTypes;
		vector<AudioDevice*> outputDevices;

		string csOptionsRender;
		string csInstrOptionsRender;
		unordered_map<string, string> csOptions;
		unordered_map<string, unsigned int> instrumentOptions;
};