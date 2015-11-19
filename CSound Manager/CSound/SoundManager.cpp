#include "SoundManager.h"

#include <include/singleton.h>

#include <CsoundManager.h>
#include <CSoundComponent.h>
#include <EntityStorage.h>

CSoundManager* SoundManager::csManager = nullptr;
CSoundInstrumentBlock* SoundManager::globalOutputModel = nullptr;
uint SoundManager::globalOutputModelIndex = 0;

void SoundManager::Init()
{
	csManager = Singleton<CSoundManager>::Instance();
	csManager->LoadConfig();
}

CSoundManager* SoundManager::GetCSManager()
{
	return csManager;
}

void SoundManager::SetGlobalOutputModel(const char * model)
{
	globalOutputModel = csManager->blocks->Get(model);
}

void SoundManager::SetGlobalOutputModelIndex(unsigned int index)
{
	globalOutputModelIndex = index;
}

CSoundInstrumentBlock * SoundManager::GetGlobalOutputModel()
{
	return globalOutputModel;
}

unsigned int SoundManager::GetGlobalOutputModelIndex()
{
	return globalOutputModelIndex;
}
