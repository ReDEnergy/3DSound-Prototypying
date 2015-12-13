#include "CSoundOptionsWindow.h"

#include <3DWorld/CSound/CSound3DSource.h>
#include <3DWorld/CSound/CSoundScene.h>

#include <Editor/Windows/Interface/QtInput.h>
#include <CSoundEditor.h>

#include <Manager/Manager.h>
#include <Manager/EventSystem.h>

#include <CSound/CSoundManager.h>
#include <CSound/SoundManager.h>
#include <CSound/CSoundScore.h>

// QT
#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QStyledItemDelegate>

CSoundOptionsWindow::CSoundOptionsWindow()
{
	LoadStyleSheet("csound-options-window.css");
	setWindowTitle("CSound Default Options");
	setObjectName("CSoundOptions");
	SetIcon("gear.png");
	setWindowFlags(Qt::WindowType::CustomizeWindowHint | Qt::WindowStaysOnTopHint | Qt::WindowType::WindowCloseButtonHint);

	testSource = new CSound3DSource();
	testSource->SetSoundModel(SoundManager::GetCSManager()->GetScore("sound-test"));

	InitUI();
}

void CSoundOptionsWindow::InitUI()
{
	qtLayout->setSpacing(5);
	qtLayout->setContentsMargins(5, 5, 5, 5);
	qtLayout->setAlignment(Qt::AlignTop);

	dropdownDevice = new SimpleDropDown("Output Device");
	dropdownDevice->setObjectName("deviceDropdown");
	dropdownDevice->setProperty("dropdownLine", 0);

	auto devices = SoundManager::GetCSManager()->GetOutputDevices();
	unsigned int index = 0;
	for (auto device : devices)
	{
		auto w = new CustomWidget(QBoxLayout::Direction::LeftToRight);
		w->setObjectName("deviceLine");

		auto l = new QLabel((string("[ ") + device->deviceID + " ]\t").c_str());
		l->setProperty("deviceID", "0");
		w->AddWidget(l);

		l = new QLabel((to_string(device->supportedChannels) + " channels").c_str());
		l->setProperty("channels", "0");
		w->AddWidget(l);

		l = new QLabel(device->deviceName);
		l->setProperty("deviceName", "0");
		w->AddWidget(l);

		qtLayout->addWidget(w);
		dropdownDevice->AddOption(device->deviceID, QVariant(index));
		index++;
	}

	dropdownDevice->OnChange([&](QVariant qindex) {
		auto devices = SoundManager::GetCSManager()->GetOutputDevices();
		auto index = qindex.toUInt();
		string outValue("-o ");
		outValue += devices[index]->deviceID;

		// Update default parameters
		SoundManager::GetCSManager()->SetCsOptionsParameter("dac", outValue.c_str());
		SoundManager::GetCSManager()->SetCsInstrumentOption("nchnls", devices[index]->supportedChannels);

		// Also update the second dropdown
		dropdownChannels->SetIndex((devices[index]->supportedChannels / 2) - 1);

		// Update the scene and testing score
		CSoundEditor::GetScene()->UpdateScores();
		testSource->GetScore()->Update();
		testSource->GetScore()->SaveToFile();
		testSource->ReloadScore();
	});

	qtLayout->addWidget(dropdownDevice);

	dropdownChannels = new SimpleDropDown("Output Channels");
	dropdownChannels->setObjectName("channelsDropdown");
	dropdownChannels->setProperty("dropdownLine", 0);
	dropdownChannels->AddOption("2 channels", 2);
	dropdownChannels->AddOption("4 channels", 4);
	dropdownChannels->AddOption("6 channels", 6);
	dropdownChannels->AddOption("8 channels", 8);

	dropdownChannels->OnChange([this](QVariant value) {
		SoundManager::GetCSManager()->SetCsInstrumentOption("nchnls", value.toUInt());
	});

	qtLayout->addWidget(dropdownChannels);


	testAudioButton = new QPushButton();
	testAudioButton->setText("Test Audio");

	QObject::connect(testAudioButton, &QPushButton::pressed, this, [this]() {
		auto state = testSource->TooglePlayback();
		state ? testAudioButton->setText("Stop Test") : testAudioButton->setText("Test Audio");;
	});

	qtLayout->addWidget(testAudioButton);
}
