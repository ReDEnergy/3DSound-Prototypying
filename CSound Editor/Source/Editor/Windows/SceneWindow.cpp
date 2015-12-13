#include "SceneWindow.h"

#include <iostream>
#include <include/math.h>

#include <Editor/Windows/Editors/ScoreEditor.h>
#include <Editor/Windows/Interface/QtListTemplates.h>
#include <Editor/Windows/Lists/ScoreList.h>
#include <Editor/Windows/TextPreview.h>
#include <Editor/Windows/Code/CodeEditorWindow.h>
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
	setWindowTitle("Scene Objects");

	InitUI();
	CreatePopupMenu();

	GUI::Set(QT_INSTACE::SCENE_EDITOR, (void*) this);
	SubscribeToEvent(EventType::EDITOR_OBJECT_SELECTED);
	SubscribeToEvent(EventType::EDITOR_OBJECT_ADDED);
	SubscribeToEvent(EventType::EDITOR_OBJECT_REMOVED);
}

void SceneWindow::Init()
{
	Clear();
	auto entries = CSoundEditor::GetScene()->GetEntries();
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
	GUI::Get<ScoreComposer>(QT_INSTACE::SCORE_COMPOSER)->Clear();
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

void SceneWindow::InsertItem(CSound3DSource * data)
{
	auto item = new SceneItem(data);
	rootItem->addChild(item);
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

void SceneWindow::CreateItem()
{
	auto S = CSoundEditor::GetScene()->CreateSource();
	auto item = new SceneItem(S);
	rootItem->addChild(item);
	qtTree->setCurrentItem(item, 0);
	QtItemClicked(item, 0);
}

void SceneWindow::DropItem(CSoundScore * score)
{
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

void SceneWindow::FindAndSelectItem(void *data)
{
	if (data == nullptr) return;

	auto noItems = qtTree->topLevelItemCount();
	for (int i = 0; i < noItems; i++) {
		auto item = qtTree->topLevelItem(i);
		auto itemData = ((SceneItem*)item)->GetData();
		if (itemData == data) {
			qtTree->setCurrentItem(item);
			QtItemClicked(item, 0);
			return;
		}
	}
}

void SceneWindow::FindAndDeleteItem(void *data)
{
	if (data == nullptr) return;
	auto obj = dynamic_cast<CSound3DSource*>((GameObject*)data);

	if (obj == nullptr) return;

	auto noItems = qtTree->topLevelItemCount();
	for (int i = 0; i < noItems; i++) {
		auto itemData = ((SceneItem*)qtTree->topLevelItem(i))->GetData();
		if (itemData == data)
		{
			bool selectNewItem = (qtTree->currentIndex().row() == i);
			qtTree->takeTopLevelItem(i);
			if (selectNewItem && noItems > 1)
			{
				// select the previous element if there is any
				if (i) i--;
				auto item = qtTree->topLevelItem(i);
				qtTree->setCurrentItem(item);
				QtItemClicked(item, 0);
			}
			else {
				Clear();
			}
		break;
		}
	}
	cout << "Delete: " << ((GameObject*)data)->GetName() << endl;
}

void SceneWindow::OpenPopupMenu(const QPoint & pos)
{
	cout << pos.x() << "-" << pos.y() << endl;
	auto qtItem = qtTree->itemAt(pos);

	if (qtItem) {
		qtContextMenu->popup(qtTree->mapToGlobal(pos));
	}
}

void SceneWindow::QtItemClicked(QTreeWidgetItem *item, int column)
{
	auto Data = ((SceneItem*)item)->GetData();
	Data->SelectObject();
	GUI::Get<ScoreComposer>(QT_INSTACE::SCORE_COMPOSER)->SetContext(Data->GetScore());
	GUI::Get<TextPreviewWindow>(QT_INSTACE::TEXT_PREVIEW)->RenderText(Data->GetRender());
	GUI::Get<CodeEditorWindow>(QT_INSTACE::CODE_EDITOR_WINDOW)->SetContext(Data->GetScore());
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

void SceneWindow::OnEvent(EventType Event, void * data)
{
	if (Event == EventType::EDITOR_OBJECT_SELECTED) {
		FindAndSelectItem(data);
	}
	if (Event == EventType::EDITOR_OBJECT_REMOVED) {
		FindAndDeleteItem(data);
	}
}

void SceneWindow::SelectFirstTopLevelItem(int row)
{
	// Clear the Sound Editor Panel if the list is empty
	int count = qtTree->topLevelItemCount();
	if (count == 0)
	{
		GUI::Get<ScoreComposer>(QT_INSTACE::SCORE_COMPOSER)->Clear();
		return;
	}

	auto item = qtTree->topLevelItem(0);
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
