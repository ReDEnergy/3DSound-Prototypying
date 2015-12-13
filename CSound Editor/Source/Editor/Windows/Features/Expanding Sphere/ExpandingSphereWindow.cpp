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
	LoadStyleSheet("moving-plane.css");
	setWindowTitle("Expaning Sphere Sound Model");
	InitUI();

	config = new ExpandingSphereConfig();
}

void ExpandingSphereWindow::InitUI()
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
				Manager::GetEvent()->EmitAsync("Stop-Expanding-Sphere");
				button->setText("Start Test");
			}
		});
	}

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

void ExpandingSphereWindow::Stop()
{
}

void ExpandingSphereWindow::ResetConfig()
{
	travelSpeedInput->SetValue(5);
	pauseBetweenScans->SetValue(0.5);
	maxDistanceInput->SetValue(10);
}

void ExpandingSphereWindow::OnEvent(const string & eventID, void * data)
{
}