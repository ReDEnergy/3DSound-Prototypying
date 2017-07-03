#pragma once
#include <vector>

#include <CSound/CSoundEntity.h>
#include <CSound/CSoundForward.h>
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

		void Update();
		void InitControlChannels();
		void AddControlChannel(const char* channel);

		const std::vector<std::string>& GetControlChannels() const;

		void SetParent(CSoundInstrument *instrument);
		void SetParent(CSoundInstrumentBlock *instrument) {};

	private:
		CSoundInstrument *parent;
		std::vector<std::string> controlChannels;
		std::vector<std::string> controls;
};
