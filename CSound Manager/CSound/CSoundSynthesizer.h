#pragma once
#include <include/dll_export.h>

#include <CSound/CSoundComponent.h>
#include <CSound/CSoundForward.h>
#include <CSound/CSoundEntity.h>
#include <CSound/Templates/CSoundList.h>

class CSoundEvent;
class CSoundInstrument;

namespace pugi {
	class xml_document;
}

class DLLExport CSoundSynthesizer
	: public CSoundEntity
	, public CSoundList<CSoundInstrument, CSoundSynthesizer>
{
	public:
		CSoundSynthesizer();
		CSoundSynthesizer(const CSoundSynthesizer &score);
		~CSoundSynthesizer();

		void Update();
		void SaveToFile();
		void Rebuild();

		bool HasSourceCode() const;
		const std::string& GetSourceCode() const;
		void SetSourceCode(std::string & code);
		void AddEvent(CSoundEvent* ev);

	private:
		void Init();

		bool useSourceCode;
		// Used for real-time editing of the score file
		// If present it will be used instead of the rendering from the visual editor
		std::string sourceCode;
		std::unordered_map<std::string, std::list<CSoundEvent*>> instrumentEvents;
		std::list<CSoundEvent*> standardEvents;

	private:
		pugi::xml_document *doc;
};