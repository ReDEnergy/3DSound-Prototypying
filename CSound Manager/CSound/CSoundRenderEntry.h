#pragma once
#include <include/utils.h>

class CSoundRenderEntry
{
	public:
		CSoundRenderEntry(const char* type, uint index)
			: type(type), index(index) {}

	public:
		const char* type;
		uint index;
};