#pragma once
#include <Editor/Windows/Interface/GenericList.h>
#include <Editor/Windows/Interface/CSoundListEditor.h>

class CSoundComponentProperty;
class ComponentEditor;
class CSoundComponent;
class PropertyEditor;

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

class ComponentEditorListWidget
	: public CSoundEditorListWidget<CSoundComponentProperty, ComponentPropertyEntry, ComponentEditor>
{
	public:
		ComponentEditorListWidget() {};
};

class ComponentEditor
	: public CSoundListEditor<CSoundComponent, ComponentEditorListWidget>
{
	public:
		ComponentEditor();
		virtual ~ComponentEditor() {};

		void Init();
		void Update();
		void AddItem();
		void ListCleared();
		void DropItem(CSoundComponentProperty *property) {};

	private:
		void InitUI();
		void QtItemClicked(QListWidgetItem * item);
		PropertyEditor *propertyEditor;
};