#include "HrtfTest.h"

#include <Editor/GUI.h>
#include <Editor/QT/Utils.h>
#include <Editor/Windows/Editors/ComponentEditor.h>
#include <3DWorld/Csound/CSound3DSource.h>
#include <3DWorld/Scripts/HrtfRecorder.h>

#include <Csound/CSoundComponentProperty.h>
#include <Csound/CSoundManager.h>
#include <Csound/SoundManager.h>

// Engine
#include <include/glm.h>
#include <Core/GameObject.h>
#include <Core/Camera/Camera.h>
#include <Component/Transform/Transform.h>
#include <Manager/Manager.h>
#include <Manager/SceneManager.h>
#include <UI/ColorPicking/ColorPicking.h>

// QT
#include <QComboBox>
#include <QFlags>
#include <QLayout>
#include <QPushButton>
#include <QStyledItemDelegate>

HrtfTest::HrtfTest()
{
	setWindowTitle("Record HRTF Test");
	gameObj = new CSound3DSource();
	recorder = new HrtfRecorder();
	InitUI();
}

void HrtfTest::InitUI()
{
	setFixedSize(QSize(200, 300));

	qtLayout->setSpacing(5);
	qtLayout->setContentsMargins(5, 5, 5, 5);
	qtLayout->setAlignment(Qt::AlignTop);


	// Clear Scene
	{
		auto *button = new QPushButton();
		button->setText("Start Recording");
		button->setMinimumHeight(30);
		qtLayout->addWidget(button);
		QObject::connect(button, &QPushButton::clicked, this, [this]() {
			recorder->Start();
		});
	}

	{
		auto *button = new QPushButton();
		button->setText("Stop Recording");
		button->setMinimumHeight(30);
		qtLayout->addWidget(button);
		QObject::connect(button, &QPushButton::clicked, this, [this]() {
			recorder->Stop();
		});
	}

	{
		auto zone = new QLabel("Absolute properties");
		zone->setAlignment(Qt::AlignCenter);
		zone->setMargin(5);
		zone->setFont(QFont("Arial", 10, QFont::Bold));
		qtLayout->addWidget(zone);
	}

}

void HrtfTest::Update()
{

}

void HrtfTest::OnEvent(EventType Event, void * data)
{

}
