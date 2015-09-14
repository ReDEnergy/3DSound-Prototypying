#include "CSoundInstrumentBlock.h"

#include <CSoundInstrument.h>
#include <CSoundComponent.h>

CSoundInstrumentBlock::CSoundInstrumentBlock()
{
	SetName("Instrument Block");
	render.reserve(1000);
}

CSoundInstrumentBlock::~CSoundInstrumentBlock() {
}

void CSoundInstrumentBlock::Update()
{
	channels.clear();

	for (auto E : entries) {
		for (auto chn : E->GetControlChannels()) {
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

#ifdef CSOUND_DEBUG
	cout << "Render => " << endl;
	cout << render << endl << endl;
#endif
}

vector<string> CSoundInstrumentBlock::GetControlChannels() const
{
	vector<string> chns;
	for (auto &channel : channels) {
		chns.push_back(channel.first);
	}
	return chns;
}