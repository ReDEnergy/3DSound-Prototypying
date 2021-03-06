﻿#include "CSoundManager.h"

#include <iostream>
#include <string>
#include <sstream>

#ifdef _WIN32
#include <Windows.h>
#endif

#include <include/pugixml.h>
#include <include/utils.h>
#include <include/math.h>
#include <stdlib.h>

#include <CSound/EntityStorage.h>
#include <CSound/CSoundComponent.h>
#include <CSound/CSoundInstrument.h>
#include <CSound/CSoundInstrumentBlock.h>
#include <CSound/CSoundSynthesizer.h>
#include <CSound/CSoundEvent.h>
#include <CSound/CSoundConfig.h>

#include <include/csound.h>

using namespace std;

CSoundManager::CSoundManager()
{
	// Create Generated Scores directory if does not exist
	#ifdef _WIN32
	CreateDirectory(CsoundManagerNS::ResourcePath::GENERATED_CSD.c_str(), NULL);
	CreateDirectory(CsoundManagerNS::ResourcePath::CSD_DATABASE.c_str(), NULL);
	#endif

	activeDacID = -1;

	scores		= new EntityStorage<CSoundSynthesizer>("score ");
	instruments	= new EntityStorage<CSoundInstrument>("instrument ");
	components	= new EntityStorage<CSoundComponent>("component ");
	blocks		= new EntityStorage<CSoundInstrumentBlock>("block ");

#ifdef _x86_64

#endif

	#if _WIN32 || _WIN64
		#ifdef _WIN64
			_putenv(("Path=.\\" + CsoundManagerNS::ResourcePath::PLUGINS_WIN_X64).c_str());
			csoundSetGlobalEnv("OPCODE6DIR64", (CsoundManagerNS::ResourcePath::PLUGINS_WIN_X64).c_str());
		#else
			_putenv(("Path=.\\" + CsoundManagerNS::ResourcePath::PLUGINS_WIN_X86).c_str());
			csoundSetGlobalEnv("OPCODE6DIR64", (CsoundManagerNS::ResourcePath::PLUGINS_WIN_X86).c_str());
		#endif
	#elif __linux__
		#ifdef __arm__
			putenv(const_cast<char*>(("Path=./" + CsoundManagerNS::ResourcePath::PLUGINS_ARM_X64).c_str()));
			csoundSetGlobalEnv("OPCODE6DIR64", (CsoundManagerNS::ResourcePath::PLUGINS_ARM_X64).c_str());
		#else
			putenv(const_cast<char*>(("Path=./" + CsoundManagerNS::ResourcePath::PLUGINS_LINUX_X64).c_str()));
				csoundSetGlobalEnv("OPCODE6DIR64", (CsoundManagerNS::ResourcePath::PLUGINS_LINUX_X64).c_str());
		#endif
	#endif

	//auto path = getenv("Path");
	//cout << "PATH ENV: " << path << endl << endl;

	auto csound = csoundCreate(NULL);

	#ifdef _WIN32
		string audioModule = "Portaudio";
	#elif __linux__
		string audioModule = "alsa";
	#endif

	csoundSetRTAudioModule(csound, const_cast<char*>(audioModule.c_str()));

	// Get output devices
	int nrDevices = csoundGetAudioDevList(csound, NULL, 1);
	if (nrDevices) {
		CS_AUDIODEVICE *devices = new CS_AUDIODEVICE[nrDevices];
		csoundGetAudioDevList(csound, devices, 1);
		for (int i = 0; i < nrDevices; i++) {
			auto AD = new AudioDevice();
			memcpy(AD, &devices[i], sizeof(CS_AUDIODEVICE));
			AD->index = i;
			AD->nrActiveChannels = AD->supportedChannels;
			outputDevices.push_back(AD);
		}
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
	configXML = pugi::LoadXML(CsoundManagerNS::ResourceFile::SYNTHESIZER_OPTIONS.c_str());

	auto config = configXML->child("config");

	// General
	#ifdef _WIN32
	auto configOS = config.child("Windows");
	#elif __linux__
	auto configOS = config.child("Linux");
	#endif

	for (auto option : configOS.child("InstrumentOptions"))
	{
		CsoundInstrumentOption opt;
		opt.name = option.name();
		if (option.attribute("name"))
			opt.name = option.attribute("name").as_string();
		opt.value = option.text().as_uint();

		instrumentOptions[option.name()] = opt;
	}

	// Get CsOptions
	for (auto option : configOS.child("CsOptions"))
	{
		csOptions[option.name()] = option.text().get();
	}

	// Get General
	auto general = configOS.child("general");
	auto dacID = general.child("dac").text().as_uint();

	if (outputDevices.size() == 0)
	{
		activeDacID = -1;
		return;
	}
	
	if (dacID >= outputDevices.size())
	{
		dacID = 0;
	}

	outputDevices[dacID]->nrActiveChannels = MIN(general.child("channels").text().as_uint(), outputDevices[dacID]->supportedChannels);

	auto devices = general.child("devices");
	for (auto deviceNode : devices) {
		auto ID = deviceNode.attribute("id").as_uint();
		if (ID < outputDevices.size()) {
			auto device = outputDevices[ID];
			auto mapping = deviceNode.child_value("mapping");
			if (mapping) {
				istringstream iss(mapping);
				for (uint i = 0; i < device->supportedChannels; i++) {
					iss >> device->mapping[i];
				}
			}
			device->changed = true;
		}
	}

	SetActiveDac(dacID);
	SetCsInstrumentOption("nchnls", outputDevices.size() ? outputDevices[activeDacID]->supportedChannels : 2);
	RenderCsOptions();
	RenderInstrumentOptions();
}

void CSoundManager::LoadTemplates()
{
	pugi::xml_document *doc = pugi::LoadXML(CsoundManagerNS::ResourceFile::TEMPLATES_CONFIG_XML.c_str());

	for (auto &component : doc->child("components").children()) {
		CSoundComponent * T = new CSoundComponent();

		T->SetName(component.child_value("name"));
		T->PreventUpdate();

		for (auto &control : component.child("controls")) {
			const char* channelName = control.text().get();
			T->AddControlChannel(channelName);
		}

		auto content = component.child("content");
		for (auto &entry : content.children()) {

			const char* tag = entry.name();
			const char* value = entry.text().get();

			CSoundComponentProperty *CP = new CSoundComponentProperty(T);
			CP->SetName(tag);
			CP->SetDefaultValue(value);
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
	pugi::xml_document *doc = pugi::LoadXML(CsoundManagerNS::ResourceFile::INSTRUMENTS_CONFIG_XML.c_str());

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
			auto C = new CSoundComponent(*T);
			I->Add(C);
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
			auto C = new CSoundComponent(*T);
			I->Add(C);
		}

		I->ResumeUpdate();
		I->Update();
		blocks->Set(name, I);
	}

	SAFE_FREE(doc);
}

void CSoundManager::LoadScores()
{
	pugi::xml_document *doc = pugi::LoadXML(CsoundManagerNS::ResourceFile::SCORES_CONFIG_XML.c_str());

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

		for (auto &event : node.child("events")) {
			auto type = event.attribute("type").as_string();
			auto id = event.attribute("id").as_string();
			auto E = new CSoundEvent(type[0], id, event.text().get());
			score->AddEvent(E);
		}

		score->ResumeUpdate();
		score->Update();
		score->SaveToFile();
	}

	SAFE_FREE(doc);
}

void CSoundManager::SaveConfigFile()
{
	auto config = configXML->child("config");

	#ifdef _WIN32
	auto configOS = config.child("Windows");
	#elif __linux__
	auto configOS = config.child("Linux");
	#endif

	// ------------------------------------------
	// Instrument Options

	auto InstrOptions = configOS.child("InstrumentOptions");
	configOS.remove_child(InstrOptions);
	InstrOptions = configOS.append_child("InstrumentOptions");
	for (auto &option : instrumentOptions)
	{
		auto node = InstrOptions.append_child(option.first.c_str());
		if (option.first.compare(option.second.name)) {
			auto att = node.append_attribute("name");
			att.set_value(option.second.name.c_str());
		}
		node.append_child(pugi::node_pcdata).set_value(to_string(option.second.value).c_str());
	}

	// ------------------------------------------
	// CsOptions

	auto CsOptions = configOS.child("CsOptions");
	configOS.remove_child(CsOptions);
	CsOptions = configOS.append_child("CsOptions");
	for (auto &option : csOptions)
	{
		auto node = CsOptions.append_child(option.first.c_str());
		node.append_child(pugi::node_pcdata).set_value(option.second.c_str());
	}

	// ------------------------------------------
	// Device options

	uint deviceID = 0;

	auto generalNode = configOS.child("general");

	auto activeDacNode = generalNode.child("dac");
	activeDacNode.text().set(activeDacID);

	auto nrActiveChnNode = generalNode.child("channels");
	nrActiveChnNode.text().set(outputDevices.size() ? outputDevices[activeDacID]->nrActiveChannels : 2);

	auto devicesNode = generalNode.child("devices");
	generalNode.remove_child("devices");
	devicesNode = generalNode.append_child("devices");

	for (auto device : outputDevices)
	{
		if (device->changed)
		{
			auto deviceNode = devicesNode.append_child("dac");
			auto att = deviceNode.append_attribute("id");
			att.set_value(deviceID);

			string buffer;
			auto mapping = deviceNode.append_child("mapping");

			for (uint i = 0; i < device->supportedChannels; i++) {
				buffer += to_string(device->mapping[i]) + " ";
			}
			mapping.append_child(pugi::node_pcdata).set_value(buffer.c_str());
		}
		deviceID++;
	}

	configXML->save_file(CsoundManagerNS::ResourceFile::SYNTHESIZER_OPTIONS.c_str(), "\t", pugi::format_default);
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
		sprintf(buff, "\n%s = %u", option.second.name.c_str(), option.second.value);
		csInstrOptionsRender += buff;
	}
	csInstrOptionsRender += "\n";
}

CSoundSynthesizer * CSoundManager::CreateScore()
{
	auto name = scores->Create();
	auto S = scores->Get(name.c_str());
	S->SetName(name.c_str());
	S->Update();
	S->SaveToFile();
	return S;
}

CSoundSynthesizer * CSoundManager::GetScore(const char * name) const
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

AudioDevice * CSoundManager::GetActiveDac() const
{
	return outputDevices[activeDacID];
}

int CSoundManager::GetActiveDacID() const
{
	return activeDacID;
}

const char * CSoundManager::GetCsOptionsRender() const
{
	return csOptionsRender.c_str();
}

void CSoundManager::SetActiveDac(uint dacID)
{
	if (outputDevices.size() == 0 || dacID == activeDacID)
		return;

	activeDacID = dacID;
	string outValue("-o ");
	outValue += outputDevices[dacID]->deviceID;
	SetCsOptionsParameter("dac", outValue.c_str());
}

void CSoundManager::SetCsOptionsParameter(const char * property, const char * value)
{
	auto item = csOptions.find(property);
	if (item != csOptions.end()) {
		int cmp = (*item).second.compare(value);
		if (cmp == 0)
			return;
		(*item).second = value;
	}
	RenderCsOptions();
	SaveConfigFile();
}

void CSoundManager::SetCsInstrumentOption(const char * property, unsigned int value)
{
	auto item = instrumentOptions.find(property);
	if (item != instrumentOptions.end()) {
		if ((*item).second.value == value)
			return;
		(*item).second.value = value;
	}
	RenderInstrumentOptions();
	SaveConfigFile();
}

const char * CSoundManager::GetInstrumentOptionsRender() const
{
	return csInstrOptionsRender.c_str();
}

unsigned int CSoundManager::GetInstrumentOption(const char *propertyName) const
{
	auto item = instrumentOptions.find(propertyName);
	if (item != instrumentOptions.end()) {
			return (*item).second.value;
	}
	return -1;
}

AudioDevice::AudioDevice()
{
	changed = false;
	for (auto i = 0; i < 8; i++) {
		mapping[i] = i;
	}
}
