#include "ComponentEditor.h"

#include <templates/singleton.h>

#include <Editor/GUI.h>
#include <Editor/QT/Utils.h>
#include <Editor/Windows/Editors/PropertyEditor.h>
#include <Editor/Windows/Editors/ComponentComposer.h>
#include <CSoundEditor.h>

#include <Csound/CSoundComponentProperty.h>
#include <Csound/CSoundManager.h>
#include <Csound/SoundManager.h>

#include <QtWidgets/QComboBox>
#include <QtWidgets/QLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStyledItemDelegate>


ComponentEditor::ComponentEditor()
{
	setWindowTitle("Component Editor");
	InitUI();
}

void ComponentEditor::InitUI()
{
	qtLayout->setMargin(0);
	qtLayout->setSpacing(2);
	qtLayout->setAlignment(Qt::AlignTop);
	qtLayout->setDirection(QBoxLayout::LeftToRight);

	propertyEditor = Singleton<CSoundPropertyEditor>::Instance();
	composer = Singleton<ComponentComposer>::Instance();

	qtLayout->addWidget(composer);
	qtLayout->addWidget(propertyEditor);
}

void ComponentEditor::Init()
{
	propertyEditor->Init();
}