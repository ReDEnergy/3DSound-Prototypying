#include "ScoreList.h"

#include <Editor/Windows/Interface/QtListTemplates.h>
#include <Editor/Windows/TextPreview.h>
#include <Editor/GUI.h>

#include <CSoundEditor.h>
#include <3DWorld/CSound/CSoundScene.h>

#include <Csound/CSound.h>

#include <QAction>
#include <QLayout>
#include <QMenu>

typedef QTListItem<CSoundScore> ListItem;

ScoreList::ScoreList()
{
	setWindowTitle("Scores");
	GUI::Set(QT_INSTACE::SCORE_LIST, (void*) this);

	QAction* actionDefaultScore = qtContextMenu->addAction("Set Default");
	QObject::connect(actionDefaultScore, &QAction::triggered, this, &ScoreList::SetDefaultScore);
}

void ScoreList::Init()
{
	auto entries = SoundManager::GetCSManager()->scores->GetEntries();
	for (auto &entry : entries)
	{
		auto item =  new ListItem(entry);
		qtList->addItem(item);
	}
}

void ScoreList::AddItem()
{
	//auto S = SoundManager::GetCSManager()->CreateScore();
	//auto item = new ListItem(S);
	//qtList->insertItem(qtList->currentRow() + 1, item);
}

void ScoreList::SetDefaultScore()
{
	CSoundEditor::GetScene()->SetDefaultScore(((ListItem*)qtList->currentItem())->GetData());
}

void ScoreList::QtItemClicked(QListWidgetItem * item)
{
	EditorList::QtItemClicked(item);
	GUI::Get<TextPreviewWindow>(QT_INSTACE::TEXT_PREVIEW)->RenderText(((ListItem*)item)->GetData()->GetRender());
}

void ScoreList::QtItemRenamed(QListWidgetItem * item)
{
	auto C = ((ListItem*)item)->GetData();
	if (item->text().toStdString().compare(C->GetName())) {
		bool rez = C->Rename(item->text().toStdString().c_str());
		if (rez == false) {
			item->setText(C->GetName());
		}
	}
	cout << item->text().toStdString().c_str() << endl;
}