#pragma once
#include <string>
#include <vector>
#include <unordered_map>

#include <CSound/CSoundForward.h>
#include <CSound/CSoundEntity.h>
#include <CSound/Templates/CSoundList.h>

class DLLExport CSoundInstrument
	: public CSoundEntity
	, public CSoundList<CSoundComponent, CSoundInstrument>
{
	public:
		CSoundInstrument();
		CSoundInstrument(const CSoundInstrument &instr);
		~CSoundInstrument();

		void Update();

		void InitControlChannels(Csound * score);
		std::vector<std::string> GetControlChannels() const;

		void SetChannelValue(const char * channelName, float value);
		void SetParent(CSoundSynthesizer *score);

		void UseGlobalOutput(bool value);

	private:
		void Init();

	private:
		bool useGlobalOutput;
		unsigned int duration;
		CSoundSynthesizer *parent;
		std::unordered_map<std::string, MYFLT*> channels;
};
