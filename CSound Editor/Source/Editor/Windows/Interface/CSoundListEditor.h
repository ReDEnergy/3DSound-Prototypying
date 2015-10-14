#pragma once

#include <templates/singleton.h>

#include <Editor/Windows/Interface/GenericList.h>
#include <CSound/CSound.h>

#include <QAction>
#include <QMenu>

template <class CSoundContext, class L>
class CSoundListEditor : public GenericList<L>
{
	public:
		CSoundListEditor() {
			CreatePopupMenu();
			activeContext = nullptr;
		}

		virtual ~CSoundListEditor() {}

		void SetContext(CSoundContext *context)
		{
			if (!context) {
				cout << "ERROR: context is NULL";
				return;
			}

			GenericList::Clear();
			activeContext = context;
			for (auto &entry : context->GetEntries())
			{
				qtList->AddItem(entry);
			}

			if (qtList->count()) {
				qtList->setItemSelected(qtList->item(0), true);
				QtItemClicked(qtList->item(0));
			}
		}

		CSoundContext* GetActiveContext() {
			return activeContext;
		}

		void Clear()
		{
			activeContext = nullptr;
			GenericList::Clear();
		}

	protected:
		void CreatePopupMenu()
		{
			QAction* actionSaveAs = qtContextMenu->addAction("Save As");
			QObject::connect(actionSaveAs, &QAction::triggered, this, &CSoundListEditor::SaveItem);
		}

		void RemoveItem()
		{
			// In case the last item is removed the context will be NULL
			// The context must be NULL only for the child list editors not the one that triggered the event
			// Context will be saved in order to be restored
			// OBSERVATION: The saving will be triggered every single time an item is removed 
			// but should still be faster than testing the list dimension. 
			auto savedContext = activeContext;
			activeContext->Remove(qtList->GetItemData(qtList->currentItem()));
			GenericList::RemoveItem();
			activeContext = savedContext;
			this->Update();
		}

	private:
		void Update() {};

		void QtItemRenamed(QListWidgetItem * item)
		{
			auto C = qtList->GetItemData(item);
			if (item->text().toStdString().compare(C->GetName())) {
				C->SetName(item->text().toStdString().c_str());
			}
		}

		void SaveItem()
		{
			cout << "Saving" << endl;
		}

	public:
		void ReorederEvent()
		{
			if (qtList->currentRow() != activeIndex) {
				cout << "reodered" << endl;
				activeContext->Move(activeIndex, qtList->currentRow());
				activeIndex = qtList->currentRow();
				this->Update();
			}
		}

	protected:
		CSoundContext *activeContext;
};

template <class CSoundType, class ListContainer, class Parent>
class CSoundQtList
	: public QTListWidget<CSoundType, ListContainer>
{
	public:
		CSoundQtList() {};

		void DropEvent(QObject *source)
		{
			cout << "DROP" << endl;
			auto parent = Singleton<Parent>::Instance();
			if (source == this)
			{
				parent->ReorederEvent();
				return;
			}
			else {
				auto appData = GetItemData(((QListWidget*)source)->currentItem());
				parent->DropItem(new CSoundType(*appData));
			}
		}
};