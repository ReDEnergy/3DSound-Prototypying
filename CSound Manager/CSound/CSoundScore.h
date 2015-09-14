#pragma once
#include <include/dll_export.h>

#include <CSound/CSoundForward.h>
#include <CSound/CSoundEntity.h>
#include <CSound/CsoundComponent.h>
#include <CSound/Templates/CSoundList.h>

class CSoundInstrument;
namespace pugi {
	class xml_document;
}

class DLLExport CSoundScore
	: public CSoundEntity
	, public CSoundList<CSoundInstrument, CSoundScore>
{
	public:
		CSoundScore();
		CSoundScore(const CSoundScore &score);
		~CSoundScore();

		void Update();
		void Save();

	private:
		void Init();

	private:
		string _valueCsOptions;
		string _valueInstrumentOptions;
		pugi::xml_document *doc;

	public:
		bool PERF_STATUS;
};