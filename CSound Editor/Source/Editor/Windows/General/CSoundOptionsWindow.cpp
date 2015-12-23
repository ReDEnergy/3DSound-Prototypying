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
#include <CSound/CSoundInstrument.h>
#include <CSound/CSoundInstrumentBlock.h>
#include <CSound/CSoundComponent.h>

// QT
#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QStyledItemDelegate>

CSoundOptionsWindow::CSoundOptionsWindow()
{
	activeTestChannel = -1;
	sourceChannelForMapping = 0;

	LoadStyleSheet("csound-options-window.css");
	setWindowTitle("CSound Default Options");
	setObjectName("CSoundOptions");
	SetIcon("gear.png");
	setWindowFlags(Qt::WindowType::CustomizeWindowHint | Qt::WindowStaysOnTopHint | Qt::WindowType::WindowCloseButtonHint);

	InitUI();

	testSource = new CSound3DSource();
	auto score = SoundManager::GetCSManager()->GetScore("sound-test");
	score->GetEntry("sound-test")->UseGlobalOutput(false);
	testSource->SetSoundModel(score, false);

	auto activeDacID = SoundManager::GetCSManager()->GetActiveDacID();
	UpdateChannelDropdown(activeDacID);
	LoadDeviceSettings(activeDacID);
	SaveDevicesConfiguration();
}

void CSoundOptionsWindow::InitUI()
{
	qtLayout->setSpacing(5);
	qtLayout->setContentsMargins(5, 5, 5, 5);
	qtLayout->setAlignment(Qt::AlignTop);
	qtLayout->setDirection(QBoxLayout::Direction::LeftToRight);

	// ---------------------------------------------------------------------------
	// Devices Panel

	dropdownDevice = new SimpleDropDown("Output Device");
	dropdownDevice->setObjectName("deviceDropdown");

	auto devicesPanel = new CustomWidget();
	devicesPanel->setObjectName("DevicesPanel");
	devicesPanel->setContentsMargins(5, 5, 5, 5);

	auto title = new QLabel("Audio devices");
	title->setAlignment(Qt::AlignCenter);
	title->setObjectName("title");
	devicesPanel->AddWidget(title);

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

		devicesPanel->AddWidget(w);
		dropdownDevice->AddOption(device->deviceID, QVariant(index));
		index++;
	}

	auto selectionZone = new CustomWidget();
	selectionZone->setObjectName("DeviceSelection");
	selectionZone->setContentsMargins(0, 10, 0, 10);

	dropdownDevice->SetIndex(SoundManager::GetCSManager()->GetActiveDacID());
	dropdownDevice->OnChange([this](QVariant qindex) {
		UpdateChannelDropdown(qindex.toUInt());
		LoadDeviceSettings(qindex.toUInt());
	});

	selectionZone->AddWidget(dropdownDevice);

	dropdownChannels = new SimpleDropDown("Output Channels");
	dropdownChannels->setObjectName("channelsDropdown");

	dropdownChannels->OnChange([this](QVariant value) {
		selectedDevice->nrActiveChannels = value.toUInt();
		LoadDeviceSettings(selectedDevice->index);
	});


	selectionZone->AddWidget(dropdownChannels);
	devicesPanel->AddWidget(selectionZone);

	saveButton = new QPushButton();
	saveButton->setText("Save Configuration");

	QObject::connect(saveButton, &QPushButton::pressed, this, [this]() {
		SaveDevicesConfiguration();
	});

	devicesPanel->AddWidget(saveButton);
	qtLayout->addWidget(devicesPanel);


	// ---------------------------------------------------------------------------
	// Channel Mapper

	configPanel = new CustomWidget();
	configPanel->setObjectName("DeviceConfigPanel");
	configPanel->setContentsMargins(5, 5, 5, 5);

	int selectedChannel = 0;

	configPanelTitle = new QLabel("Channel mapping");
	configPanelTitle->setAlignment(Qt::AlignCenter);
	configPanelTitle->setObjectName("title");
	configPanel->AddWidget(configPanelTitle);

	configPanelChannelsBox = new CustomWidget();

	auto channels = SoundManager::GetDefaultChannelNames();
	//mappedChannelsButtons[channelID]->setText(channels[channelTarget].c_str());


	for (int i = 0; i < 8; i++)
	{
		auto w = new CustomWidget(QBoxLayout::Direction::LeftToRight);
		w->setObjectName("channelMapLine");

		auto l = new QPushButton("Play");
		l->setProperty("playback", "false");
		w->AddWidget(l);
		QObject::connect(l, &QPushButton::pressed, this, [this, i]() {
			TestChannel(i);
		});
		testButtons.push_back(l);

		auto label = new QLabel(to_string(i + 1).c_str());
		label->setAlignment(Qt::AlignCenter);
		label->setProperty("channelID", "");
		label->setProperty("channelColor", to_string(i).c_str());
		w->AddWidget(label);

		l = new QPushButton(channels[i].c_str());
		l->setProperty("channelColor", to_string(i).c_str());
		w->AddWidget(l);

		QObject::connect(l, &QPushButton::pressed, this, [this, i]() {
			SetSourceChannelForMapping(i);
		});

		l = new QPushButton();
		w->AddWidget(l);

		QObject::connect(l, &QPushButton::pressed, this, [this, i]() {
			CSoundOptionsWindow::MapSourceChannelTo(i);
		});
		mappedChannelsButtons.push_back(l);

		l = new QPushButton("Play");
		l->setProperty("playback", "false");
		w->AddWidget(l);
		QObject::connect(l, &QPushButton::pressed, this, [this, i]() {
			TestChannel(selectedDevice->mapping[i]);
		});
		testButtonsMap.push_back(l);

		channelBoxes.push_back(w);
		configPanelChannelsBox->AddWidget(w);
	}

	configPanel->AddWidget(configPanelChannelsBox);
	qtLayout->addWidget(configPanel);
}


void CSoundOptionsWindow::SetSourceChannelForMapping(uint channelID)
{
	sourceChannelForMapping = channelID;
}

void CSoundOptionsWindow::MapSourceChannelTo(uint channelID)
{
	selectedDevice->changed = true;
	selectedDevice->mapping[channelID] = sourceChannelForMapping;
	UpdateChannelButton(channelID, sourceChannelForMapping);
	SoundManager::GetCSManager()->SaveConfigFile();
}

void CSoundOptionsWindow::UpdateChannelButton(uint channelID, uint channelTarget)
{
	auto channels = SoundManager::GetDefaultChannelNames();

	mappedChannelsButtons[channelID]->setProperty("channelColor", to_string(channelTarget).c_str());
	mappedChannelsButtons[channelID]->setText(channels[channelTarget].c_str());

	style()->unpolish(mappedChannelsButtons[channelID]);
	mappedChannelsButtons[channelID]->ensurePolished();
}

void CSoundOptionsWindow::LoadDeviceSettings(uint deviceID)
{
	auto CSoundManager = SoundManager::GetCSManager();
	auto devices = CSoundManager->GetOutputDevices();

	selectedDevice = devices[deviceID];

	// Update the second dropdown
	dropdownChannels->StopCallbacks();
	dropdownChannels->SetIndex(selectedDevice->nrActiveChannels - 1);
	dropdownChannels->ResumeCallbacks();

	// Update Configuration Panel
	configPanelTitle->setText(("Device " + to_string(deviceID) + " - output mapping").c_str());

	for (int i = 0; i < selectedDevice->supportedChannels; i++)
	{
		channelBoxes[i]->show();
		UpdateChannelButton(i, selectedDevice->mapping[i]);
	}

	for (int i = selectedDevice->supportedChannels; i < 8; i++) {
		channelBoxes[i]->Hide();
	}

	UpdateTestScore();
}

void CSoundOptionsWindow::UpdateChannelDropdown(uint deviceID)
{
	auto devices = SoundManager::GetCSManager()->GetOutputDevices();
	auto device = devices[deviceID];

	dropdownChannels->RemoveOptions();
	for (int i = 0; i < device->supportedChannels; i++) {
		dropdownChannels->AddOption((to_string(i + 1) + " channels").c_str(), i + 1);
	}
	dropdownChannels->ResumeCallbacks();
}

void CSoundOptionsWindow::UpdateScene()
{
	// Update the scene and testing score
	CSoundEditor::GetScene()->TriggerCSoundRebuild();
}

void CSoundOptionsWindow::UpdateTestScore()
{
	auto CSoundManager = SoundManager::GetCSManager();
	auto dacID = CSoundManager->GetActiveDacID();
	auto nrChannels = CSoundManager->GetInstrumentOption("nchnls");

	CSoundManager->SetActiveDac(selectedDevice->index);
	CSoundManager->SetCsInstrumentOption("nchnls", selectedDevice->supportedChannels);

	testSource->GetScore()->Update();
	testSource->GetScore()->SaveToFile();
	testSource->ReloadScore();

	CSoundManager->SetActiveDac(dacID);
	CSoundManager->SetCsInstrumentOption("nchnls", nrChannels);
}

void CSoundOptionsWindow::SaveDevicesConfiguration()
{
	auto CSoundManager = SoundManager::GetCSManager();
	CSoundManager->SetActiveDac(selectedDevice->index);
	CSoundManager->SetCsInstrumentOption("nchnls", selectedDevice->supportedChannels);

	auto block = SoundManager::GetGlobalOutputModel();
	auto component = block->GetEntry("global-output");

	string componentName = "output" + to_string(selectedDevice->nrActiveChannels);
	auto entry = component->GetEntry(componentName.c_str());
	if (entry) {
		auto val = SoundManager::GetChannelMapping(selectedDevice, selectedDevice->nrActiveChannels);
		entry->SetValue(val);
	}
	UpdateScene();
}

void CSoundOptionsWindow::TestChannel(uint channelID)
{
	if (activeTestChannel >= 0) {
		testButtons[activeTestChannel]->setText("Play");
		testButtons[activeTestChannel]->setProperty("playback", "false");
		style()->unpolish(testButtons[activeTestChannel]);
		testButtons[activeTestChannel]->ensurePolished();
	}

	if (channelID == activeTestChannel) {
		testSource->StopScore();
		activeTestChannel = -1;
		return;
	}

	activeTestChannel = channelID;
	testSource->PlayScore();
	testSource->SetControlChannel("kChannel", channelID + 1, true);
	testButtons[channelID]->setText("Stop");
	testButtons[channelID]->setProperty("playback", "true");
	style()->unpolish(testButtons[channelID]);
	testButtons[channelID]->ensurePolished();
}

void CSoundOptionsWindow::hideEvent(QHideEvent * event)
{
	if (activeTestChannel >= 0) {
		testButtons[activeTestChannel]->setText("Play");
		testButtons[activeTestChannel]->setProperty("playback", "false");
		style()->unpolish(testButtons[activeTestChannel]);
		testButtons[activeTestChannel]->ensurePolished();
	}

	testSource->StopScore();
}
