#pragma once
#include <Csound/CSoundInstrument.h>
#include <Csound/CSoundScore.h>

#include <Editor/Windows/Interface/CSoundListEditor.h>

class CSoundComponent;
class CSoundInstrument;
class InstrumentComposer;

class InstrumentEditorList
	: public CSoundQtList<CSoundComponent, QTListItem<CSoundComponent>, InstrumentComposer>
{
	public:
		InstrumentEditorList();
};

class InstrumentComposer
	: public CSoundListEditor<CSoundInstrument, InstrumentEditorList>
{
	public:
		InstrumentComposer();
		virtual ~InstrumentComposer() {};

		void Update();
		void DropItem(CSoundComponent *comp);
		void ListCleared();

	private:
		void QtItemClicked(QListWidgetItem * item);

};

class InstrumentEditor : public DockWindow
{
	public:
		InstrumentEditor();
		~InstrumentEditor();

		void Init();
	
	private:
		void InitUI();

	private:
		InstrumentComposer *composer;
};