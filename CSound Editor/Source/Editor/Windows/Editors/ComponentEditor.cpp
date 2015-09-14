//#include "pchQT.h"
#include "ComponentEditor.h"

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

ComponentEditor::ComponentEditor()
{
	setWindowTitle("Component Editor");
	GUI::Set(QT_INSTACE::COMPONENT_EDITOR, (void*) this);

	InitUI();
}

void ComponentEditor::Update()
{
	//GUI::Get<ScoreEditor>(QT_INSTACE::SCORE_EDITOR)->Update();
	GUI::Get<InstrumentEditor>(QT_INSTACE::INSTRUMENT_EDITOR)->Update();
}

void ComponentEditor::AddItem()
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

void ComponentEditor::ListCleared()
{
	//GUI::Get<PropertyEditor>(QT_INSTACE::PROPERTY_EDITOR)->SetContext(nullptr);
}

void ComponentEditor::InitUI()
{
	auto componentEditor = new QWidget();
	auto layout = new QVBoxLayout();
	componentEditor->setLayout(layout);

	{
		QPushButton *button = new QPushButton();
		button->setMinimumSize(0, 25);
		button->setText("Add new");
		layout->addWidget(button);

		QObject::connect(button, &QPushButton::clicked, this, &ComponentEditor::AddItem);
	}
	{
		QPushButton *button = new QPushButton();
		button->setMinimumSize(0, 25);
		button->setText("Update");
		layout->addWidget(button);

		QObject::connect(button, &QPushButton::clicked, this, &ComponentEditor::Update);
	}

	propertyEditor = new PropertyEditor();

	qtLayout->setDirection(QBoxLayout::Direction::TopToBottom);
	qtLayout->addWidget(componentEditor);
	qtLayout->addWidget(propertyEditor);
}

void ComponentEditor::QtItemClicked(QListWidgetItem * item)
{
	CSoundListEditor::QtItemClicked(item);
	auto data = qtList->GetItemData(item);
	GUI::Get<PropertyEditor>(QT_INSTACE::PROPERTY_EDITOR)->SetContext(data);
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
	cout << input->text().toStdString() << endl;
	if (input->text().compare(appData->GetValue())) {
		appData->SetValue(input->text().toStdString().c_str());
		input->setText(appData->GetValue());
		GUI::Get<InstrumentEditor>(QT_INSTACE::INSTRUMENT_EDITOR)->Update();
	}
}