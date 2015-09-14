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
		static CSoundInstrumentBlock* GetGlobalOutputModel();

	private:
		static CSoundManager* csManager;
		static CSoundInstrumentBlock *globalOutputModel;
};