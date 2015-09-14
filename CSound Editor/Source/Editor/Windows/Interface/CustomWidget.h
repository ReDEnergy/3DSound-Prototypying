#pragma once
#include <string>

#include <Editor/QTForward.h>
#include <QtWidgets/QWidget>

using namespace std;

class CustomWidget
	: public QWidget
{
	public:
		CustomWidget();
		virtual ~CustomWidget() {};

		virtual void Init();
		virtual void Hide();
		virtual void Close() {};

		void ReloadStyleSheet();
		void LoadStyleSheet(const char* fileLocation);

	protected:
		string styleSheet;
		QLayout* qtLayout;
};