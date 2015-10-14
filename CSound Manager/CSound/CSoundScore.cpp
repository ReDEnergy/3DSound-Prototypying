#include "CSoundScore.h"

#include <sstream>

#include <include/pugixml.h>
#include <include/utils.h>

#include <CSound/CSoundInstrument.h>

CSoundScore::CSoundScore()
{
	SetName("Sound Source");
	Init();
}

CSoundScore::CSoundScore(const CSoundScore & score)
{
	SetName(score.GetName());
	render.reserve(1000);

	Init();
	PreventUpdate();
	for (auto I : score.entries) {
		Add(new CSoundInstrument(*I));
	}
	ResumeUpdate();
	Update();
	Save();
}

CSoundScore::~CSoundScore() {
}

void CSoundScore::Init()
{
	_valueCsOptions = string(
		"\n+rtaudio=alsa -o dac" \
		"\n-B 64" \
		"\n-b 2048\n\t");

	_valueInstrumentOptions = string(
		"\nksmps = 8 \
		\nnchnls = 2\n");

	doc = nullptr;
}

void CSoundScore::Update()
{
	SAFE_FREE(doc);
	doc = new pugi::xml_document();

	pugi::xml_node CsSynthesizer = doc->append_child("CsoundSynthesizer");

	// ----------------------
	// Generate CsOptions
	pugi::xml_node CsOptions = CsSynthesizer.append_child("CsOptions");

	CsOptions.append_child(pugi::node_pcdata).set_value(_valueCsOptions.c_str());

	// ----------------------
	// Generate CsInstruments
	pugi::xml_node CsInstruments = CsSynthesizer.append_child("CsInstruments");

	CsInstruments.append_child(pugi::node_pcdata).set_value(_valueInstrumentOptions.c_str());

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
	doc->save(ss);

	render = ss.str();

#ifdef CSOUND_DEBUG
	cout << "Render => " << endl;
	cout << render << endl << endl;
#endif
}

void CSoundScore::Save()
{
	if (doc == nullptr)
		Update();

	// ----------------------
	// Save File
	string filename = "Resources//" + name + ".csd";
	doc->save_file(filename.c_str());
}