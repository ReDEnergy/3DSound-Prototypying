#pragma once
#include <string>

#include <Editor/include/QTForward.h>

class QtConfig
{
	private:
		QtConfig() = delete;
		~QtConfig() = delete;

	public:
		static std::string GetAppName();
		static std::string GetAppVersion();
		static QIcon* GetIcon(const char * fileName);
		static bool SetWidgetStyleSheet(QWidget *widget, const char * file);
};
