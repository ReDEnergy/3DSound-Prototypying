#pragma once
#include <Editor/Windows/Interface/DockWindow.h>

class CSoundSynthesizer;
class QTextEdit;
class QTimer;

class CodeEditorWindow
	: public DockWindow
{
	public:
		CodeEditorWindow();
		virtual ~CodeEditorWindow() {};

		void SetContext(CSoundSynthesizer *score);

	private:
		void InitUI();

	private:
		CSoundSynthesizer *activeContext;
		QTextEdit *qtTextEdit;
		QTimer *codeUpdateTimer;
		bool disableFirstUpdate;
}; 