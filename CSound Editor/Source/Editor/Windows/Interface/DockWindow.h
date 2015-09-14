#pragma once
#include <string>

#include <Editor/QTForward.h>
#include <QtWidgets/QDockWidget>

using namespace std;

class DockWindow
	: public QDockWidget
{
	public:
		DockWindow();
		virtual ~DockWindow() {};

		virtual void Init();
		virtual void Hide();
		virtual void Close() {};

		void ReloadStyleSheet();
		void LoadStyleSheet(const char* fileLocation);

	protected:
		virtual void DockedEvent(bool state);
		
	protected:
		string styleSheet;
		QBoxLayout* qtLayout;
};