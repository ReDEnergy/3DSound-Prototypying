#include "SweepingPlaneWindow.h"

#include <Editor/Windows/Interface/QtInput.h>

#include <Manager/Manager.h>
#include <Manager/EventSystem.h>

#include <3DWorld/Scripts/SoundModelsConfig.h>

// QT
#include <QPushButton>
#include <QLabel>
#include <QKeyEvent>
#include <QDir>

SweepingPlaneWindow::SweepingPlaneWindow()
{
	LoadStyleSheet("sound-model-config.css");
	setWindowTitle("Sweeping Plane Sound Model");
	SetOnTop();
	InitUI();

	config = new SweepingPlaneConfig();
}

void SweepingPlaneWindow::InitUI()
{
	qtLayout->setSpacing(5);
	qtLayout->setContentsMargins(5, 5, 5, 5);
	qtLayout->setAlignment(Qt::AlignTop);

	{
		bool started = false;
		auto *button = new QPushButton();
		button->setText("Start Test");
		button->setMinimumHeight(30);
		qtLayout->addWidget(button);
		QObject::connect(button, &QPushButton::clicked, this, [button, this]() {
			if (button->text().compare("Start Test") == 0) {
				button->setText("Stop Test");
				Start();
			}
			else {
				Manager::GetEvent()->EmitAsync("Stop-Sweeping-Plane");
				button->setText("Start Test");
			}
		});
	}

	// ------------------------------------------------------------------------
	// Configuration Panel

	scanTime = new SimpleFloatInput("Travel time:", "seconds");
	scanTime->AcceptNegativeValues(false);
	AddWidget(scanTime);

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

	tickInterval = new SimpleFloatInput("Tick Interval:", "degrees");
	tickInterval->AcceptNegativeValues(false);
	tickInterval->OnUserEdit([this](float value) {
		if (value < 5)
			tickVolume->SetValue(5);
	});
	AddWidget(tickInterval);

	tickVolume = new SimpleFloatInput("Tick volume:", "");
	tickVolume->AcceptNegativeValues(false);
	tickVolume->OnUserEdit([this](float value) {
		if (value > 2)
			tickVolume->SetValue(2);
	});
	AddWidget(tickVolume);

	soundGain = new SimpleFloatInput("Gain:", "");
	soundGain->AcceptNegativeValues(false);
	AddWidget(soundGain);

	{
		auto button = new QPushButton();
		button->setText("Reset Configuration");
		button->setMinimumHeight(30);
		AddWidget(button);
		QObject::connect(button, &QPushButton::clicked, this, &SweepingPlaneWindow::ResetConfig);
	}

	// ------------------------------------------------------------------------
	// Init Configuration
	ResetConfig();
}

void SweepingPlaneWindow::Start()
{
	config->fullScanTime = scanTime->GetValue();
	config->pauseBetweenScans = pauseBetweenScans->GetValue();
	config->maxDistanceReach = maxDistanceInput->GetValue();
	config->tickInterval = tickInterval->GetValue();
	config->tickVolume = tickVolume->GetValue();
	config->soundGain = soundGain->GetValue();
	Manager::GetEvent()->EmitAsync("Start-Sweeping-Plane", config);
}

void SweepingPlaneWindow::Stop()
{
}

void SweepingPlaneWindow::ResetConfig()
{
	scanTime->SetValue(1.5f);
	maxDistanceInput->SetValue(5);
	pauseBetweenScans->SetValue(0.5f);
	tickInterval->SetValue(15.0f);
	tickVolume->SetValue(0.25f);
	soundGain->SetValue(2);
}

void SweepingPlaneWindow::OnEvent(const string & eventID, void * data)
{
}