#pragma once
#include <Editor/Windows/Interface/DockWindow.h>
#include <Editor/Windows/Interface/QtListTemplates.h>

#include <QLayout>

template <class QTListWidget>
class GenericList
	: public DockWindow
{
	public:
		GenericList();
		virtual ~GenericList() {};

		QTListWidget * GetQtList() const;

		virtual void Clear();
		virtual void Update() {};

	protected:
		virtual void InitUI();
		virtual void ListCleared() {};

		virtual void AddItem() {};
		virtual void RemoveItem();
		virtual void RenameItem();
		virtual void CreatePopupMenu();

		virtual void OpenPopupMenu(const QPoint & pos);
		virtual void QtItemClicked(QListWidgetItem *item);
		virtual void QtItemRenamed(QListWidgetItem *item) {};

	protected:
		unsigned int activeIndex;
		QListWidgetItem *activeItem;
		QTListWidget *qtList;
		QMenu *qtContextMenu;
};

template<class QTListWidget>
inline GenericList<QTListWidget>::GenericList()
{
	setWindowTitle("GenericList");

	InitUI();
	CreatePopupMenu();
}

template<class QTListWidget>
inline QTListWidget * GenericList<QTListWidget>::GetQtList() const
{
	return qtList;
}

template<class QTListWidget>
inline void GenericList<QTListWidget>::Clear()
{
	activeItem = nullptr;
	activeIndex = -1;
	qtList->clear();
	ListCleared();
}

template<class QTListWidget>
inline void GenericList<QTListWidget>::InitUI()
{
	qtList = new QTListWidget();
	qtList->setDragEnabled(true);
	qtList->setDragDropMode(QAbstractItemView::InternalMove);
	qtList->setAlternatingRowColors(true);
	qtLayout->addWidget(qtList);

	QObject::connect(qtList, &QListWidget::itemPressed, this, &GenericList::QtItemClicked);
	QObject::connect(qtList, &QListWidget::itemChanged, this, &GenericList::QtItemRenamed);
}

template<class QTListWidget>
inline void GenericList<QTListWidget>::RemoveItem()
{
	qtList->takeItem(qtList->currentRow());
	if (qtList->count() == 0)
		Clear();
}

template<class QTListWidget>
inline void GenericList<QTListWidget>::RenameItem()
{
	qtList->editItem(qtList->currentItem());
}

template<class QTListWidget>
inline void GenericList<QTListWidget>::CreatePopupMenu()
{
	qtList->setContextMenuPolicy(Qt::CustomContextMenu);
	QObject::connect(qtList, &QWidget::customContextMenuRequested, this, &GenericList::OpenPopupMenu);

	qtContextMenu = new QMenu(this);

	QAction* actionDefine = qtContextMenu->addAction("Add");
	QAction* actionRename = qtContextMenu->addAction("Rename");
	QAction* actionRemove = qtContextMenu->addAction("Delete");

	QObject::connect(actionDefine, &QAction::triggered, this, &GenericList::AddItem);
	QObject::connect(actionRename, &QAction::triggered, this, &GenericList::RenameItem);
	QObject::connect(actionRemove, &QAction::triggered, this, &GenericList::RemoveItem);
}

template<class QTListWidget>
inline void GenericList<QTListWidget>::OpenPopupMenu(const QPoint & pos)
{
	auto qtItem = qtList->itemAt(pos);

	if (qtItem) {
		qtContextMenu->popup(qtList->mapToGlobal(pos));
	}
}

template<class QTListWidget>
inline void GenericList<QTListWidget>::QtItemClicked(QListWidgetItem * item)
{
	activeIndex = qtList->currentRow();
	if (item) {
		cout << item->text().toStdString().c_str() << endl;
	}
}