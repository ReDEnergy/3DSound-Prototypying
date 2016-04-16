#include "CSoundInstrumentBlock.h"

#include <CSoundInstrument.h>
#include <CSoundComponent.h>

using namespace std;

CSoundInstrumentBlock::CSoundInstrumentBlock()
{
	SetName("Instrument Block");
	render.reserve(1000);
}

CSoundInstrumentBlock::~CSoundInstrumentBlock() {
}

void CSoundInstrumentBlock::Update()
{
	for (auto E : entries) {
		for (auto chn : E->GetControlChannels()) {
			channels.push_back(chn);
		}
	}

	render.clear();

	for (auto &chn : channels) {
		render.append("\n\t" + chn + " chnget \"" + chn + "\"");
	}

	render.append("\n");

	for (auto C : entries) {
		render.append("\n\t");
		render.append(C->GetRender());
	}

#ifdef CSOUND_DEBUG
	cout << "Render => " << endl;
	cout << render << endl << endl;
#endif
}

void CSoundInstrumentBlock::AddControlChannel(const char* channel)
{
	channels.push_back(channel);
}

const vector<string>& CSoundInstrumentBlock::GetControlChannels() const
{
	return channels;
}