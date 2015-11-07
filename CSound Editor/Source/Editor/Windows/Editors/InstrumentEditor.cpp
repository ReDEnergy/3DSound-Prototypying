#include "InstrumentEditor.h"

#include <templates/singleton.h>

#include <Editor/GUI.h>

#include <Editor/Windows/TextPreview.h>
#include <Editor/Windows/Editors/ScoreEditor.h>
//#include <Editor/Windows/Editors/ComponentEditor.h>
#include <Editor/Windows/Editors/ComponentComposer.h>
#include <Editor/Windows/Lists/ComponentList.h>

#include <Csound/CSoundInstrument.h>

InstrumentComposer::InstrumentComposer()
{
	GUI::Set(QT_INSTACE::INSTRUMENT_COMPOSER, (void*) this);
}

void InstrumentComposer::Update()
{
	if (activeContext) {
		GUI::Get<ScoreComposer>(QT_INSTACE::SCORE_COMPOSER)->Update();
		GUI::Get<TextPreviewWindow>(QT_INSTACE::TEXT_PREVIEW)->RenderText(activeContext->GetRender());
	}
}

void InstrumentComposer::DropItem(CSoundComponent * comp)
{
	if (activeContext) {
		activeContext->Add(comp);
		SetContext(activeContext);
		Update();
	}
}

void InstrumentComposer::ListCleared()
{
	GUI::Get<ComponentComposer>(QT_INSTACE::COMPONENT_COMPOSER)->Clear();
}

void InstrumentComposer::QtItemClicked(QListWidgetItem * item)
{
	CSoundListEditor::QtItemClicked(item);
	auto data = qtList->GetItemData(item);
	GUI::Get<ComponentComposer>(QT_INSTACE::COMPONENT_COMPOSER)->SetContext(data);
}

InstrumentEditorList::InstrumentEditorList()
{
	auto W = GUI::Get<ComponentList>(QT_INSTACE::COMPONENT_LIST)->GetQtList();
	AddDragnDropSource(W);
}

InstrumentEditor::InstrumentEditor()
{
	setWindowTitle("Instrument Editor");
	InitUI();
}

InstrumentEditor::~InstrumentEditor()
{
}

void InstrumentEditor::InitUI()
{
	composer = Singleton<InstrumentComposer>::Instance();
	qtLayout->addWidget(composer);
}

void InstrumentEditor::Init()
{
	composer->Init();
}
