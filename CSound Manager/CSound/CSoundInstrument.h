#pragma once
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
		vector<string> GetControlChannels() const;
		unsigned int GetDuration() const;

		void SetChannelValue(const char * channelName, float value);
		void SetParent(CSoundScore *score);
		void SetDuration(unsigned int duration);

		void UseGlobalOutput(bool value);

	private:
		void Init();

	private:
		bool useGlobalOutput;
		unsigned int duration;
		CSoundScore *parent;
		unordered_map<string, MYFLT*> channels;
};
