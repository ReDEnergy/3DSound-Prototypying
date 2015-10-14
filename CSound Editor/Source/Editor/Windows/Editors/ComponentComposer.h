#pragma once
#include <Editor/Windows/Interface/CSoundListEditor.h>

class CSoundComponentProperty;
class CSoundComponent;

class ComponentPropertyEntry
	: public QTListItem<CSoundComponentProperty>, public QObject
{
	public:
		ComponentPropertyEntry(CSoundComponentProperty *prop);
		~ComponentPropertyEntry();

	private:
		void QtItemValueChange();

	private:
		QLineEdit *input;
};

class ComponentComposer
	: public CSoundListEditor<CSoundComponent, CSoundQtList<CSoundComponentProperty, ComponentPropertyEntry, ComponentComposer>>
{
	protected:
		ComponentComposer();
		virtual ~ComponentComposer() {};

	public:
		void Init();
		void Update();
		void AddItem();
		void ListCleared();
		void DropItem(CSoundComponentProperty *property) {};

	private:
		void InitUI();
		void QtItemClicked(QListWidgetItem * item);
};