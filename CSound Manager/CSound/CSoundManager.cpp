#include "CSoundManager.h"

#include <include/pugixml.h>
#include <include/utils.h>

#include <CSound/EntityStorage.h>
#include <CSound/CSoundComponent.h>
#include <CSound/CSoundInstrument.h>
#include <CSound/CSoundInstrumentBlock.h>
#include <CSound/CSoundScore.h>

#include <include/csound.h>

CSoundManager::CSoundManager()
{
	scores		= new EntityStorage<CSoundScore>("score ");
	instruments	= new EntityStorage<CSoundInstrument>("instrument ");
	components	= new EntityStorage<CSoundComponent>("component ");
	blocks		= new EntityStorage<CSoundInstrumentBlock>("block ");

	//csoundSetGlobalEnv("OPCODE6DIR64", "Resources/CSound");
	auto csound = csoundCreate(NULL);
	csoundSetRTAudioModule(csound, "Portaudio");

	// Get output devices
	int nrDevices = csoundGetAudioDevList(csound, NULL, 1);
	CS_AUDIODEVICE *devices = new CS_AUDIODEVICE[nrDevices];
	csoundGetAudioDevList(csound, devices, 1);
	for (int i = 0; i < nrDevices; i++) {
		outputDevices.push_back((AudioDevice*)(&(devices[i])));
	}
}

CSoundManager::~CSoundManager() {
}

void CSoundManager::LoadConfig()
{
	Clear();
	LoadScoreConfig();
	LoadTemplates();
	LoadInstruments();
	LoadScores();
}

void CSoundManager::Clear()
{
	components->Clear();
	instruments->Clear();
	scores->Clear();
}

void CSoundManager::LoadScoreConfig()
{
	auto configXML = pugi::LoadXML("Resources//CSound//score-config.xml");

	auto config = configXML->child("config");

	for (auto option : config.child("CsOptions"))
	{
		csOptions[option.name()] = option.text().get();
	}

	for (auto option : config.child("InstrumentOptions"))
	{
		instrumentOptions[option.name()] = option.text().as_uint();
	}

	RenderCsOptions();
	RenderInstrumentOptions();

	SAFE_FREE(configXML);
}

void CSoundManager::LoadTemplates()
{
	pugi::xml_document *doc = pugi::LoadXML("Resources//CSound//templates.xml");

	for (auto &component : doc->child("components").children()) {
		CSoundComponent * T = new CSoundComponent();

		T->SetName(component.child_value("name"));
		T->PreventUpdate();

		for (auto &control : component.child("controls")) {
			const char* channelName = control.text().get();
			T->AddControlChannel(channelName);
		}

		auto &content = component.child("content");
		for (auto &entry : content.children()) {

			const char* tag = entry.name();
			const char* default = entry.text().get();

			CSoundComponentProperty *CP = new CSoundComponentProperty(T);
			CP->SetName(tag);
			CP->SetDefault(default);
			T->Add(CP);

			RegisterType(tag);
		}

		T->ResumeUpdate();
		T->Update();

		components->Set(T->GetName(), T);
	}

	SAFE_FREE(doc);
}

void CSoundManager::LoadInstruments()
{
	pugi::xml_document *doc = pugi::LoadXML("Resources//CSound//instruments.xml");

	for (auto &instrument : doc->child("instruments").children()) {
		auto I = new CSoundInstrument();
		const char* name = instrument.child_value("name");
		I->SetName(name);
		I->PreventUpdate();
		for (auto &comp : instrument.child("components")) {
			const char* cName = comp.text().get();
			auto T = components->Get(cName);
			if (!T) {
				continue;
			}
			auto comp = new CSoundComponent(*T);
			I->Add(comp);
		}

		I->ResumeUpdate();
		I->Update();
		instruments->Set(name, I);
	}

	// ************************
	// Load Instrument Snippets

	for (auto &instrument : doc->child("blocks").children()) {
		auto I = new CSoundInstrumentBlock();
		const char* name = instrument.child_value("name");
		I->SetName(name);
		I->PreventUpdate();

		for (auto &control : instrument.child("controls")) {
			const char* channelName = control.text().get();
			I->AddControlChannel(channelName);
		}

		for (auto &comp : instrument.child("components")) {
			const char* cName = comp.text().get();
			auto T = components->Get(cName);
			if (!T) {
				continue;
			}
			auto comp = new CSoundComponent(*T);
			I->Add(comp);
		}

		I->ResumeUpdate();
		I->Update();
		blocks->Set(name, I);
	}

	SAFE_FREE(doc);
}

void CSoundManager::LoadScores()
{
	pugi::xml_document *doc = pugi::LoadXML("Resources//CSound//scores.xml");

	for (auto &node : doc->child("scores").children()) {
		const char* name = node.child_value("name");
		auto score = scores->Create(name);
		score->SetName(name);
		score->PreventUpdate();
		for (auto &instr : node.child("instruments")) {
			const char* cName = instr.text().get();
			auto I = instruments->Get(cName);
			if (!I) {
				continue;
			}
			auto comp = new CSoundInstrument(*I);
			score->Add(comp);
		}
		score->ResumeUpdate();
		score->Update();
		score->SaveToFile();
	}

	SAFE_FREE(doc);
}

void CSoundManager::RenderCsOptions()
{
	csOptionsRender.clear();

	for (auto &option : csOptions) {
		csOptionsRender += "\n";
		csOptionsRender += (option.second);
	}
	csOptionsRender += "\n";
}

void CSoundManager::RenderInstrumentOptions()
{
	csInstrOptionsRender.clear();

	char buff[128] = {0};
	for (auto &option : instrumentOptions)
	{
		sprintf(buff, "\n%s = %u", option.first.c_str(), option.second);
		csInstrOptionsRender += buff;
	}
	csInstrOptionsRender += "\n";
}

CSoundScore * CSoundManager::CreateScore()
{
	auto name = scores->Create();
	auto S = scores->Get(name.c_str());
	S->SetName(name.c_str());
	S->Update();
	S->SaveToFile();
	return S;
}

CSoundScore * CSoundManager::GetScore(const char * name) const
{
	return scores->Get(name);
}

list<string> CSoundManager::GetPropertyTypes() const
{
	list<string> props;
	for (auto &p : propertyTypes) {
		props.push_back(p.first);
	}
	return props;
}

void CSoundManager::RegisterType(const char * name)
{
	propertyTypes[name] = 0;
}

const vector<AudioDevice*>& CSoundManager::GetOutputDevices() const
{
	return outputDevices;
}

const char * CSoundManager::GetCsOptionsRender() const
{
	return csOptionsRender.c_str();
}

void CSoundManager::SetCsOptionsParameter(const char * property, const char * value)
{
	auto item = csOptions.find(property);
	if (item != csOptions.end()) {
		(*item).second = value;
	}
	RenderCsOptions();
}

void CSoundManager::SetCsInstrumentOption(const char * property, unsigned int value)
{
	auto item = instrumentOptions.find(property);
	if (item != instrumentOptions.end()) {
		(*item).second = value;
	}
	RenderInstrumentOptions();
}

const char * CSoundManager::GetInstrumentOptionsRender() const
{
	return csInstrOptionsRender.c_str();
}
