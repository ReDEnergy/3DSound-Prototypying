#include "MovingPlaneWindow.h"

#include <Editor/Windows/Interface/QtInput.h>

#include <Manager/Manager.h>
#include <Manager/EventSystem.h>

#include <3DWorld/Scripts/SoundModelsConfig.h>

// QT
#include <QPushButton>
#include <QLabel>
#include <QKeyEvent>
#include <QDir>

MovingPlaneWindow::MovingPlaneWindow()
{
	LoadStyleSheet("sound-model-config.css");
	setWindowTitle("Moving Plane Sound Model");
	SetOnTop();
	InitUI();

	config = new MovingPlaneConfig();
}

void MovingPlaneWindow::InitUI()
{
	qtLayout->setSpacing(5);
	qtLayout->setContentsMargins(5, 5, 5, 5);
	qtLayout->setAlignment(Qt::AlignTop);

	startStopButton = new QPushButton();
	startStopButton->setText("Start Test");
	startStopButton->setMinimumHeight(30);
	qtLayout->addWidget(startStopButton);
	QObject::connect(startStopButton, &QPushButton::clicked, this, &MovingPlaneWindow::ToggleState);

	// ------------------------------------------------------------------------
	// Configuration Panel

	travelSpeedInput = new SimpleFloatInput("Travel speed:", "meters/second");
	travelSpeedInput->AcceptNegativeValues(false);
	AddWidget(travelSpeedInput);

	maxDistanceInput = new SimpleFloatInput("Max distance:", "meters");
	maxDistanceInput->AcceptNegativeValues(false);
	maxDistanceInput->OnUserEdit([this](float distance) {
		if (distance == 0)
			maxDistanceInput->SetValue(1);
	});
	AddWidget(maxDistanceInput);

	pauseBetweenScans = new SimpleFloatInput("Pause between scans:", "seconds");
	pauseBetweenScans->AcceptNegativeValues(false);
	AddWidget(pauseBetweenScans);

	tickInterval = new SimpleFloatInput("Tick Interval:", "meters");
	tickInterval->AcceptNegativeValues(false);
	tickInterval->OnUserEdit([this](float value) {
		if (value < 0.5)
			tickInterval->SetValue(0.5);
	});
	AddWidget(tickInterval);

	soundGain = new SimpleFloatInput("Gain:", "");
	soundGain->AcceptNegativeValues(false);
	AddWidget(soundGain);

	{
		auto button = new QPushButton();
		button->setText("Reset Configuration");
		button->setMinimumHeight(30);
		AddWidget(button);
		QObject::connect(button, &QPushButton::clicked, this, &MovingPlaneWindow::ResetConfig);
	}

	// ------------------------------------------------------------------------
	// Init Configuration
	ResetConfig();
}

void MovingPlaneWindow::Start()
{
	config->travelSpeed = travelSpeedInput->GetValue();
	config->pauseBetweenScans = pauseBetweenScans->GetValue();
	config->maxDistanceReach = maxDistanceInput->GetValue();
	config->tickInterval = tickInterval->GetValue();
	config->soundGain = soundGain->GetValue();
	Manager::GetEvent()->EmitAsync("Start-Moving-Plane", config);
}

void MovingPlaneWindow::ResetConfig()
{
	travelSpeedInput->SetValue(2);
	maxDistanceInput->SetValue(5);
	pauseBetweenScans->SetValue(0.5f);
	tickInterval->SetValue(1);
	soundGain->SetValue(2);
}

void MovingPlaneWindow::ToggleState()
{
	if (startStopButton->text().compare("Start Test") == 0) {
		startStopButton->setText("Stop Test");
		Start();
	}
	else {
		Manager::GetEvent()->EmitAsync("Stop-Moving-Plane");
		startStopButton->setText("Start Test");
	}
}