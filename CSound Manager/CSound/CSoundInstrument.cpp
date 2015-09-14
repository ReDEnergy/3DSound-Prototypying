#include "CSoundInstrument.h"

#include <include/csound.h>

#include <CSound/CSoundComponent.h>
#include <CSound/CSoundInstrumentBlock.h>
#include <CSound/CSoundScore.h>
#include <CSound/SoundManager.h>

CSoundInstrument::CSoundInstrument()
{
	SetName("New Instrument");
	render.reserve(1000);
	parent = nullptr;
}

CSoundInstrument::CSoundInstrument(const CSoundInstrument & instr)
{
	SetName(instr.GetName());
	render.reserve(1000);
	parent = nullptr;

	PreventUpdate();
	for (auto C : instr.entries) {
		Add(new CSoundComponent(*C));
	}
	ResumeUpdate();
	Update();
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

	auto outputModel = SoundManager::GetGlobalOutputModel();

	if (outputModel) {
		for (auto chn : outputModel->GetControlChannels()) {
			channels[chn];
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

	if (outputModel) {
		for (auto C : outputModel->GetEntries()) {
			render.append("\n\t");
			render.append(C->GetRender());
		}
	}

#ifdef CSOUND_DEBUG
	cout << "Render => " << endl;
	cout << render << endl << endl;
#endif

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

void CSoundInstrument::SetParent(CSoundScore * score)
{
	parent = score;
}