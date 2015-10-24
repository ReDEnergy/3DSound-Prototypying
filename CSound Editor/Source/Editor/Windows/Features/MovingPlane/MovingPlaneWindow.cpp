#include "MovingPlaneWindow.h"

#include <Editor/Windows/Interface/QtInput.h>

#include <Manager/Manager.h>
#include <Manager/EventSystem.h>

// QT
#include <QPushButton>
#include <QLabel>
#include <QKeyEvent>
#include <QDir>

MovingPlaneWindow::MovingPlaneWindow()
{
	LoadStyleSheet("moving-plane.qss");
	setWindowTitle("Moving Plane Sound Model");
	InitUI();

	config = new MovingPlaneConfig();
}

void MovingPlaneWindow::InitUI()
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
				Manager::GetEvent()->EmitAsync("Stop-Moving-Plane");
				button->setText("Start Test");
			}
		});
	}

	// ------------------------------------------------------------------------
	// Configuration Panel

	planeTravelSpeed = new SimpleFloatInput("Travel speed:", "meters/second");
	planeTravelSpeed->AcceptNegativeValues(false);
	//planeTravelSpeed->OnUserEdit([this](float speed) {
	//	if (speed == 0)
	//		planeTravelSpeed->SetValue(1);
	//});
	AddWidget(planeTravelSpeed);

	planeMaxDistance = new SimpleFloatInput("Plane max distance:", "meters");
	planeMaxDistance->AcceptNegativeValues(false);
	planeMaxDistance->OnUserEdit([this](float distance) {
		if (distance == 0)
			planeMaxDistance->SetValue(1);
	});
	AddWidget(planeMaxDistance);

	pauseBetweenScans = new SimpleFloatInput("Pause between scans:", "seconds");
	pauseBetweenScans->AcceptNegativeValues(false);
	AddWidget(pauseBetweenScans);

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
	config->travelSpeed = planeTravelSpeed->GetValue();
	config->pauseBetweenScans = pauseBetweenScans->GetValue();
	config->maxDistanceReach = planeMaxDistance->GetValue();
	Manager::GetEvent()->EmitAsync("Start-Moving-Plane", config);
}

void MovingPlaneWindow::Stop()
{
}

void MovingPlaneWindow::ResetConfig()
{
	planeTravelSpeed->SetValue(5);
	pauseBetweenScans->SetValue(0.5);
	planeMaxDistance->SetValue(10);
}

void MovingPlaneWindow::OnEvent(const string & eventID, void * data)
{
}