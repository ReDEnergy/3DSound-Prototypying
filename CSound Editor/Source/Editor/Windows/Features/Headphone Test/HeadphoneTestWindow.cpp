#include "HeadphoneTestWindow.h"

#include <Editor/Windows/Interface/QtInput.h>
#include <Editor/Windows/Interface/QtSortableInput.h>
#include <Editor/Windows/Features/Headphone Test/HeadphoneTestGenerator.h>
#include <Editor/Windows/Features/Headphone Test/HeadphoneTestAnswerPanel.h>
#include <3DWorld/Scripts/HeadphoneTestScript.h>

#include <Manager/Manager.h>
#include <Manager/EventSystem.h>

#include <Utils/utils.h>

// QT
#include <QPushButton>
#include <QLabel>
#include <QKeyEvent>
#include <QPlainTextEdit>
#include <QDir>

static HeadphoneTestScript *recorder;

HeadphoneTestWindow::HeadphoneTestWindow()
{
	LoadStyleSheet("headphone-test.css");
	setWindowTitle("Headphone Test");

	config = new HeadphoneTestConfig();
	recorder = new HeadphoneTestScript();

	SubscribeToEvent("HRTF-test-end");

	// Default Test
	customTests.push_back(new HeadphoneTestConfig());

	{
		auto *test = new HeadphoneTestConfig();
		test->azimuthValues = { -60, 0, 60 };
		test->elevationValues = { -60, 0, 60 };
		test->randomIterations = 4;
		customTests.push_back(test);
	}

	{
		auto test = new HeadphoneTestConfig();
		test->azimuthValues = { -90, -60, -30, 0, 30, 60, 90 };
		test->elevationValues = { 0 };
		test->randomIterations = 4;
		customTests.push_back(test);
	}

	{
		auto test = new HeadphoneTestConfig();
		test->azimuthValues = { -90, -60, -30, 0, 30, 60, 90 };
		test->elevationValues = { -60, -30, 0, 30, 60 };
		test->randomIterations = 4;
		customTests.push_back(test);
	}

	testStarted = false;

	InitUI();
}

void HeadphoneTestWindow::InitUI()
{
	qtLayout->setSpacing(5);
	qtLayout->setContentsMargins(5, 5, 5, 5);
	qtLayout->setAlignment(Qt::AlignTop);
	qtLayout->setDirection(QBoxLayout::LeftToRight);

	answerPanel = new HeadphoneTestAnswerPanel();
	answerPanel->OnButtonClick([](float azimuth, float elevation) {
		recorder->VerifyAnswer(azimuth, elevation);
	});

	sampleGenerator = new HeadphoneTestGenerator();

	sectionLeft = new CustomWidget(QBoxLayout::TopToBottom);
	sectionLeft->setObjectName("sectionLeft");
	sectionLeft->setContentsMargins(5, 5, 5, 5);

	{
		buttonStartStop = new QPushButton();
		buttonStartStop->setText("Start Test");
		buttonStartStop->setMinimumHeight(30);
		sectionLeft->AddWidget(buttonStartStop);
		QObject::connect(buttonStartStop, &QPushButton::clicked, this, [this]() {
			testStarted ? Manager::GetEvent()->EmitAsync("stop-HRTF-test") : Start();
		});
	}

	// ------------------------------------------------------------------------
	// Help Info

	{
		auto info = new QLabel("Keyboard keys [Numpad]");
		info->setMinimumHeight(30);
		info->setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Fixed);
		info->setAlignment(Qt::AlignCenter);
		info->setMargin(5);
		info->setFont(QFont("Arial", 10, QFont::Bold));
		sectionLeft->AddWidget(info);

		auto block = new QPlainTextEdit();
		block->setFont(QFont("Times New Roman", 10));
		block->setReadOnly(true);
		block->insertPlainText(" 4, 8  - chnage azimuth left/right\n");
		block->insertPlainText(" 2, 8  - chnage elevation down/up\n");
		block->insertPlainText(" 5      - center azimuth, elevation to [0, 0]\n");
		block->insertPlainText(" Enter - submit answer");
		block->setFixedHeight(75);
		sectionLeft->AddWidget(block);

		azimuthAnswer = new SimpleFloatInput("Response azimuth:", "deg", 2, true);
		elevationAnswer = new SimpleFloatInput("Response elevation:", "deg", 2, true);

		auto wrap = new CustomWidget();
		wrap->setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Fixed);
		wrap->layout()->setContentsMargins(0, 20, 0, 20);
		wrap->AddWidget(azimuthAnswer);
		wrap->AddWidget(elevationAnswer);
		sectionLeft->AddWidget(wrap);
	}

	// ------------------------------------------------------------------------
	// Configuration Panel

	generalConfigPanel = new CustomWidget();
	generalConfigPanel->setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Fixed);

	{
		auto zone = new QLabel("Configure Test");
		zone->setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Fixed);
		zone->setAlignment(Qt::AlignCenter);
		zone->setFont(QFont("Arial", 10, QFont::Bold));
		generalConfigPanel->AddWidget(zone);
	}

	{
		auto W = new CustomWidget(QBoxLayout::LeftToRight);
		W->layout()->setSpacing(0);
		W->setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Fixed);

		auto label = new QLabel("Test Name: ");
		label->setMinimumWidth(95);
		testName = new QLineEdit();
		W->AddWidget(label);
		W->AddWidget(testName);
		generalConfigPanel->AddWidget(W);
	}

	auto dropdown = new SimpleDropDown("Custom test");
	dropdown->SetLabelWidth(100);
	dropdown->AddOption("Test 1", QVariant(1));
	dropdown->AddOption("Test 2", QVariant(2));
	dropdown->AddOption("Test 3", QVariant(3));
	dropdown->OnChange([&](QVariant value) {
		SetConfig(customTests[value.toUInt()]);
	});
	generalConfigPanel->AddWidget(dropdown);


	showAdvancedConfig = new SimpleCheckBox("Advanced mode:", false);
	showAdvancedConfig->setContentsMargins(0, 0, 0, 10);
	showAdvancedConfig->SetLabelWidth(100);
	showAdvancedConfig->OnUserEdit([this](bool value) {
		if (value) {
			AddWidget(configArea);
		}
		else {
			configArea->DetachFromParent();
			adjustSize();
		}
	});
	generalConfigPanel->AddWidget(showAdvancedConfig);

	sectionLeft->AddWidget(generalConfigPanel);
	AddWidget(sectionLeft);

	// ------------------------------------------------------------------------
	// Advanced Configurations

	// All configuration options will be display inside the config area
	configArea = new CustomWidget(QBoxLayout::Direction::LeftToRight);
	configArea->setObjectName("configArea");

	advanceConfigPanel = new CustomWidget();
	advanceConfigPanel->setObjectName("advancedConfigPanel");
	advanceConfigPanel->setContentsMargins(5, 5, 5, 5);

	randomValues = new SimpleCheckBox("Random Values:", true);
	randomValues->SetLabelWidth(100);
	randomValues->OnUserEdit([this](bool value) {
		if (value) {
			sampleGenerator->DetachFromParent();
			configArea->adjustSize();
			adjustSize();
		}
		else {
			configArea->AddWidget(sampleGenerator);
		}
	});
	advanceConfigPanel->AddWidget(randomValues);

	wait4Input = new SimpleCheckBox("Wait for user input:", true);
	wait4Input->SetLabelWidth(100);
	advanceConfigPanel->AddWidget(wait4Input);

	randomIterations = new SimpleFloatInput("Random iterations:", "iterations", 0);
	randomIterations->AcceptNegativeValues(false);
	randomIterations->OnUserEdit([this](float value) {
		if (value == 0)
			randomIterations->SetValue(1);
	});
	advanceConfigPanel->AddWidget(randomIterations);

	// Test output models

	testHRTF = new SimpleCheckBox("Test HRTF:", true);
	testHRTF->SetLabelWidth(100);
	advanceConfigPanel->AddWidget(testHRTF);

	testIndividualHRTF = new SimpleCheckBox("Individual HRTF:", true);
	testIndividualHRTF->SetLabelWidth(100);
	advanceConfigPanel->AddWidget(testIndividualHRTF);

	testStereoPanning = new SimpleCheckBox("Stereo Panning:", true);
	testStereoPanning->SetLabelWidth(100);
	advanceConfigPanel->AddWidget(testStereoPanning);

	// Time config

	prepareTime = new SimpleFloatInput("Prepare time:", "sec");
	prepareTime->AcceptNegativeValues(false);
	advanceConfigPanel->AddWidget(prepareTime);

	sampleDuration = new SimpleFloatInput("Sample duration:", "sec");
	sampleDuration->AcceptNegativeValues(false);
	advanceConfigPanel->AddWidget(sampleDuration);

	sampleInterval = new SimpleFloatInput("Sample interval:", "sec");
	sampleInterval->AcceptNegativeValues(false);
	advanceConfigPanel->AddWidget(sampleInterval);

	// Values config

	sortableAzimuth = new QtSortableInput("Azimut values:", 0);
	sortableAzimuth->OnUserEdit([this](const vector<float>& vec) {
		sampleGenerator->SetupGenerator(vec, sortableElevation->GetValues());
	});
	advanceConfigPanel->AddWidget(sortableAzimuth);

	sortableElevation = new QtSortableInput("Elevation values:", 0);
	sortableElevation->OnUserEdit([this](const vector<float>& vec) {
		sampleGenerator->SetupGenerator(sortableAzimuth->GetValues(), vec);
	});
	advanceConfigPanel->AddWidget(sortableElevation);

	{
		auto *button = new QPushButton();
		button->setText("Reset Configuration");
		button->setMinimumHeight(30);
		advanceConfigPanel->AddWidget(button);
		QObject::connect(button, &QPushButton::clicked, this, &HeadphoneTestWindow::ResetConfig);
	}

	configArea->AddWidget(advanceConfigPanel);

	// ------------------------------------------------------------------------
	// Init Configuration
	SetConfig(customTests[1]);
}

void HeadphoneTestWindow::Start()
{
	if (randomValues->GetValue() == false && sampleGenerator->GetNumberOfSamples() == 0)
	{
		cout << "Please add at least 1 sample" << endl;
		Stop();
		return;
	}

	config->outputTested[1] = testHRTF->GetValue();
	config->outputTested[2] = testStereoPanning->GetValue();
	config->outputTested[3] = testIndividualHRTF->GetValue();
	if (!(config->outputTested[1] || config->outputTested[2] || config->outputTested[3])) {
		Stop();
		return;
	}

	bool exist = QDir("Headphone Test Results").exists();
	if (!exist) {
		QDir().mkdir("Headphone Test Results");
	}

	testStarted = true;

	config->randomTest = randomValues->GetValue();
	config->prepareTime = prepareTime->GetValue();
	config->randomIterations = randomIterations->GetValue();
	config->samplePlaybackDuration = sampleDuration->GetValue();
	config->sampleInterval = sampleInterval->GetValue();
	config->waitForInput = wait4Input->GetValue();
	config->testName = testName->text().trimmed().toStdString();

	if (config->randomTest)
	{
		config->azimuthValues = sortableAzimuth->GetValues();
		config->elevationValues = sortableElevation->GetValues();
	}
	else {	  
		config->azimuthValues = sampleGenerator->GetAzimuthValues();
		config->elevationValues = sampleGenerator->GetElevationValues();
	}

	answerPanel->SetupAnswerPanel(sortableAzimuth->GetValues(), sortableElevation->GetValues());

	Manager::GetEvent()->EmitAsync("start-HRTF-test", (void*)config);

	buttonStartStop->setText("Stop Test");
	configArea->DetachFromParent();
	generalConfigPanel->DetachFromParent();
	AddWidget(answerPanel);
	ResetAnswerKeyOffset();
}

void HeadphoneTestWindow::Stop()
{
	testStarted = false;
	if (showAdvancedConfig->GetValue())
	{
		AddWidget(configArea);
	}
	sectionLeft->AddWidget(generalConfigPanel);
	answerPanel->DetachFromParent();
	buttonStartStop->setText("Start Test");
	adjustSize();
}

void HeadphoneTestWindow::SetConfig(HeadphoneTestConfig * config)
{
	randomValues->SetValue(config->randomTest);
	prepareTime->SetValue(config->prepareTime);
	sampleDuration->SetValue(config->samplePlaybackDuration);
	sampleInterval->SetValue(config->sampleInterval);
	randomIterations->SetValue(config->randomIterations);
	wait4Input->SetValue(config->waitForInput);

	auto value = Utils::Join(config->azimuthValues);
	sortableAzimuth->SetValue(value.c_str());

	value = Utils::Join(config->elevationValues);
	sortableElevation->SetValue(value.c_str());
}

void HeadphoneTestWindow::ResetConfig()
{
	HeadphoneTestConfig cfg;
	SetConfig(&cfg);
}

void HeadphoneTestWindow::OnEvent(const string & eventID, void * data)
{
	if (eventID.compare("HRTF-test-end") == 0) {
		Stop();
		return;
	}
}

void HeadphoneTestWindow::keyPressEvent(QKeyEvent * event)
{
	if (!testStarted) return;
	switch (event->key())
	{
	case Qt::Key::Key_4:
	{
		keyOffsetX -= 1;
		if (keyOffsetX < 0) keyOffsetX = 0;
		KeyResponseInfo();
		break;
	}

	case Qt::Key::Key_5:
	{
		ResetAnswerKeyOffset();
		KeyResponseInfo();
		break;
	}

	case Qt::Key::Key_6:
	{
		keyOffsetX += 1;
		int sizeX = sortableAzimuth->GetValues().size();
		if (keyOffsetX >= sizeX)
			keyOffsetX = sizeX - 1;
		KeyResponseInfo();
		break;
	}

	case Qt::Key::Key_8:
	{
		keyOffsetY += 1;
		int sizeY = sortableElevation->GetValues().size();
		if (keyOffsetY >= sizeY)
			keyOffsetY = sizeY - 1;
		KeyResponseInfo();
		break;
	}

	case Qt::Key::Key_2:
	{
		keyOffsetY -= 1;
		if (keyOffsetY < 0) keyOffsetY = 0;
		KeyResponseInfo();
		break;
	}

	case Qt::Key::Key_Enter:
	{
		SendKeyboardAnswer();
		ResetAnswerKeyOffset();
		break;
	}
	case Qt::Key::Key_Return:
	{
		SendKeyboardAnswer();
		ResetAnswerKeyOffset();
		break;
	}
	default:
		break;
	}
}

void HeadphoneTestWindow::KeyResponseInfo() const
{
	auto azimuth = sortableAzimuth->GetValues()[keyOffsetX];
	auto elevation = sortableElevation->GetValues()[keyOffsetY];
	azimuthAnswer->SetValue(azimuth);
	elevationAnswer->SetValue(elevation);
}

void HeadphoneTestWindow::ResetAnswerKeyOffset()
{
	keyOffsetX = sortableAzimuth->GetValues().size() / 2;
	keyOffsetY = sortableElevation->GetValues().size() / 2;
	KeyResponseInfo();
}

void HeadphoneTestWindow::SendKeyboardAnswer() const
{
	auto azimuth = sortableAzimuth->GetValues()[keyOffsetX];
	auto elevation = sortableElevation->GetValues()[keyOffsetY];
	recorder->VerifyAnswer(azimuth, elevation);
}
