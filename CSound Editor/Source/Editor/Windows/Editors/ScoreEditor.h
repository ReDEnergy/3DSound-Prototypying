#pragma once
#include <Editor/Windows/Interface/CSoundListEditor.h>

class CSoundInstrument;
class CSoundScore;
class ScoreComposer;

class ScoreEditorList
	: public CSoundQtList<CSoundInstrument, QTListItem<CSoundInstrument>, ScoreComposer>
{
	public:
		ScoreEditorList();
};

class ScoreComposer
	: public CSoundListEditor<CSoundScore, ScoreEditorList>
{
	public:
		ScoreComposer();
		virtual ~ScoreComposer() {};
	
		void Update();
		void DropItem(CSoundInstrument *instr);
		void ListCleared();

	private:
		void QtItemClicked(QListWidgetItem * item);
};

class ScoreEditor : public DockWindow
{
	public:
		ScoreEditor();
		~ScoreEditor();

		void Init();

	private:
		void InitUI();

	private:
		ScoreComposer *composer;
};