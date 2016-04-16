#include "CSoundInstrument.h"

#include <iostream>
#include <include/csound.h>

#include <CSound/CSoundComponent.h>
#include <CSound/CSoundInstrumentBlock.h>
#include <CSound/CSoundSynthesizer.h>
#include <CSound/SoundManager.h>

CSoundInstrument::CSoundInstrument()
{
	SetName("New Instrument");
	Init();
}

CSoundInstrument::CSoundInstrument(const CSoundInstrument & instr)
{
	Init();
	SetName(instr.GetName());

	PreventUpdate();
	for (auto C : instr.entries) {
		Add(new CSoundComponent(*C));
	}
	ResumeUpdate();
	Update();
}

void CSoundInstrument::Init()
{
	parent = nullptr;
	useGlobalOutput = true;
}

CSoundInstrument::~CSoundInstrument() {
}

void CSoundInstrument::Update()
{
	channels.clear();

	for (auto E : entries) {
		for (auto chn : E->GetControlChannels()) {
			channels[chn];
		}
	}

	if (useGlobalOutput) {
		auto outputModel = SoundManager::GetGlobalOutputModel();
		if (outputModel) {
			for (auto chn : outputModel->GetControlChannels()) {
				channels[chn];
			}
		}
	}

	render.clear();

	for (auto &C : channels) {
		render.append("\n\t" + C.first + " chnget \"" + C.first + "\"");
	}

	render.append("\n");

	for (auto C : entries) {
		render.append("\n\t");
		render.append(C->GetRender());
	}

	if (useGlobalOutput) {
		auto outputModel = SoundManager::GetGlobalOutputModel();
		if (outputModel) {
			for (auto C : outputModel->GetEntries()) {
				render.append("\n\t");
				render.append(C->GetRender());
			}
		}
	}

	if (parent)
		parent->Update();
}

void CSoundInstrument::InitControlChannels(Csound * score)
{
	for (auto &channel : channels) {
		if (score->GetChannelPtr(channel.second, channel.first.c_str(), CSOUND_INPUT_CHANNEL | CSOUND_CONTROL_CHANNEL) == 0)
			*channel.second = 1;
	}
}

vector<string> CSoundInstrument::GetControlChannels() const
{
	vector<string> chns;
	for (auto &channel : channels) {
		chns.push_back(channel.first);
	}
	return chns;
}

void CSoundInstrument::SetChannelValue(const char *channelName, float value)
{
	if (channels.find(channelName) != channels.end())
		*channels[channelName] = value;
}

void CSoundInstrument::SetParent(CSoundSynthesizer * score)
{
	parent = score;
}

void CSoundInstrument::UseGlobalOutput(bool value)
{
	useGlobalOutput = value;
}
