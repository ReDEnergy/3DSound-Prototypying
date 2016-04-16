#include "CSoundPlayer.h"

#include <iostream>

#include <include/utils.h>
#include <include/csound.h>

#include <CSound/CSoundSynthesizer.h>
#include <CSound/CSoundInstrument.h>

CSoundPlayer::CSoundPlayer(CSoundSynthesizer * score)
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

void CSoundPlayer::SetPlaybackTime(float time)
{
	if (perfThread)
		perfThread->SetScoreOffsetSeconds(time);
}

void CSoundPlayer::SendEvent(char eventType, int nrParams, double* params) const
{
	perfThread->ScoreEvent(0, eventType, nrParams, params);
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