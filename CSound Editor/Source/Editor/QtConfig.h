#pragma once
#include <unordered_map>

using namespace std;

#include <Editor/QTForward.h>

class QtConfig
{
	private:
		QtConfig() = delete;
		~QtConfig() = delete;

	public:
		static QIcon* GetIcon(const char * fileName);
		static bool SetWidgetStyleSheet(QWidget *widget, const char * file);
};
