#include "CSoundSerialize.h"

#include <include/pugixml.h>
#include <include/utils.h>

#include <functional>
#include <thread>
#include <unordered_map>

#include <CSoundComponent.h>
#include <CSoundComponentProperty.h>
#include <CSoundInstrument.h>
#include <CSoundSynthesizer.h>

using namespace pugi;
using namespace std;

unordered_map<uint, CSoundSynthesizer*> CSoundSerialize::Load(const char * file)
{
	unordered_map<uint, CSoundInstrument*> instruments;
	unordered_map<uint, CSoundComponent*> components;
	unordered_map<uint, CSoundSynthesizer*> scores;

	///////////////////////////////////////////////////////////////////////////
	// Open Document
	auto doc = LoadXML(file);

	// First we need Components for building the Instruments
	// Afterwards the Instruments for creating the Scores

	///////////////////////////////////////////////////////////////////////////
	// Read Components

	xml_node nodeComponents = doc->child("components");

	for (auto &component : nodeComponents.children())
	{
		auto name = component.child_value("name");
		auto UID = component.child("uid").text().as_uint();

		auto C = new CSoundComponent();
		C->SetName(name);
		components[UID] = C;

		for (auto &property : component.child("content"))
		{
			auto type = property.child_value("type");
			auto def = property.child_value("default");
			auto value = property.child("value");

			CSoundComponentProperty *CP = new CSoundComponentProperty(C);
			CP->SetName(type);
			CP->SetDefaultValue(def);
			if (value) {
				CP->SetValue(value.text().get());
			}
			C->Add(CP);
		}
	}

	///////////////////////////////////////////////////////////////////////////
	// Read Instruments

	xml_node nodeInstruments = doc->child("instruments");

	for (auto &instrument : nodeInstruments)
	{
		auto name = instrument.child_value("name");
		auto UID = instrument.child("uid").text().as_uint();

		auto I = new CSoundInstrument();
		I->SetName(name);
		instruments[UID] = I;

		for (auto &componentID : instrument.child("components"))
		{
			auto ID = componentID.text().as_uint();
			if (components.find(ID) != components.end()) {
				I->Add(components[ID]);
			}
		}
	}

	///////////////////////////////////////////////////////////////////////////
	// Read Scores

	xml_node nodeScores = doc->child("scores");

	for (auto &score : nodeScores)
	{
		auto name = score.child_value("name");
		auto UID = score.child("uid").text().as_uint();

		auto S = new CSoundSynthesizer();
		S->SetName(name);
		S->SetUID(UID);
		scores[UID] = S;

		for (auto &instrumentID : score.child("instruments"))
		{
			auto ID = instrumentID.text().as_uint();
			if (instruments.find(ID) != instruments.end()) {
				S->Add(instruments[ID]);
			}
		}

	}

	///////////////////////////////////////////////////////////////////////////
	// Close Document and Return

	SAFE_FREE(doc);
	return scores;
}

pugi::xml_document * CSoundSerialize::GetXML(const vector<CSoundSynthesizer*>& scoreList)
{
	unordered_map<uint, CSoundInstrument*> instruments;
	unordered_map<uint, CSoundComponent*> components;
	unordered_map<uint, CSoundSynthesizer*> scores;

	///////////////////////////////////////////////////////////////////////////
	// Find Unique Elements

	// unique scores
	for (auto &score : scoreList) {
		scores[score->GetUID()] = score;
	}

	// unique instruments
	for (auto &score : scores) {
		for (auto &instr : score.second->GetEntries()) {
			instruments[instr->GetUID()] = instr;
		}
	}

	// unique components
	for (auto &instr : instruments) {
		for (auto &comp : instr.second->GetEntries()) {
			components[comp->GetUID()] = comp;
		}
	}

	///////////////////////////////////////////////////////////////////////////
	// Create Document
	xml_document *doc = new xml_document();

	///////////////////////////////////////////////////////////////////////////
	// Save Scores

	xml_node nodeScores = doc->append_child("scores");

	for (auto &entry : scores)
	{
		auto score = entry.second;
		auto scoreNode = nodeScores.append_child("score");
		CreateNode("name", score->GetName(), scoreNode);
		CreateNode("uid", score->GetUID(), scoreNode);

		auto instrumentsNode = scoreNode.append_child("instruments");

		for (auto instr : score->GetEntries())
		{
			CreateNode("instrumentID", instr->GetUID(), instrumentsNode);
		}
	}

	///////////////////////////////////////////////////////////////////////////
	// Save Instruments
	xml_node nodeInstruments = doc->append_child("instruments");

	for (auto &instr : instruments)
	{
		auto instrument = instr.second;
		auto instrumentNode = nodeInstruments.append_child("instrument");
		CreateNode("name", instrument->GetName(), instrumentNode);
		CreateNode("uid", instrument->GetUID(), instrumentNode);

		auto componentsNode = instrumentNode.append_child("components");

		for (auto instr : instrument->GetEntries())
		{
			CreateNode("componentID", instr->GetUID(), componentsNode);
		}
	}

	///////////////////////////////////////////////////////////////////////////
	// Save Components

	xml_node nodeComponents = doc->append_child("components");

	for (auto &comp : components)
	{
		auto C = comp.second;
		auto compNode = nodeComponents.append_child("component");
		CreateNode("name", C->GetName(), compNode);
		CreateNode("uid", C->GetUID(), compNode);

		auto componentsNode = compNode.append_child("content");

		///////////////////////////////////////////////////////////////////////
		// Write Properties
		for (auto prop : C->GetEntries())
		{
			auto propNode = componentsNode.append_child("property");
			CreateNode("type", prop->GetName(), propNode);
			CreateNode("default", prop->GetDefaultValue(), propNode);
			if (prop->IsValueSet()) {
				CreateNode("value", prop->GetValue(), propNode);
			}
		}
	}

	///////////////////////////////////////////////////////////////////////////
	// Return Document
	return doc;
}
