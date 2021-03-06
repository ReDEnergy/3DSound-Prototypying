#pragma once
#include <Editor/Windows/Interface/CSoundListEditor.h>

class CSoundInstrument;
class CSoundSynthesizer;
class ScoreComposer;

class ScoreEditorList
	: public CSoundQtList<CSoundInstrument, QTListItem<CSoundInstrument>, ScoreComposer>
{
	public:
		ScoreEditorList();
};

class ScoreComposer
	: public CSoundListEditor<CSoundSynthesizer, ScoreEditorList>
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
		void Update();

	private:
		void InitUI();

	private:
		ScoreComposer *composer;
};