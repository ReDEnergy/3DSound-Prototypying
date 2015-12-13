#include "ComponentComposer.h"

#include <iostream>

#include <Editor/Windows/TextPreview.h>
#include <Editor/Windows/Editors/InstrumentEditor.h>
#include <Editor/Windows/Editors/ScoreEditor.h>
#include <Editor/Windows/Editors/PropertyEditor.h>
#include <Editor/GUI.h>

#include <CSound/CSoundComponentProperty.h>
#include <CSound/CSoundComponent.h>

#include <include/Qt.h>
#include <include/utils.h>

using namespace std;

static const string op("op");

ComponentComposer::ComponentComposer()
{
	setWindowTitle("Component");
	GUI::Set(QT_INSTACE::COMPONENT_COMPOSER, (void*) this);

	InitUI();
}

void ComponentComposer::Update()
{
	GUI::Get<InstrumentComposer>(QT_INSTACE::INSTRUMENT_COMPOSER)->Update();
}

void ComponentComposer::AddItem()
{
	if (activeContext) {
		CSoundComponentProperty *P = new CSoundComponentProperty(activeContext);
		P->SetName("text");
		P->SetDefault("0");
		activeContext->Add(P);
		SetContext(activeContext);
		Update();
	}
}

void ComponentComposer::ListCleared()
{
	//GUI::Get<CSoundPropertyEditor>(QT_INSTACE::PROPERTY_EDITOR)->SetContext(nullptr);
}

void ComponentComposer::InitUI()
{
	qtLayout->setSpacing(0);

	auto componentEditor = new QWidget();
	auto layout = new QVBoxLayout();
	layout->setSpacing(3);
	layout->setContentsMargins(0, 3, 0, 0);

	componentEditor->setLayout(layout);

	{
		QPushButton *button = new QPushButton();
		button->setMinimumSize(0, 25);
		button->setText("Add new");
		layout->addWidget(button);

		QObject::connect(button, &QPushButton::clicked, this, &ComponentComposer::AddItem);
	}
	{
		QPushButton *button = new QPushButton();
		button->setMinimumSize(0, 25);
		button->setText("Update");
		layout->addWidget(button);

		QObject::connect(button, &QPushButton::clicked, this, &ComponentComposer::Update);
	}

	qtLayout->addWidget(componentEditor);
}

void ComponentComposer::QtItemClicked(QListWidgetItem * item)
{
	CSoundListEditor::QtItemClicked(item);
	auto data = qtList->GetItemData(item);
	GUI::Get<CSoundPropertyEditor>(QT_INSTACE::PROPERTY_EDITOR)->SetContext(data);
}


ComponentPropertyEntry::ComponentPropertyEntry(CSoundComponentProperty * prop)
	: QTListItem(prop)
{
	setText("");

	auto *label = new QLabel();
	label->setText(prop->GetName());
	label->setMinimumSize(QSize(50, 0));

	input = new QLineEdit();
	input->setText(prop->GetValue());

	// Disable operation entries
	if (op.compare(prop->GetName()) == 0) {
		QPalette *palette = new QPalette();
		palette->setColor(QPalette::Text, Qt::gray);
		input->setPalette(*palette);
		input->setReadOnly(true);
	}

	QObject::connect(input, &QLineEdit::editingFinished, this, &ComponentPropertyEntry::QtItemValueChange);

	auto *layout = new QHBoxLayout();
	layout->setContentsMargins(10, 2, 2, 2);

	layout->addWidget(label);
	layout->addWidget(input);

	widget = new QWidget();
	widget->setLayout(layout);
}

ComponentPropertyEntry::~ComponentPropertyEntry()
{
}

void ComponentPropertyEntry::QtItemValueChange()
{
	if (input->text().compare(appData->GetValue())) {
		appData->SetValue(input->text().toStdString().c_str());
		input->setText(appData->GetValue());
		GUI::Get<InstrumentComposer>(QT_INSTACE::INSTRUMENT_COMPOSER)->Update();
	}
}