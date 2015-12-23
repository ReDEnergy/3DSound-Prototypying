#include "SoundManager.h"

#include <include/singleton.h>

#include <CsoundManager.h>
#include <CSoundComponent.h>
#include <EntityStorage.h>

CSoundManager* SoundManager::csManager = nullptr;
CSoundInstrumentBlock* SoundManager::globalOutputModel = nullptr;
uint SoundManager::globalOutputModelIndex = 0;
vector<string> SoundManager::standardChannels;
vector<string> SoundManager::output8Channels;

SoundManager::SoundManager()
{
}

void SoundManager::Init()
{
	standardChannels.push_back("Front Left");
	standardChannels.push_back("Front Right");
	standardChannels.push_back("Center");
	standardChannels.push_back("Subwoofer");
	standardChannels.push_back("Rear Left");
	standardChannels.push_back("Rear Right");
	standardChannels.push_back("Side Left");
	standardChannels.push_back("Side Right");

	output8Channels.push_back("afrontleft");
	output8Channels.push_back("afrontright");
	output8Channels.push_back("acenter");
	output8Channels.push_back("asubwoofer");
	output8Channels.push_back("arearleft");
	output8Channels.push_back("arearright");
	output8Channels.push_back("asideleft");
	output8Channels.push_back("asideright");

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

vector<string>& SoundManager::GetDefaultChannelNames()
{
	return standardChannels;
}

const char * SoundManager::GetChannelMapping(AudioDevice * device, unsigned int channelCount)
{
	string *buff = new string();
	buff->reserve(256);
	buff->append("outch ");
	int limit = (channelCount - 1);

	for (int i = 0; i < limit; i++)
	{
		*buff += to_string(device->mapping[i] + 1) + ", " + output8Channels[i] + ", ";
	}
	*buff += to_string(device->mapping[channelCount - 1] + 1) + ", " + output8Channels[channelCount - 1];

	return buff->c_str();
}
