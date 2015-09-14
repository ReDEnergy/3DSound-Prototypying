#include "CSoundManager.h"

#include <include/pugixml.h>

#include <CSound/EntityStorage.h>
#include <CSound/CSoundComponent.h>
#include <CSound/CSoundInstrument.h>
#include <CSound/CSoundInstrumentBlock.h>
#include <CSound/CSoundScore.h>

CSoundManager::CSoundManager()
{
	scores		= new EntityStorage<CSoundScore>("score ");
	instruments	= new EntityStorage<CSoundInstrument>("instrument ");
	components	= new EntityStorage<CSoundComponent>("component ");
	blocks		= new EntityStorage<CSoundInstrumentBlock>("block ");
}

CSoundManager::~CSoundManager() {
}

void CSoundManager::Init()
{
	Clear();
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

void CSoundManager::LoadTemplates()
{
	pugi::xml_document *doc = pugi::LoadXML("Resources//CSound//templates.xml");

	for (auto component : doc->child("components").children()) {
		CSoundComponent * T = new CSoundComponent();
		T->Init(component);
		components->Set(T->GetName(), T);
	}
}

void CSoundManager::LoadInstruments()
{
	pugi::xml_document *doc = pugi::LoadXML("Resources//CSound//instruments.xml");

	for (auto instrument : doc->child("instruments").children()) {
		auto I = new CSoundInstrument();
		const char* name = instrument.child_value("name");
		I->SetName(name);
		I->PreventUpdate();
		for (auto comp : instrument.child("components")) {
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

	for (auto instrument : doc->child("blocks").children()) {
		auto I = new CSoundInstrumentBlock();
		const char* name = instrument.child_value("name");
		I->SetName(name);
		I->PreventUpdate();
		for (auto comp : instrument.child("components")) {
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
}

void CSoundManager::LoadScores()
{
	pugi::xml_document *doc = pugi::LoadXML("Resources//CSound//scores.xml");

	for (auto node : doc->child("scores").children()) {
		const char* name = node.child_value("name");
		auto score = scores->Create(name);
		score->SetName(name);
		score->PreventUpdate();
		for (auto instr : node.child("instruments")) {
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
		score->Save();
	}
}

CSoundScore * CSoundManager::CreateScore()
{
	auto name = scores->Create();
	auto S = scores->Get(name.c_str());
	S->SetName(name.c_str());
	S->Save();
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
