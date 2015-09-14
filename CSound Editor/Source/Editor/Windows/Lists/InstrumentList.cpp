#include "InstrumentList.h"

#include <Editor/Windows/Interface/QtListTemplates.h>
#include <Editor/Windows/Editors/InstrumentEditor.h>
#include <Editor/Windows/TextPreview.h>
#include <Editor/GUI.h>

typedef QTListItem<CSoundInstrument> ListItem;

InstrumentList::InstrumentList()
{
	setWindowTitle("Instruments");
	GUI::Set(QT_INSTACE::INSTRUMENT_LIST, (void*) this);
}

void InstrumentList::Init()
{
	auto entries = SoundManager::GetCSManager()->instruments->GetEntries();
	for (auto &entry : entries)
	{
		auto item =  new ListItem(entry);
		qtList->addItem(item);
	}
}

void InstrumentList::QtItemClicked(QListWidgetItem * item)
{
	cout << item->text().toStdString().c_str()  << endl;
	GUI::Get<TextPreviewWindow>(QT_INSTACE::TEXT_PREVIEW)->RenderText(((ListItem*)item)->GetData()->GetRender());
}

void InstrumentList::QtItemRenamed(QListWidgetItem * item)
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