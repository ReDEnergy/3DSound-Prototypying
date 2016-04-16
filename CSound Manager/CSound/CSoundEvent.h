#pragma once

#include <include/dll_export.h>
#include <string>

using namespace std;

class DLLExport CSoundEvent
{
	public:
		CSoundEvent(char type, const char* instrumentID, string parameters);
		~CSoundEvent();

		string GetRender() const;
		string GetRender(unsigned int instrumentIndex) const;
		const char* GetInstrumentID() const;

	private:
		char type;
		string instrumentID;
		string parameters;
};
