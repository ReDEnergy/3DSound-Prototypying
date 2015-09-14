#pragma once
#include <unordered_map>

#include <CSound/CSoundForward.h>
#include <CSound/CSoundEntity.h>
#include <CSound/Templates/CSoundList.h>

class DLLExport CSoundInstrumentBlock
	: public CSoundEntity
	, public CSoundList<CSoundComponent, CSoundInstrument>
{
	public:
		CSoundInstrumentBlock();
		~CSoundInstrumentBlock();

		void Update();
		vector<string> GetControlChannels() const;

	private:
		unordered_map<string, bool> channels;
};
