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
		void SaveToFile();

		bool HasSourceCode() const;
		const string& GetSourceCode() const;
		void SetSourceCode(string & code);

	private:
		void Init();

		bool useSourceCode;
		// Used for real-time editing of the score file
		// If present it will be used instead of the rendering from the visual editor
		string sourceCode;

	private:
		pugi::xml_document *doc;
};