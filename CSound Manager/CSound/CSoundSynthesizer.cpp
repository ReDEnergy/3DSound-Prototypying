#include "CSoundSynthesizer.h"

#include <sstream>
#include <iostream>

#include <include/pugixml.h>
#include <include/utils.h>

#include <CSound/CSoundEvent.h>
#include <CSound/CSoundInstrument.h>
#include <CSound/CSoundManager.h>
#include <CSound/SoundManager.h>

CSoundSynthesizer::CSoundSynthesizer()
{
	useSourceCode = false;
	SetName("Sound Source");
	Init();
}

CSoundSynthesizer::CSoundSynthesizer(const CSoundSynthesizer & score)
{
	useSourceCode = false;
	SetName(score.GetName());
	instrumentEvents = score.instrumentEvents;

	Init();
	PreventUpdate();
	for (auto I : score.entries) {
		Add(new CSoundInstrument(*I));
	}
	ResumeUpdate();
	Update();
	SaveToFile();
}

CSoundSynthesizer::~CSoundSynthesizer() {
}

void CSoundSynthesizer::Init()
{
	doc = nullptr;
}

void CSoundSynthesizer::Update()
{
	auto CsManager = SoundManager::GetCSManager();

	SAFE_FREE(doc);
	doc = new pugi::xml_document();

	pugi::xml_node CsSynthesizer = doc->append_child("CsoundSynthesizer");

	// ----------------------
	// Generate CsOptions
	pugi::xml_node CsOptions = CsSynthesizer.append_child("CsOptions");

	CsOptions.append_child(pugi::node_pcdata).set_value(CsManager->GetCsOptionsRender());

	// ----------------------
	// Generate CsInstruments
	pugi::xml_node CsInstruments = CsSynthesizer.append_child("CsInstruments");
	CsInstruments.append_child(pugi::node_pcdata).set_value(CsManager->GetInstrumentOptionsRender());

	// ----------------------
	// Generate CsScore
	pugi::xml_node CsScore = CsSynthesizer.append_child("CsScore");
	CsScore.append_child(pugi::node_pcdata).set_value("\n");

	// Attach Standard Events
	for (auto &event : standardEvents) {
		CsScore.append_child(pugi::node_pcdata).set_value(event->GetRender().c_str());
	}

	// Fill data
	uint instrumentID = 1;
	for (auto I : entries) {
		string instr = "\ninstr " + to_string(instrumentID) + "\n";
		instr.append(I->GetRender());
		instr.append("\nendin");
		instr.append("\n\n");

		auto eventsIt = instrumentEvents.find(I->GetName());
		if (eventsIt != instrumentEvents.end()) {
			for (auto &event : eventsIt->second) {
				CsScore.append_child(pugi::node_pcdata).set_value(event->GetRender(instrumentID).c_str());
			}
		}

		CsInstruments.append_child(pugi::node_pcdata).set_value(instr.c_str());
		instrumentID++;
	}

	CsInstruments.append_child(pugi::node_pcdata).set_value("\t");
	CsScore.append_child(pugi::node_pcdata).set_value("\t");

	// Save rendering
	std::stringstream ss;
	doc->save(ss, "\t", pugi::format_no_escapes);
	render = ss.str();
}

void CSoundSynthesizer::SaveToFile()
{
	string filename = "Resources//CSound//Scores//" + name + ".csd";
	
	if (useSourceCode) {
		auto F = fopen(filename.c_str(), "w");
		fprintf(F, "%s", sourceCode.c_str());
		fclose(F);
	}
	else {
		doc->save_file(filename.c_str(), "\t", pugi::format_no_escapes);
	}
}

void CSoundSynthesizer::Rebuild()
{
	for (auto instrument : entries) {
		instrument->Update();
	}
	Update();
	SaveToFile();
}

bool CSoundSynthesizer::HasSourceCode() const
{
	return false;
}

const string& CSoundSynthesizer::GetSourceCode() const
{
	return sourceCode;
}

void CSoundSynthesizer::SetSourceCode(string& code)
{
	if (code.length() == 0) {
		if (useSourceCode) {
			useSourceCode = false;
			sourceCode = "";
			SaveToFile();
		}
	}
	else {
		useSourceCode = true;
		sourceCode = std::move(code);
		SaveToFile();
	}
}

void CSoundSynthesizer::AddEvent(CSoundEvent* ev)
{
	auto entry = GetEntry(ev->GetInstrumentID());
	if (entry) {
		instrumentEvents[ev->GetInstrumentID()].push_back(ev);
	}
	else {
		standardEvents.push_back(ev);
	}
}