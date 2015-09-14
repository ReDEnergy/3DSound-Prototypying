#pragma once
#include <unordered_map>

#include <include/pugixml.h>

#include <CSound/CSoundEntity.h>
#include <CSound/CSoundForward.h>
#include <CSound/CsoundComponent.h>
#include <CSound/CSoundComponentProperty.h>
#include <CSound/Templates/CSoundList.h>

class DLLExport CSoundComponent
	: public CSoundEntity
	, public CSoundList<CSoundComponentProperty, CSoundComponent>
{
	public:
		CSoundComponent();
		CSoundComponent(const CSoundComponent &Comp);
		~CSoundComponent();

		void Init(const pugi::xml_node & component);

		void Update();
		void InitControlChannels();

		const vector<string>& GetControlChannels() const;

		void SetParent(CSoundInstrument *instrument);
		void SetParent(CSoundInstrumentBlock *instrument) {};

	private:
		void InitProperties();

	private:
		CSoundInstrument *parent;
		vector<string> controlChannels;
};
