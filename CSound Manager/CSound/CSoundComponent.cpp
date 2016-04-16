#include "CSoundComponent.h"

#include <CSound/CSoundComponentProperty.h>
#include <CSound/CSoundInstrument.h>
#include <CSound/CSoundManager.h>
#include <CSound/SoundManager.h>

using namespace std;

CSoundComponent::CSoundComponent()
{
	parent = nullptr;
	SetName("New Component");
}

CSoundComponent::CSoundComponent(const CSoundComponent &Comp)
{
	parent = nullptr;
	SetName(Comp.GetName());

	PreventUpdate();
	for (auto P : Comp.entries) {
		Add(new CSoundComponentProperty(*P));
	}

	controls = Comp.controls;
	ResumeUpdate();

	InitControlChannels();
	Update();
}

CSoundComponent::~CSoundComponent()
{
}

void CSoundComponent::Update()
{
	InitControlChannels();
	render.clear();

	const char* lastype = nullptr;
	const char *value = nullptr;

	for (auto &prop : entries)
	{
		if (lastype)
			render.append(strcmp(lastype, prop->GetName()) ? " " : ", ");
		render.append(prop->GetValue());
		lastype = prop->GetName();
	}

#ifdef CSOUND_DEBUG
	cout << "Render => " << endl;
	cout << render << endl << endl;
#endif

	if (parent)
		parent->Update();
}

void CSoundComponent::InitControlChannels()
{
	controlChannels.clear();
	for (auto &prop : entries)
	{
		if (strcmp(prop->GetName(), "control") == 0) {
			controlChannels.push_back(prop->GetDefaultValue());
		}
	}

	for (auto &chn : controls) {
		controlChannels.push_back(chn);
	}
}

void CSoundComponent::AddControlChannel(const char * channel)
{
	controls.push_back(channel);
}

const vector<string>& CSoundComponent::GetControlChannels() const
{
	return controlChannels;
}

void CSoundComponent::SetParent(CSoundInstrument * instrument)
{
	parent = instrument;
}
