#pragma once
#include <include/dll_export.h>

#include <unordered_map>
#include <string>

#include <CSound/CSoundForward.h>

class DLLExport CSoundManager
{
	protected:
		CSoundManager();
		~CSoundManager();

	public:
		void Init();
		void Clear();
		CSoundScore* CreateScore();
		CSoundScore* GetScore(const char *name) const;

		list<string> GetPropertyTypes() const;
		void RegisterType(const char *name);

	private:
		void LoadTemplates();
		void LoadInstruments();
		void LoadScores();

	public:
		EntityStorage<CSoundScore> *scores;
		EntityStorage<CSoundComponent> *components;
		EntityStorage<CSoundInstrument> *instruments;
		EntityStorage<CSoundInstrumentBlock> *blocks;

	private:
		unsigned int controlSamples;
		unordered_map<string, char> propertyTypes;
};