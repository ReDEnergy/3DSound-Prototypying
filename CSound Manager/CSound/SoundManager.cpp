#include "SoundManager.h"

#include <include/singleton.h>

#include <CsoundManager.h>
#include <CSoundComponent.h>
#include <EntityStorage.h>

using namespace std;

CSoundManager* SoundManager::csManager = nullptr;
CSoundInstrumentBlock* SoundManager::globalOutputModel = nullptr;
uint SoundManager::globalOutputModelIndex = 0;
vector<string> SoundManager::standardChannels;
vector<string> SoundManager::output8Channels;
vector<string> SoundManager::output4Channels;

SoundManager::SoundManager()
{
}

void SoundManager::Init()
{
	CsoundManagerNS::Version::LogInfo();

	standardChannels.push_back("Front Left");
	standardChannels.push_back("Front Right");
	standardChannels.push_back("Center");
	standardChannels.push_back("Subwoofer");
	standardChannels.push_back("Rear Left");
	standardChannels.push_back("Rear Right");
	standardChannels.push_back("Side Left");
	standardChannels.push_back("Side Right");

	output4Channels.push_back("afrontleft");
	output4Channels.push_back("afrontright");
	output4Channels.push_back("acenter");
	output4Channels.push_back("asubwoofer");
	output4Channels.push_back("arearleft");
	output4Channels.push_back("arearright");
	output4Channels.push_back("asideleft");
	output4Channels.push_back("asideright");

	output8Channels.push_back("aLeftFrontUp");
	output8Channels.push_back("aLeftFrontDown");
	output8Channels.push_back("aLeftBackUp");
	output8Channels.push_back("aLeftBackDown");
	output8Channels.push_back("aRightFrontUp");
	output8Channels.push_back("aRightFrontDown");
	output8Channels.push_back("aRightBackUp");
	output8Channels.push_back("aRightBackDown");

	csManager = Singleton<CSoundManager>::Instance();
	csManager->LoadConfig();
}

CSoundManager* SoundManager::GetCSManager()
{
	return csManager;
}

void SoundManager::SetGlobalOutputModel(const char * model)
{
	if (model)
		globalOutputModel = csManager->blocks->Get(model);
	else
		globalOutputModel = nullptr;
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

	vector<string> *channelNames = &output4Channels;
	if (channelCount == 8) channelNames = &output8Channels;

	for (int i = 0; i < limit; i++)
	{
		*buff += to_string(device->mapping[i] + 1) + ", " + (*channelNames)[i] + ", ";
	}
	*buff += to_string(device->mapping[channelCount - 1] + 1) + ", " + (*channelNames)[channelCount - 1];

	return buff->c_str();
}