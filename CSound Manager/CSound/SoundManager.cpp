#include "SoundManager.h"

#include <include/singleton.h>

#include <CsoundManager.h>
#include <CSoundComponent.h>
#include <EntityStorage.h>

CSoundManager* SoundManager::csManager = nullptr;
CSoundInstrumentBlock* SoundManager::globalOutputModel = nullptr;

void SoundManager::Init()
{
	csManager = Singleton<CSoundManager>::Instance();
	csManager->Init();
}

CSoundManager* SoundManager::GetCSManager()
{
	return csManager;
}

void SoundManager::SetGlobalOutputModel(const char * model)
{
	globalOutputModel = csManager->blocks->Get(model);
}

CSoundInstrumentBlock * SoundManager::GetGlobalOutputModel()
{
	return globalOutputModel;
}
