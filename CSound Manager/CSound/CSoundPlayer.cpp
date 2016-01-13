#include "CSoundPlayer.h"

#include <iostream>

#include <include/utils.h>
#include <include/csound.h>

#include <CSound/CSoundScore.h>
#include <CSound/CSoundInstrument.h>

uintptr_t csThread(void *clientData);

CSoundPlayer::CSoundPlayer(CSoundScore * score)
{
	this->score = score;
	csound = nullptr;
	ThreadID = nullptr;
	PERF_STATUS = false;
	perfThread = nullptr;
}

CSoundPlayer::~CSoundPlayer() {
	Clean();
}

bool CSoundPlayer::Reload()
{
	bool state = PERF_STATUS;
	bool rez = Init();
	if (state && rez)
		Play();
	return rez;
}

bool CSoundPlayer::Init()
{
	Clean();
	csound = new Csound();
	string filename = "Resources//CSound//Scores//" + string(score->GetName()) + ".csd";
	int error = csound->Compile((char*)filename.c_str());

	if (error) {
		Clean();
		return false;
	}

	InitControlChannels();
	return true;
}

void CSoundPlayer::Clean()
{
	invalidChannels.clear();
	PERF_STATUS = false;
	//if (ThreadID) {
	//	csoundJoinThread(ThreadID);
	//	ThreadID = nullptr;
	//}
	if (perfThread) {
		perfThread->Stop();
		perfThread->Join();
		SAFE_FREE(perfThread);
	}
	if (csound) {
		csound->Cleanup();
		SAFE_FREE(csound);
	}
}

void CSoundPlayer::Play()
{
	if (csound) {
		PERF_STATUS = true;
		if (!perfThread)
			perfThread = new CsoundPerformanceThread(csound);
		//ThreadID = csoundCreateThread(csThread, this);
		perfThread->Play();
	}
}

bool CSoundPlayer::IsPlaying()
{
	return PERF_STATUS;
}

void CSoundPlayer::Pause()
{
	perfThread->Pause();
}

void CSoundPlayer::Stop()
{
	perfThread->Pause();
	if (PERF_STATUS) {
		PERF_STATUS = false;
		cout << "Sound Score Time: " << csound->GetScoreTime() << endl;
	}
}
	
void CSoundPlayer::InitControlChannels()
{
	if (!csound) return;

	for (auto instr : score->GetEntries()) {
		for (auto &chn : instr->GetControlChannels()) {
			channels[chn];
		}
	}

	for (auto &channel : channels) {
		if (csound->GetChannelPtr(channel.second, channel.first.c_str(), CSOUND_INPUT_CHANNEL | CSOUND_CONTROL_CHANNEL) == 0)
			*channel.second = 1;
	}
}

void CSoundPlayer::SetControl(const char* channelName, float value, bool forceUpdate)
{
	if (!forceUpdate && (!PERF_STATUS || !csound)) return;

	if (channels.find(channelName) != channels.end()) {
		*channels[channelName] = value;
	}
	else {
		if (invalidChannels.find(channelName) != invalidChannels.end()) {
			return;
		}
		else {
			double *value;
			if (csound->GetChannelPtr(value, channelName, CSOUND_INPUT_CHANNEL | CSOUND_CONTROL_CHANNEL) == 0) {
				channels[channelName] = value;
			}
			else {
				invalidChannels[channelName] = 1;
			}
		}
	}
}

//uintptr_t csThread(void *data)
//{	
//	CSoundPlayer* score = (CSoundPlayer*)data;
//
//	score->PERF_STATUS = true;
//
//	// Play sound
//	while ((score->csound->PerformKsmps() == 0)	&& score->PERF_STATUS) {}
//
//	score->PERF_STATUS = false;
//
//	return 0;
//}