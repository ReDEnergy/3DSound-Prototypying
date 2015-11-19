#pragma once
#include <unordered_map>

using namespace std;

#include <Editor/QTForward.h>

class QtEditor
{
	private:
		QtEditor() = delete;
		~QtEditor() = delete;

	public:
		static QIcon* GetIcon(const char * fileName);
};
