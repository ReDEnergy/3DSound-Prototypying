#include "HrtfTest.h"

#include <Editor/Windows/Interface/QtInput.h>
#include <Editor/Windows/Interface/QtSortableInput.h>
#include <Editor/Windows/Features/HrtfTestGenerator.h>
#include <Editor/Windows/Features/HrtfTestAnswerPanel.h>
#include <3DWorld/Scripts/HrtfRecorder.h>

#include <Manager/Manager.h>
#include <Manager/EventSystem.h>

// QT
#include <QPushButton>
#include <QLabel>
#include <QKeyEvent>

HrtfTest::HrtfTest()
{
	LoadStyleSheet("hrtf-test.qss");
	setWindowTitle("Record HRTF Test");
	InitUI();

	SubscribeToEvent("HRTF-test-end");
}

void HrtfTest::InitUI()
{
	qtLayout->setSpacing(5);
	qtLayout->setContentsMargins(5, 5, 5, 5);
	qtLayout->setAlignment(Qt::AlignTop);

	answerPanel = new HrtfTestAnswerPanel();

	sampleGenerator = new HrtfTestGenerator();

	{
		auto *button = new QPushButton();
		button->setText("Start Test");
		button->setMinimumHeight(30);
		qtLayout->addWidget(button);
		QObject::connect(button, &QPushButton::clicked, this, &HrtfTest::Start);
	}

	{
		auto *button = new QPushButton();
		button->setText("Stop Test");
		button->setMinimumHeight(30);
		qtLayout->addWidget(button);
		QObject::connect(button, &QPushButton::clicked, this, []() {
			Manager::GetEvent()->EmitAsync("stop-HRTF-test");
		});
	}

	// ------------------------------------------------------------------------
	// Configuration Panel

	configArea = new CustomWidget();
	advanceConfig = new CustomWidget();

	{
		auto zone = new QLabel("Configure Test");
		zone->setFixedHeight(35);
		zone->setAlignment(Qt::AlignCenter);
		zone->setFont(QFont("Arial", 10, QFont::Bold));
		configArea->AddWidget(zone);
	}

	{
		auto W = new CustomWidget(QBoxLayout::LeftToRight);
		W->layout()->setSpacing(0);
		auto label = new QLabel("Test Name: ");
		label->setMinimumWidth(95);
		testName = new QLineEdit();
		W->AddWidget(label);
		W->AddWidget(testName);
		configArea->AddWidget(W);
	}

	randomIterations = new SimpleFloatInput("Random iterations:", "iterations", 0);
	randomIterations->AcceptNegativeValues(false);
	randomIterations->OnUserEdit([this](float value) {
		if (value == 0)
			randomIterations->SetValue(1);
	});
	configArea->AddWidget(randomIterations);

	auto showAdvancedConfig = new SimpleCheckBox("Advanced mode:", false);
	showAdvancedConfig->OnUserEdit([this](bool value) {
		value ? configArea->AddWidget(advanceConfig) : advanceConfig->DetachFromParent();
	});
	configArea->AddWidget(showAdvancedConfig);

	randomValues = new SimpleCheckBox("Random Values:", true);
	randomValues->OnUserEdit([this](bool value) {
		value ? sampleGenerator->DetachFromParent() : advanceConfig->AddWidget(sampleGenerator);
	});
	advanceConfig->AddWidget(randomValues);

	wait4Input = new SimpleCheckBox("Wait for user input:", true);
	wait4Input->SetLabelWidth(100);
	advanceConfig->AddWidget(wait4Input);

	prepareTime = new SimpleFloatInput("Prepare time:", "sec");
	prepareTime->AcceptNegativeValues(false);
	advanceConfig->AddWidget(prepareTime);

	sampleDuration = new SimpleFloatInput("Sample duration:", "sec");
	sampleDuration->AcceptNegativeValues(false);
	advanceConfig->AddWidget(sampleDuration);

	sampleInterval = new SimpleFloatInput("Sample interval:", "sec");
	sampleInterval->AcceptNegativeValues(false);
	advanceConfig->AddWidget(sampleInterval);

	sortableAzimuth = new QtSortableInput("Azimut values:", 0);
	sortableAzimuth->OnUserEdit([this](const vector<float>& vec) {
		sampleGenerator->SetupGenerator(vec, sortableElevation->GetValues());
	});	advanceConfig->AddWidget(sortableAzimuth);

	sortableElevation = new QtSortableInput("Elevation values:", 0);
	sortableElevation->OnUserEdit([this](const vector<float>& vec) {
		sampleGenerator->SetupGenerator(sortableAzimuth->GetValues(), vec);
	});
	advanceConfig->AddWidget(sortableElevation);

	{
		auto *button = new QPushButton();
		button->setText("Reset Configuration");
		button->setMinimumHeight(30);
		advanceConfig->AddWidget(button);
		QObject::connect(button, &QPushButton::clicked, this, &HrtfTest::ResetConfig);
	}

	qtLayout->addWidget(configArea);

	// ------------------------------------------------------------------------
	// Init Configuration
	ResetConfig();
}

void HrtfTest::Start()
{
	if (randomValues->GetValue() == false && sampleGenerator->GetNumberOfSamples() == 0)
	{
		cout << "Please add at least 1 sample" << endl;
		return;
	}

	static HrtfTestConfig config;
	config.randomTest = randomValues->GetValue();
	config.prepareTime = prepareTime->GetValue();
	config.randomIterations = randomIterations->GetValue();
	config.samplePlaybackDuration = sampleDuration->GetValue();
	config.sampleInterval = sampleInterval->GetValue();
	config.waitForInput = wait4Input->GetValue();
	config.testName = testName->text().trimmed().toStdString();

	if (config.randomTest)
	{
		config.azimuthValues = sortableAzimuth->GetValues();
		config.elevationValues = sortableElevation->GetValues();
	}
	else {
		config.azimuthValues = sampleGenerator->GetAzimuthValues();
		config.elevationValues = sampleGenerator->GetElevationValues();
	}

	answerPanel->SetupAnswerPanel(sortableAzimuth->GetValues(), sortableElevation->GetValues());

	Manager::GetEvent()->EmitAsync("start-HRTF-test", (void*)(&config));

	AddWidget(answerPanel);
	configArea->DetachFromParent();
}

void HrtfTest::Stop()
{
	if (configArea->parent() != this)
	{
		qtLayout->addWidget(configArea);
	}
	answerPanel->DetachFromParent();
}

void HrtfTest::ResetConfig()
{
	prepareTime->SetValue(3);
	sampleDuration->SetValue(200.0f);
	sampleInterval->SetValue(3.0f); 
	randomIterations->SetValue(1);
	sortableElevation->SetValue("-60, -30, 0, 30, 60");
	sortableAzimuth->SetValue("-90, -60, -30, 0, 30, 60, 90");
	wait4Input->SetValue(true);
}

void HrtfTest::OnEvent(const string & eventID, void * data)
{
	if (eventID.compare("HRTF-test-end") == 0) {
		Stop();
		return;
	}
}

void HrtfTest::keyPressEvent(QKeyEvent * event)
{
	cout << event->key() << endl;
}

void HrtfTest::keyReleaseEvent(QKeyEvent * event)
{
}
