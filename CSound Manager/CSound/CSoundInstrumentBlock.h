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
		void AddControlChannel(const char* channel);
		const std::vector<std::string>& GetControlChannels() const;

	private:
		std::vector<std::string> channels;
};
