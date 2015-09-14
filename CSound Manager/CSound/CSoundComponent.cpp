#include "CSoundComponent.h"

#include <CSound/CSoundComponentProperty.h>
#include <CSound/CSoundInstrument.h>
#include <CSound/CSoundManager.h>
#include <CSound/SoundManager.h>

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
	ResumeUpdate();

	InitControlChannels();
	Update();
}

CSoundComponent::~CSoundComponent()
{
}

void CSoundComponent::Init(const pugi::xml_node &component)
{
	SetName(component.child_value("name"));
	PreventUpdate();

	auto content = component.child("content");
	for (auto &entry : content.children()) {

		const char* tag = entry.name();
		const char* default = entry.text().get();

		CSoundComponentProperty *CP = new CSoundComponentProperty(this);
		CP->SetName(tag);
		CP->SetDefault(default);
		Add(CP);

		SoundManager::GetCSManager()->RegisterType(tag);
	}

	ResumeUpdate();
	Update();
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
}

const vector<string>& CSoundComponent::GetControlChannels() const
{
	return controlChannels;
}

void CSoundComponent::SetParent(CSoundInstrument * instrument)
{
	parent = instrument;
}
