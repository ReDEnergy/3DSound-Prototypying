#pragma once
#include <Editor/Windows/Interface/DockWindow.h>

class CSoundScore;
class QTextEdit;
class QTimer;

class CodeEditorWindow
	: public DockWindow
{
	public:
		CodeEditorWindow();
		virtual ~CodeEditorWindow() {};

		void SetContext(CSoundScore *score);

	private:
		void InitUI();

	private:
		CSoundScore *activeContext;
		QTextEdit *qtTextEdit;
		QTimer *codeUpdateTimer;
		bool disableFirstUpdate;
}; 