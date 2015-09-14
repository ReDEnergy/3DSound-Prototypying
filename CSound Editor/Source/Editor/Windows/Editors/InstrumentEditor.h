#pragma once
#include <Csound/CSoundInstrument.h>
#include <Csound/CSoundScore.h>

#include <Editor/Windows/Interface/GenericList.h>
#include <Editor/Windows/Interface/CSoundListEditor.h>

class CSoundComponent;
class CSoundInstrument;
class InstrumentEditor;


class InstrumentEditorListWidget
	: public CSoundEditorListWidget<CSoundComponent, QTListItem<CSoundComponent>, InstrumentEditor>
{
	public:
		InstrumentEditorListWidget();
};

class InstrumentEditor
	: public CSoundListEditor<CSoundInstrument, InstrumentEditorListWidget>
{
	public:
		InstrumentEditor();
		virtual ~InstrumentEditor() {};

		void Update();
		void DropItem(CSoundComponent *comp);
		void ListCleared();

	private:
		void QtItemClicked(QListWidgetItem * item);

};