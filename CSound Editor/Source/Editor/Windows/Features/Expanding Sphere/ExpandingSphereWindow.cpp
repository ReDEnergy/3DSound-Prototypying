#include "ExpandingSphereWindow.h"

#include <Editor/Windows/Interface/QtInput.h>

#include <Manager/Manager.h>
#include <Manager/EventSystem.h>

#include <3DWorld/Scripts/SoundModelsConfig.h>

// QT
#include <QPushButton>
#include <QLabel>
#include <QKeyEvent>
#include <QDir>

ExpandingSphereWindow::ExpandingSphereWindow()
{
	LoadStyleSheet("sound-model-config.css");
	setWindowTitle("Expaning Sphere Sound Model");
	SetOnTop();
	InitUI();

	config = new ExpandingSphereConfig();
}

void ExpandingSphereWindow::InitUI()
{
	qtLayout->setSpacing(5);
	qtLayout->setContentsMargins(5, 5, 5, 5);
	qtLayout->setAlignment(Qt::AlignTop);

	startStopButton = new QPushButton();
	startStopButton->setText("Start Test");
	startStopButton->setMinimumHeight(30);
	qtLayout->addWidget(startStopButton);
	QObject::connect(startStopButton, &QPushButton::clicked, this, &ExpandingSphereWindow::ToggleState);

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

	startTickVolume = new SimpleFloatInput("Start scan FX volume:", "", 0);
	startTickVolume->SetValue(100);
	startTickVolume->AcceptNegativeValues(false);
	startTickVolume->OnUserEdit([this](float value) {
		if (value > 100)
			startTickVolume->SetValue(100);
	});
	AddWidget(startTickVolume);

	{
		auto button = new QPushButton();
		button->setText("Reset Configuration");
		button->setMinimumHeight(30);
		AddWidget(button);
		QObject::connect(button, &QPushButton::clicked, this, &ExpandingSphereWindow::ResetConfig);
	}

	// ------------------------------------------------------------------------
	// Init Configuration
	ResetConfig();
}

void ExpandingSphereWindow::Start()
{
	config->travelSpeed = travelSpeedInput->GetValue();
	config->pauseBetweenScans = pauseBetweenScans->GetValue();
	config->maxDistanceReach = maxDistanceInput->GetValue();
	config->startScanFXVolume = startTickVolume->GetValue();
	Manager::GetEvent()->EmitAsync("Start-Expanding-Sphere", config);
}

void ExpandingSphereWindow::ToggleState()
{
	if (startStopButton->text().compare("Start Test") == 0) {
		startStopButton->setText("Stop Test");
		Start();
	}
	else {
		Manager::GetEvent()->EmitAsync("Stop-Expanding-Sphere");
		startStopButton->setText("Start Test");
	}
}

void ExpandingSphereWindow::ResetConfig()
{
	travelSpeedInput->SetValue(2);
	pauseBetweenScans->SetValue(0.5);
	maxDistanceInput->SetValue(5);
}