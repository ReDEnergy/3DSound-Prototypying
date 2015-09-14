#pragma once
#include <Editor/Windows/Interface/GenericList.h>
#include <Editor/Windows/Interface/CSoundListEditor.h>

class CSoundInstrument;
class CSoundScore;
class ScoreEditor;

class ScoreEditorListWidget
	: public CSoundEditorListWidget<CSoundInstrument, QTListItem<CSoundInstrument>, ScoreEditor>
{
	public:
		ScoreEditorListWidget();
};

class ScoreEditor
	: public CSoundListEditor<CSoundScore, ScoreEditorListWidget>
{
	public:
		ScoreEditor();
		virtual ~ScoreEditor() {};
	
		void Update();
		void DropItem(CSoundInstrument *instr);
		void ListCleared();

	private:
		void QtItemClicked(QListWidgetItem * item);
	
};