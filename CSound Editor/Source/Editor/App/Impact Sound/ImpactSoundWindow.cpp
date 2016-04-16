#include "ImpactSoundWindow.h"

#include <3DWorld/Scripts/SoundModelsConfig.h>

#include <Manager/Manager.h>
#include <Manager/EventSystem.h>

#include <Editor/Windows/Interface/QtInput.h>

// QT
#include <QPushButton>
#include <QLabel>
#include <QKeyEvent>
#include <QDir>

ImpactSoundWindow::ImpactSoundWindow()
{
	LoadStyleSheet("sound-model-config.css");
	setWindowTitle("Impact Sound Model");
	SetOnTop();
	InitUI();

	config = new ImpactSoundConfig();
}

void ImpactSoundWindow::InitUI()
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
				Manager::GetEvent()->EmitAsync("Stop-Impact-Sound");
				button->setText("Start Test");
			}
		});
	}

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
		QObject::connect(button, &QPushButton::clicked, this, &ImpactSoundWindow::ResetConfig);
	}

	// ------------------------------------------------------------------------
	// Init Configuration
	ResetConfig();
}

void ImpactSoundWindow::Start()
{
	config->soundGain = soundGain->GetValue();
	Manager::GetEvent()->EmitAsync("Start-Impact-Sound", config);
}

void ImpactSoundWindow::Stop()
{
}

void ImpactSoundWindow::ResetConfig()
{
	soundGain->SetValue(2);
}

void ImpactSoundWindow::OnEvent(const string & eventID, void * data)
{
}