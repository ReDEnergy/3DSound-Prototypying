#pragma once

#include <include/dll_export.h>
#include <string>

class DLLExport CSoundEvent
{
	public:
		CSoundEvent(char type, const char* instrumentID, std::string parameters);
		~CSoundEvent();

		std::string GetRender() const;
		std::string GetRender(unsigned int instrumentIndex) const;
		const char* GetInstrumentID() const;

	private:
		char type;
		std::string instrumentID;
		std::string parameters;
};
