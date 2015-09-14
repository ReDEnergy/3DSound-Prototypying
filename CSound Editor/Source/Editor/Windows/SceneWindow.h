#pragma once
#include <Editor/Windows/Interface/DockWindow.h>

class CSoundScore;

#include <QTreeWidget>

class SceneTreeWidget : public QTreeWidget
{
public:
	void dragEnterEvent(QDragEnterEvent *event);
	void dropEvent(QDropEvent *event);
};

class SceneWindow
	: public DockWindow
{
	friend class SceneTreeWidget;

	public:
		SceneWindow();
		virtual ~SceneWindow() {};

		void CreateItem();
		void Clear();
		void Init();

	private:
		void InitUI();

		void PlayItem();
		void StopItem();
		void ReloadItem();
		void RemoveItem();
		void RenameItem();
		void CreatePopupMenu();

		void OpenPopupMenu(const QPoint & pos);
		void QtItemClicked(QTreeWidgetItem* item, int column);
		void QtItemRenamed(QListWidgetItem* item);

	protected:
		void DropItem(CSoundScore* score);

	private:
		void SelectFirstTopLevelItem(int row = 0);

	private:
		QTreeWidget *qtTree;
		QTreeWidgetItem* rootItem;
		QMenu *qtContextMenu;
};