#include "CSoundPlayer.h"

#include <iostream>

#include <include/utils.h>
#include <include/csound.h>

#include <CSound/CSoundConfig.h>
#include <CSound/CSoundSynthesizer.h>
#include <CSound/CSoundInstrument.h>

using namespace std;

CSoundPlayer::CSoundPlayer()
	: CSoundPlayer(nullptr)
{
}

CSoundPlayer::CSoundPlayer(CSoundSynthesizer * score)
{
	this->score = score;
	csound = nullptr;
	ThreadID = nullptr;
	perfThread = nullptr;
	isPlaying = false;
}

CSoundPlayer::CSoundPlayer(const CSoundPlayer& player)
	: CSoundPlayer(player.score)
{
}

CSoundPlayer::~CSoundPlayer() {
	Clean();
}

bool CSoundPlayer::Reload()
{
	bool state = isPlaying;
	bool rez = LoadCSDFile(synthFile.c_str());
	if (state && rez)
		Play();
	return rez;
}

bool CSoundPlayer::LoadCSD(std::string file, std::string location)
{
	this->csdFile = file;
	synthFile = location + csdFile;
	return LoadCSDFile(synthFile.c_str());
}

bool CSoundPlayer::LoadSynthesizer(CSoundSynthesizer * score, std::string location)
{
	this->score = score;
	synthFile = location + string(score->GetName()) + ".csd";
	return LoadCSDFile(synthFile.c_str());
	return false;
}

bool CSoundPlayer::LoadCSDFromDB(std::string file)
{
	this->csdFile = file;
	synthFile = CsoundManagerNS::ResourcePath::CSD_DATABASE + csdFile;
	return LoadCSDFile(synthFile.c_str());
}

bool CSoundPlayer::LoadCSDFile(const char* file)
{
	Clean();
	csound = new Csound();
	int error = csound->Compile(const_cast<char*>(file));

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

	if (isPlaying)
	{
		isPlaying = false;
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
		if (!perfThread)
		{
			perfThread = new CsoundPerformanceThread(csound);
			cout << "Synthesizer thread started: " << csound->GetScoreTime() << endl;
		}
		perfThread->Play();
		isPlaying = true;
	}
}

bool CSoundPlayer::IsPlaying() const
{
	return isPlaying;
}

void CSoundPlayer::SetPlaybackState(bool value)
{
	value ? Play() : Pause();
}

void CSoundPlayer::Pause()
{
	if (isPlaying)
	{
		isPlaying = false;
		perfThread->Pause();
	}
}

void CSoundPlayer::TogglePause()
{
	isPlaying ? Pause() : Play();
}

void CSoundPlayer::Stop()
{
	Pause();
}

void CSoundPlayer::SetPlaybackTime(float time)
{
	if (isPlaying)
		perfThread->SetScoreOffsetSeconds(time);
}

void CSoundPlayer::GetPlaybackTime() const
{
	csound->GetScoreOffsetSeconds();
}

void CSoundPlayer::SendEvent(char eventType, int nrParams, double* params) const
{
	if (isPlaying)
		perfThread->ScoreEvent(0, eventType, nrParams, params);
}

std::string CSoundPlayer::GetCSDFile(bool fullPath) const
{
	return (fullPath ? synthFile : csdFile);
}

CSoundSynthesizer * CSoundPlayer::GetSynthesizer() const
{
	return score;
}

Csound* CSoundPlayer::GetCsoundHandle() const
{
	return csound;
}

void CSoundPlayer::InitControlChannels()
{
	if (!csound) return;

	if (score) {
		for (auto instr : score->GetEntries()) {
			for (auto &chn : instr->GetControlChannels()) {
				channels[chn];
			}
		}
	}

	for (auto &channel : channels) {
		if (csound->GetChannelPtr(channel.second, channel.first.c_str(), CSOUND_INPUT_CHANNEL | CSOUND_CONTROL_CHANNEL) == 0)
			*channel.second = 1;
	}
}

void CSoundPlayer::SetControl(const char* channelName, float value, bool forceUpdate)
{
	if (!csound)
		return;

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
