#include "PropertyEditor.h"

#include <Editor/GUI.h>
#include <Editor/QT/Utils.h>
#include <Editor/Windows/Editors/ComponentEditor.h>

#include <Csound/CSoundComponentProperty.h>
#include <Csound/CSoundManager.h>
#include <Csound/SoundManager.h>

#include <QtWidgets/QComboBox>
#include <QtWidgets/QLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStyledItemDelegate>


PropertyEditor::PropertyEditor()
{
	// setWindowTitle("Property Editor");
	GUI::Set(QT_INSTACE::PROPERTY_EDITOR, (void*) this);
	InitUI();
}

void PropertyEditor::SetContext(CSoundComponentProperty * prop)
{
	context = prop;
	value->setText(context->GetValue());
	defaultValue->setText(context->GetDefaultValue());
	auto typeIndex = dropdown->findText(context->GetName());
	dropdown->setCurrentIndex(typeIndex);
}

void PropertyEditor::InitUI()
{
	LoadStyleSheet("Resources/StyleSheets/stylesheet.qss");
	qtLayout->setMargin(5);
	qtLayout->setAlignment(Qt::AlignTop);

	{
		dropdown = new QComboBox();

		QStyledItemDelegate* itemDelegate = new QStyledItemDelegate();
		dropdown->setItemDelegate(itemDelegate);

		auto widget = Wrap("Type", 50, dropdown);
		qtLayout->addWidget(widget);
	}

	{
		value = new QLineEdit();
		auto widget = Wrap("Value", 50, value);
		qtLayout->addWidget(widget);
	}

	{
		defaultValue = new QLineEdit();
		auto widget = Wrap("Default", 50, defaultValue);
		qtLayout->addWidget(widget);
	}

	{
		QPushButton *button = new QPushButton();
		button->setMinimumSize(0, 25);
		button->setText("Update");
		qtLayout->addWidget(button);

		QObject::connect(button, &QPushButton::clicked, this, &PropertyEditor::Update);
	}
}

void PropertyEditor::Update()
{
	if (!context) return;

	context->SetDefault(defaultValue->text().toStdString().c_str());
	context->SetValue(value->text().toStdString().c_str());
	context->SetName(dropdown->currentText().toStdString().c_str());
	auto CE = GUI::Get<ComponentEditor>(QT_INSTACE::COMPONENT_EDITOR);
	CE->Update();
	CE->SetContext(CE->GetActiveContext());
}

void PropertyEditor::Init()
{
	auto types = SoundManager::GetCSManager()->GetPropertyTypes();

	for (auto type : types)
	{
		dropdown->addItem(type.c_str());
	}
}