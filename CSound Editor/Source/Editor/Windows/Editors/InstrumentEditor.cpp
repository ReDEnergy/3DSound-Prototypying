#include "InstrumentEditor.h"

#include <Editor/GUI.h>

#include <Editor/Windows/TextPreview.h>
#include <Editor/Windows/Editors/ScoreEditor.h>
#include <Editor/Windows/Editors/ComponentEditor.h>
#include <Editor/Windows/Lists/ComponentList.h>

#include <Csound/CSoundInstrument.h>

InstrumentEditor::InstrumentEditor()
{
	setWindowTitle("Instrument Editor");
	GUI::Set(QT_INSTACE::INSTRUMENT_EDITOR, (void*) this);
}

void InstrumentEditor::Update()
{
	if (activeContext) {
		GUI::Get<ScoreEditor>(QT_INSTACE::SCORE_EDITOR)->Update();
		GUI::Get<TextPreviewWindow>(QT_INSTACE::TEXT_PREVIEW)->RenderText(activeContext->GetRender());
	}
}

void InstrumentEditor::DropItem(CSoundComponent * comp)
{
	if (activeContext) {
		activeContext->Add(comp);
		SetContext(activeContext);
		Update();
	}
}

void InstrumentEditor::ListCleared()
{
	GUI::Get<ComponentEditor>(QT_INSTACE::COMPONENT_EDITOR)->Clear();
}

void InstrumentEditor::QtItemClicked(QListWidgetItem * item)
{
	CSoundListEditor::QtItemClicked(item);
	auto data = qtList->GetItemData(item);
	GUI::Get<ComponentEditor>(QT_INSTACE::COMPONENT_EDITOR)->SetContext(data);
}

InstrumentEditorListWidget::InstrumentEditorListWidget()
{
	auto W = GUI::Get<ComponentList>(QT_INSTACE::COMPONENT_LIST)->GetQtList();
	AddDragnDropSource(W);
}
