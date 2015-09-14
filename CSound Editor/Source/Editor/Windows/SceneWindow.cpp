#include "SceneWindow.h"

#include <iostream>
#include <include/math.h>

#include <Editor/Windows/Editors/ScoreEditor.h>
#include <Editor/Windows/Interface/QtListTemplates.h>
#include <Editor/Windows/Lists/ScoreList.h>
#include <Editor/Windows/TextPreview.h>
#include <Editor/GUI.h>
#include <CSoundEditor.h>
#include <3DWorld/CSound/CSound3DSource.h>
#include <3DWorld/CSound/CSoundScene.h>

#include <QLayout>
#include <QMenu>
#include <QPushButton>
#include <QDragEnterEvent>


typedef QTTreeItem<CSound3DSource> SceneItem;
typedef QTListItem<CSoundScore> DropItem;

SceneWindow::SceneWindow()
{
	setWindowTitle("SceneWindow");

	InitUI();
	CreatePopupMenu();

	GUI::Set(QT_INSTACE::SCENE_EDITOR, (void*) this);
}

void SceneWindow::Init()
{
	Clear();
	auto entries = CSoundEditor::GetScene()->_3DSources->GetEntries();
	for (auto &entry : entries)
	{
		auto item = new SceneItem(entry);
		rootItem->addChild(item);
	}
}

void SceneWindow::InitUI()
{
	qtTree = new SceneTreeWidget();
	qtTree->setDragEnabled(true);
	qtTree->setDragDropMode(QAbstractItemView::InternalMove);
	qtTree->setHeaderLabel("3D Sound Sources");
	qtTree->setHeaderHidden(true);
	qtTree->setAlternatingRowColors(true);
	qtLayout->addWidget(qtTree);

	QObject::connect(qtTree, &QTreeWidget::itemPressed, this, &SceneWindow::QtItemClicked);

	QPushButton *button = new QPushButton();
	button->setMinimumSize(0, 25);
	button->setText("Add Source");
	qtLayout->addWidget(button);

	QObject::connect(button, &QPushButton::clicked, this, &SceneWindow::CreateItem);

	rootItem = qtTree->invisibleRootItem();
}


void SceneWindow::Clear()
{
	qtTree->clear();
	GUI::Get<ScoreEditor>(QT_INSTACE::SCORE_EDITOR)->Clear();
}


void SceneWindow::PlayItem()
{
	((SceneItem*)qtTree->currentItem())->GetData()->PlayScore();
}

void SceneWindow::StopItem()
{
	((SceneItem*)qtTree->currentItem())->GetData()->StopScore();
}

void SceneWindow::ReloadItem()
{
	((SceneItem*)qtTree->currentItem())->GetData()->ReloadScore();
}

void SceneWindow::RemoveItem()
{
	int row = qtTree->currentIndex().row();
	auto item = qtTree->takeTopLevelItem(row);
	CSoundEditor::GetScene()->Remove(((SceneItem*)item)->GetData());
	SelectFirstTopLevelItem(row);
}

void SceneWindow::RenameItem()
{
	qtTree->editItem(qtTree->currentItem());
}

void SceneWindow::CreatePopupMenu()
{
	qtContextMenu = new QMenu(this);

	QAction* actionPlay = qtContextMenu->addAction("Play");
	QAction* actionStop = qtContextMenu->addAction("Stop");
	QAction* actionReload = qtContextMenu->addAction("Reload");
	QAction* actionRename = qtContextMenu->addAction("Rename");
	QAction* actionDelete = qtContextMenu->addAction("Delete");

	qtTree->setContextMenuPolicy(Qt::CustomContextMenu);

	QObject::connect(qtTree, &QWidget::customContextMenuRequested, this, &SceneWindow::OpenPopupMenu);

	QObject::connect(actionPlay, &QAction::triggered, this, &SceneWindow::PlayItem);
	QObject::connect(actionStop, &QAction::triggered, this, &SceneWindow::StopItem);
	QObject::connect(actionReload, &QAction::triggered, this, &SceneWindow::ReloadItem);
	QObject::connect(actionRename, &QAction::triggered, this, &SceneWindow::RenameItem);
	QObject::connect(actionDelete, &QAction::triggered, this, &SceneWindow::RemoveItem);
}

void SceneWindow::OpenPopupMenu(const QPoint & pos)
{
	auto qtItem = qtTree->itemAt(pos);

	if (qtItem) {
		qtContextMenu->popup(qtTree->mapToGlobal(pos));
	}
}

void SceneWindow::QtItemClicked(QTreeWidgetItem *item, int column)
{
	((SceneItem*)item)->GetData()->SelectObject();
	GUI::Get<ScoreEditor>(QT_INSTACE::SCORE_EDITOR)->SetContext(((SceneItem*)item)->GetData()->GetScore());
	GUI::Get<TextPreviewWindow>(QT_INSTACE::TEXT_PREVIEW)->RenderText(((SceneItem*)item)->GetData()->GetRender());
}

void SceneWindow::QtItemRenamed(QListWidgetItem * item)
{
	auto C = ((SceneItem*)item)->GetData();
	if (item->text().toStdString().compare(C->GetName())) {
		bool rez = CSoundEditor::GetScene()->Rename(C, item->text().toStdString().c_str());
		if (rez == false) {
			item->setText(C->GetName());
		}
	}
}

void SceneWindow::DropItem(CSoundScore * score)
{
}

void SceneWindow::SelectFirstTopLevelItem(int row)
{
	// Clear the Sound Editor Panel if the list is empty
	int count = qtTree->topLevelItemCount();
	if (count == 0)
	{
		GUI::Get<ScoreEditor>(QT_INSTACE::SCORE_EDITOR)->Clear();
		return;
	}

	auto item = qtTree->itemAt(0, 0);
	qtTree->setCurrentItem(item, 0);
	QtItemClicked(item, 0);
}

void SceneWindow::CreateItem()
{
	auto S = CSoundEditor::GetScene()->CreateSource();
	auto item = new SceneItem(S);
	rootItem->addChild(item);
	qtTree->setCurrentItem(item, 0);
	QtItemClicked(item, 0);
}

void SceneTreeWidget::dragEnterEvent(QDragEnterEvent * event)
{
	cout << "enter" << endl;
	auto source = event->source();
	auto W = GUI::Get<ScoreList>(QT_INSTACE::SCORE_LIST)->GetQtList();
	if (source == this || source == W) {
		event->acceptProposedAction();
	}
}

void SceneTreeWidget::dropEvent(QDropEvent * event)
{
	auto source = event->source();

	if (source == this) {
		QTreeWidget::dropEvent(event);
		return;
	}

	auto W = GUI::Get<ScoreList>(QT_INSTACE::SCORE_LIST)->GetQtList();
	if (source == W) {
		auto appData = ((DropItem*)((QListWidget*)source)->currentItem())->GetData();
		((SceneWindow*)(this->parent()->parent()))->DropItem(appData);
		cout << "DROP FROM [SCORE LIST]: " << appData->GetName() << endl;
	}

}
