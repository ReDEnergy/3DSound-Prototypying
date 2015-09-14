#include "ScoreEditor.h"

#include <Editor/Windows/TextPreview.h>
#include <Editor/Windows/Editors/InstrumentEditor.h>
#include <Editor/Windows/Lists/InstrumentList.h>
#include <3DWorld/CSound/CsoundScene.h>
#include <CSoundEditor.h>

#include <Editor/GUI.h>

// Engine Library
#include <Manager/Manager.h>
#include <Manager/EventSystem.h>


ScoreEditor::ScoreEditor()
{
	setWindowTitle("Score Editor");
	GUI::Set(QT_INSTACE::SCORE_EDITOR, (void*) this);
}

void ScoreEditor::Update()
{
	if (activeContext) {
		activeContext->Save();
	}
	Manager::GetEvent()->EmitSync("model-changed", activeContext);
}

void ScoreEditor::QtItemClicked(QListWidgetItem * item)
{
	CSoundListEditor::QtItemClicked(item);
	auto data = qtList->GetItemData(item);
	GUI::Get<InstrumentEditor>(QT_INSTACE::INSTRUMENT_EDITOR)->SetContext(data);
	GUI::Get<TextPreviewWindow>(QT_INSTACE::TEXT_PREVIEW)->RenderText(data->GetRender());
}

void ScoreEditor::DropItem(CSoundInstrument * instr)
{
	if (activeContext) {
		CSoundEditor::GetScene()->TrackInstrument(instr);
		instr->Update();
		activeContext->Add(instr);
		SetContext(activeContext);
		Update();
	}
}

void ScoreEditor::ListCleared()
{
	GUI::Get<InstrumentEditor>(QT_INSTACE::INSTRUMENT_EDITOR)->Clear();
	GUI::Get<TextPreviewWindow>(QT_INSTACE::TEXT_PREVIEW)->RenderText("");
}

ScoreEditorListWidget::ScoreEditorListWidget()
{
	auto W = GUI::Get<InstrumentList>(QT_INSTACE::INSTRUMENT_LIST)->GetQtList();
	AddDragnDropSource(W);
}