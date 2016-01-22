#include "HeadphoneTestWindow.h"

#include <Editor/Windows/Interface/QtInput.h>
#include <Editor/Windows/Interface/QtSortableInput.h>
#include <Editor/Windows/Features/Headphone Test/HeadphoneTestGenerator.h>
#include <Editor/Windows/Features/Headphone Test/HeadphoneTestAnswerPanel.h>
#include <3DWorld/Scripts/HeadphoneTestScript.h>

#include <include/utils.h>
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
	SetOnTop();

	config = new HeadphoneTestConfig();
	recorder = new HeadphoneTestScript();

	SubscribeToEvent("HRTF-test-end");

	// Default Test
	customTests.push_back(new HeadphoneTestConfig());

	{
		auto *test = new HeadphoneTestConfig();
		test->testID = "Test 1";
		test->azimuthValues = { -60, 0, 60 };
		test->elevationValues = { -60, 0, 60 };
		test->randomIterations = 4;
		test->outputTested = 7;
		customTests.push_back(test);
	}

	{
		auto test = new HeadphoneTestConfig();
		test->testID = "Test 2";
		test->azimuthValues = { -90, -60, -30, 0, 30, 60, 90 };
		test->elevationValues = { 0 };
		test->randomIterations = 4;
		test->outputTested = 7;
		customTests.push_back(test);
	}

	{
		auto test = new HeadphoneTestConfig();
		test->testID = "Test 3";
		test->azimuthValues = { -90, -60, -30, 0, 30, 60, 90 };
		test->elevationValues = { -60, -30, 0, 30, 60 };
		test->randomIterations = 4;
		test->outputTested = 7;
		customTests.push_back(test);
	}

	{
		auto *test = new HeadphoneTestConfig();
		test->testID = "Test Quad";
		test->azimuthValues = { -60, 0, 60 };
		test->elevationValues = { -60, 0, 60 };
		test->randomIterations = 2;
		SET_BIT(test->outputTested, 4);
		customTests.push_back(test);
	}

	{
		auto *test = new HeadphoneTestConfig();
		test->testID = "Test 8 channels";
		test->azimuthValues = { -60, 0, 60 };
		test->elevationValues = { -60, 0, 60 };
		test->randomIterations = 2;
		SET_BIT(test->outputTested, 5);
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
		sectionLeft->AddWidget(buttonStartStop);
		QObject::connect(buttonStartStop, &QPushButton::clicked, this, [this]() {
			testStarted ? Manager::GetEvent()->EmitAsync("stop-HRTF-test") : Start();
		});
	}

	// ------------------------------------------------------------------------
	// Help Info

	{
		auto info = new QLabel("Keyboard keys [Numpad]");
		info->setProperty("HeadlineSection", "");
		info->setAlignment(Qt::AlignCenter);
		info->setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Fixed);
		sectionLeft->AddWidget(info);

		auto block = new QPlainTextEdit();
		block->setReadOnly(true);
		block->insertPlainText(" 4, 8  - change azimuth left/right\n");
		block->insertPlainText(" 2, 8  - change elevation down/up\n");
		block->insertPlainText(" 5      - center azimuth, elevation to [0, 0]\n");
		block->insertPlainText(" Enter - submit answer");
		block->setFixedHeight(75);
		sectionLeft->AddWidget(block);

		azimuthAnswer = new SimpleFloatInput("Azimuth response:", "deg", 2, true);
		elevationAnswer = new SimpleFloatInput("Elevation response:", "deg", 2, true);

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
		zone->setProperty("HeadlineSection", "");
		zone->setAlignment(Qt::AlignCenter);
		zone->setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Fixed);
		generalConfigPanel->AddWidget(zone);
	}

	{
		auto W = new CustomWidget(QBoxLayout::LeftToRight);
		W->layout()->setSpacing(0);
		W->setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Fixed);

		auto label = new QLabel("Test Name: ");
		testName = new QLineEdit();
		W->AddWidget(label);
		W->AddWidget(testName);
		generalConfigPanel->AddWidget(W);
	}

	auto dropdown = new SimpleDropDown("Custom test");
	for (uint i = 1; i < customTests.size(); i++)
	{
		dropdown->AddOption(customTests[i]->testID.c_str(), QVariant(i));
	}
	dropdown->OnChange([&](QVariant value) {
		SetConfig(customTests[value.toUInt()]);
	});
	generalConfigPanel->AddWidget(dropdown);


	showAdvancedConfig = new SimpleCheckBox("Advanced mode:", false);
	showAdvancedConfig->setContentsMargins(0, 0, 0, 10);
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
	advanceConfigPanel->AddWidget(wait4Input);

	randomIterations = new SimpleFloatInput("Random iterations:", "iterations", 0);
	randomIterations->AcceptNegativeValues(false);
	randomIterations->OnUserEdit([this](float value) {
		if (value == 0)
			randomIterations->SetValue(1);
	});
	advanceConfigPanel->AddWidget(randomIterations);

	// Test output models

	testTypes["HRTF"] = new SimpleCheckBox("Test HRTF:", true);
	advanceConfigPanel->AddWidget(testTypes["HRTF"]);

	testTypes["indivHRTF"] = new SimpleCheckBox("Individual HRTF:", true);
	advanceConfigPanel->AddWidget(testTypes["indivHRTF"]);

	testTypes["StereoPan"] = new SimpleCheckBox("Stereo Panning:", true);
	advanceConfigPanel->AddWidget(testTypes["StereoPan"]);

	testTypes["QuadHRTF"] = new SimpleCheckBox("Quad HRTF:", false);
	advanceConfigPanel->AddWidget(testTypes["QuadHRTF"]);

	testTypes["QuadPan"] = new SimpleCheckBox("Quad Panning:", false);
	advanceConfigPanel->AddWidget(testTypes["QuadPan"]);

	testTypes["8ChanHRTF"] = new SimpleCheckBox("8 Channel:", false);
	advanceConfigPanel->AddWidget(testTypes["8ChanHRTF"]);

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

	config->outputTested = 0;
	config->outputTested |= int(testTypes["HRTF"]->GetValue());
	config->outputTested |= int(testTypes["StereoPan"]->GetValue()) << 1;
	config->outputTested |= int(testTypes["indivHRTF"]->GetValue()) << 2;
	config->outputTested |= int(testTypes["QuadHRTF"]->GetValue()) << 4;
	config->outputTested |= int(testTypes["8ChanHRTF"]->GetValue()) << 5;
	config->outputTested |= int(testTypes["QuadPan"]->GetValue()) << 6;

	if (config->outputTested == 0) {
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

	testTypes["HRTF"]->SetValue(GET_BIT(config->outputTested, 0));
	testTypes["StereoPan"]->SetValue(GET_BIT(config->outputTested, 1));
	testTypes["indivHRTF"]->SetValue(GET_BIT(config->outputTested, 2));
	testTypes["QuadHRTF"]->SetValue(GET_BIT(config->outputTested, 4));
	testTypes["8ChanHRTF"]->SetValue(GET_BIT(config->outputTested, 5));
	testTypes["QuadPan"]->SetValue(GET_BIT(config->outputTested, 6));

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
