#pragma once
#include <vector>
#include <unordered_map>

#include <include/dll_export.h>
#include <include/pugixml.h>

#include <CSound/CSoundForward.h>

class DLLExport CSoundSerialize
{
	private:
		CSoundSerialize() {};
		~CSoundSerialize() {};

	public:
		static unordered_map<uint, CSoundSynthesizer*> Load(const char* file);
		static pugi::xml_document* GetXML(const vector<CSoundSynthesizer*> &scoreList);
};