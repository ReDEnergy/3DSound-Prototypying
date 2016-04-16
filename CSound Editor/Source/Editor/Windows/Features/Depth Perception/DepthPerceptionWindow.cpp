#include "DepthPerceptionWindow.h"

#include <Editor/Windows/Interface/QtInput.h>

#include <Manager/Manager.h>
#include <Manager/EventSystem.h>

#include <3DWorld/Scripts/SoundModelsConfig.h>

// QT
#include <QPushButton>
#include <QLabel>
#include <QKeyEvent>
#include <QDir>

DepthPerceptionWindow::DepthPerceptionWindow()
{
	LoadStyleSheet("sound-model-config.css");

	setObjectName("DepthPerceptionWindow");
	setWindowTitle("Depth Perception Sound Model");

	SetOnTop();
	InitUI();

	config = new DepthPerceptionConfig();
}

void DepthPerceptionWindow::InitUI()
{
	qtLayout->setSpacing(5);
	qtLayout->setContentsMargins(5, 5, 5, 5);
	qtLayout->setAlignment(Qt::AlignTop);

	startStopButton = new QPushButton();
	startStopButton->setText("Start Test");
	startStopButton->setMinimumHeight(30);
	qtLayout->addWidget(startStopButton);
	QObject::connect(startStopButton, &QPushButton::clicked, this, &DepthPerceptionWindow::ToggleState);


	// ------------------------------------------------------------------------
	// Configuration Panel

	soundGain = new SimpleFloatInput("Gain:", "");
	soundGain->AcceptNegativeValues(false);
	AddWidget(soundGain);

	{
		auto button = new QPushButton();
		button->setText("Reset Configuration");
		button->setMinimumHeight(30);
		AddWidget(button);
		QObject::connect(button, &QPushButton::clicked, this, &DepthPerceptionWindow::ResetConfig);
	}

	// ------------------------------------------------------------------------
	// Init Configuration
	ResetConfig();
}

void DepthPerceptionWindow::Start()
{
	config->soundGain = soundGain->GetValue();
	Manager::GetEvent()->EmitAsync("Start-Depth-Perception", config);
}

void DepthPerceptionWindow::ToggleState()
{
	if (startStopButton->text().compare("Start Test") == 0) {
		startStopButton->setText("Stop Test");
		Start();
	}
	else {
		Manager::GetEvent()->EmitAsync("Stop-Depth-Perception");
		startStopButton->setText("Start Test");
	}
}

void DepthPerceptionWindow::ResetConfig()
{
	soundGain->SetValue(1);
}