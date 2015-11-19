#pragma once
#include <include/dll_export.h>

#include <CSound/CSoundForward.h>

class DLLExport SoundManager
{
	private:
		SoundManager() {};
		~SoundManager() {};

	public:

		static void Init();
		static CSoundManager* GetCSManager();

		static void SetGlobalOutputModel(const char* model);
		static void SetGlobalOutputModelIndex(unsigned int index);

		static CSoundInstrumentBlock* GetGlobalOutputModel();
		static unsigned int GetGlobalOutputModelIndex();

	private:
		static CSoundManager* csManager;
		static CSoundInstrumentBlock *globalOutputModel;
		static uint globalOutputModelIndex;
};