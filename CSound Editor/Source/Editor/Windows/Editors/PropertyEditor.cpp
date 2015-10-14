#include "PropertyEditor.h"

#include <Editor/GUI.h>
#include <Editor/QT/Utils.h>
#include <Editor/Windows/Editors/ComponentComposer.h>
#include <CSoundEditor.h>

#include <Csound/CSoundComponentProperty.h>
#include <Csound/CSoundManager.h>
#include <Csound/SoundManager.h>

#include <QComboBox>
#include <QLayout>
#include <QPushButton>
#include <QStyledItemDelegate>
#include <QLineEdit>


CSoundPropertyEditor::CSoundPropertyEditor()
{
	setWindowTitle("Property Editor");
	GUI::Set(QT_INSTACE::PROPERTY_EDITOR, (void*) this);
	InitUI();

	context = nullptr;
}

void CSoundPropertyEditor::SetContext(CSoundComponentProperty * prop)
{
	context = prop;
	value->setText(context->GetValue());
	defaultValue->setText(context->GetDefaultValue());
	auto typeIndex = dropdown->findText(context->GetName());
	dropdown->setCurrentIndex(typeIndex);
}

void CSoundPropertyEditor::InitUI()
{
	qtLayout->setMargin(0);
	qtLayout->setAlignment(Qt::AlignTop);

	{
		dropdown = new QComboBox();

		QStyledItemDelegate* itemDelegate = new QStyledItemDelegate();
		dropdown->setItemDelegate(itemDelegate);

		auto widget = Wrap("Type", 40, dropdown);
		qtLayout->addWidget(widget);
	}

	{
		value = new QLineEdit();
		auto widget = Wrap("Value", 40, value);
		qtLayout->addWidget(widget);
	}

	{
		defaultValue = new QLineEdit();
		auto widget = Wrap("Default", 40, defaultValue);
		qtLayout->addWidget(widget);
	}

	{
		QPushButton *button = new QPushButton();
		button->setMinimumSize(0, 25);
		button->setText("Update");
		qtLayout->addWidget(button);

		QObject::connect(button, &QPushButton::clicked, this, &CSoundPropertyEditor::Update);
	}
}

void CSoundPropertyEditor::Update()
{
	if (!context) return;

	context->SetDefault(defaultValue->text().toStdString().c_str());
	context->SetValue(value->text().toStdString().c_str());
	context->SetName(dropdown->currentText().toStdString().c_str());
	auto CE = GUI::Get<ComponentComposer>(QT_INSTACE::COMPONENT_COMPOSER);
	CE->Update();
	CE->SetContext(CE->GetActiveContext());
}

void CSoundPropertyEditor::Init()
{
	auto types = SoundManager::GetCSManager()->GetPropertyTypes();

	for (auto type : types)
	{
		dropdown->addItem(type.c_str());
	}
}