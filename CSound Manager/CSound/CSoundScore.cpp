﻿#include "CSoundScore.h"

#include <sstream>
#include <iostream>

#include <include/pugixml.h>
#include <include/utils.h>

#include <CSound/CSoundInstrument.h>
#include <CSound/CSoundManager.h>
#include <CSound/SoundManager.h>

CSoundScore::CSoundScore()
{
	useSourceCode = false;
	SetName("Sound Source");
	Init();
}

CSoundScore::CSoundScore(const CSoundScore & score)
{
	useSourceCode = false;
	SetName(score.GetName());

	Init();
	PreventUpdate();
	for (auto I : score.entries) {
		Add(new CSoundInstrument(*I));
	}
	ResumeUpdate();
	Update();
	SaveToFile();
}

CSoundScore::~CSoundScore() {
}

void CSoundScore::Init()
{
	doc = nullptr;
}

void CSoundScore::Update()
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

	uint instrumentID = 1;
	for (auto I : entries) {
		string instr = "\ninstr " + to_string(instrumentID) + "\n";
		instr.append(I->GetRender());
		instr.append("\nendin");
		instr.append("\n\n");

		CsInstruments.append_child(pugi::node_pcdata).set_value(instr.c_str());
		instrumentID++;
	}

	CsInstruments.append_child(pugi::node_pcdata).set_value("\t");

	// ----------------------
	// Generate CsScore
	pugi::xml_node CsScore = CsSynthesizer.append_child("CsScore");

	CsScore.append_child(pugi::node_pcdata).set_value("\n");

	instrumentID = 1;
	for (auto I : entries) {
		string instr = "i" + to_string(instrumentID) + " 0 " + to_string(I->GetDuration()) + " \n";
		CsScore.append_child(pugi::node_pcdata).set_value(instr.c_str());
		instrumentID++;
	}

	CsScore.append_child(pugi::node_pcdata).set_value("\t");

	// Save rendering
	std::stringstream ss;
	doc->save(ss, "\t", pugi::format_no_escapes);
	render = ss.str();
}

void CSoundScore::SaveToFile()
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

bool CSoundScore::HasSourceCode() const
{
	return false;
}

const string& CSoundScore::GetSourceCode() const
{
	return sourceCode;
}

void CSoundScore::SetSourceCode(string& code)
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
