#include "ComponentList.h"

#include <Editor/Windows/Interface/QtListTemplates.h>
#include <Editor/Windows/Editors/ComponentComposer.h>
#include <Editor/Windows/TextPreview.h>
#include <Editor/GUI.h>

#include <Csound/Csound.h>

typedef QTListItem<CSoundComponent> ListItem;

ComponentList::ComponentList()
{
	setWindowTitle("Components");
	GUI::Set(QT_INSTACE::COMPONENT_LIST, (void*) this);
}

void ComponentList::Init()
{
	auto entries = SoundManager::GetCSManager()->components->GetEntries();
	for (auto entry : entries)
	{
		auto item = new ListItem(entry);
		qtList->addItem(item);
	}
}

void ComponentList::QtItemClicked(QListWidgetItem * item)
{
	EditorList::QtItemClicked(item);
	auto data = ((ListItem*)item)->GetData();
	GUI::Get<TextPreviewWindow>(QT_INSTACE::TEXT_PREVIEW)->RenderText(((ListItem*)item)->GetData()->GetRender());
	//GUI::Get<ComponentComposer>(QT_INSTACE::COMPONENT_COMPOSER)->SetContext(data);
}

void ComponentList::QtItemRenamed(QListWidgetItem * item)
{
	EditorList::QtItemRenamed(item);
	auto C = ((ListItem*)item)->GetData();
	if (item->text().toStdString().compare(C->GetName())) {
		bool rez = C->Rename(item->text().toStdString().c_str());
		if (rez == false) {
			item->setText(C->GetName());
		}
	}
	cout << item->text().toStdString().c_str() << endl;
}