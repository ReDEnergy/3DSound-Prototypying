#pragma once
#include <Editor/Windows/Interface/DockWindow.h>

class CSoundComponent;

class TextPreviewWindow
	: public DockWindow
{
	public:
		TextPreviewWindow();
		virtual ~TextPreviewWindow() {};

		void RenderText(const char * text);

	private:
		void InitUI();

	private:
		QTextEdit *qtTextEdit;
}; 