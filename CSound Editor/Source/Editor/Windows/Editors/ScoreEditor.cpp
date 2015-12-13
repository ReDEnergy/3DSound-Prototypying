#include "ScoreEditor.h"

#include <templates/singleton.h>

#include <Editor/Windows/TextPreview.h>
#include <Editor/Windows/Editors/InstrumentEditor.h>
#include <Editor/Windows/Lists/InstrumentList.h>
#include <3DWorld/CSound/CsoundScene.h>
#include <CSoundEditor.h>

#include <Editor/GUI.h>

// Engine Library
#include <Manager/Manager.h>
#include <Manager/EventSystem.h>


ScoreComposer::ScoreComposer()
{
	GUI::Set(QT_INSTACE::SCORE_COMPOSER, (void*) this);
}

void ScoreComposer::Update()
{
	if (activeContext) {
		activeContext->SaveToFile();
		GUI::Get<TextPreviewWindow>(QT_INSTACE::TEXT_PREVIEW)->RenderText(activeContext->GetRender());
	}
	
	Manager::GetEvent()->EmitAsync("model-changed", activeContext);
}

void ScoreComposer::QtItemClicked(QListWidgetItem * item)
{
	CSoundListEditor::QtItemClicked(item);
	auto data = qtList->GetItemData(item);
	GUI::Get<InstrumentComposer>(QT_INSTACE::INSTRUMENT_COMPOSER)->SetContext(data);
	GUI::Get<TextPreviewWindow>(QT_INSTACE::TEXT_PREVIEW)->RenderText(data->GetRender());
}

void ScoreComposer::DropItem(CSoundInstrument * instr)
{
	if (activeContext) {
		CSoundEditor::GetScene()->TrackInstrument(instr);
		instr->Update();
		activeContext->Add(instr);
		SetContext(activeContext);
		Update();
	}
}

void ScoreComposer::ListCleared()
{
	GUI::Get<InstrumentComposer>(QT_INSTACE::INSTRUMENT_COMPOSER)->Clear();
	GUI::Get<TextPreviewWindow>(QT_INSTACE::TEXT_PREVIEW)->RenderText("");
}

ScoreEditorList::ScoreEditorList()
{
	auto W = GUI::Get<InstrumentList>(QT_INSTACE::INSTRUMENT_LIST)->GetQtList();
	AddDragnDropSource(W);
}

ScoreEditor::ScoreEditor()
{
	setWindowTitle("Instruments");
	InitUI();
}

ScoreEditor::~ScoreEditor()
{
}

void ScoreEditor::InitUI()
{
	composer = Singleton<ScoreComposer>::Instance();
	qtLayout->addWidget(composer);
}

void ScoreEditor::Init()
{
	composer->Init();
}

void ScoreEditor::Update()
{
	auto activeContext = composer->GetActiveContext();
	if (activeContext)
		GUI::Get<TextPreviewWindow>(QT_INSTACE::TEXT_PREVIEW)->RenderText(activeContext->GetRender());
}
