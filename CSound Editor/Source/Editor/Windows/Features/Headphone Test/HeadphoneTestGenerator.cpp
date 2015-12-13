#include "HeadphoneTestGenerator.h"

#include <Editor/Windows/Interface/QtInput.h>

#include <include/utils.h>
#include <Manager/Manager.h>
#include <Manager/EventSystem.h>

// QT
#include <QLabel>
#include <QFlags>
#include <QCheckBox>
#include <QLayout>
#include <QLineEdit>
#include <QPushButton>

HeadphoneTestGenerator::HeadphoneTestGenerator()
{
	generatedPanel = nullptr;
	InitUI();
}

void HeadphoneTestGenerator::InitUI()
{
	qtLayout->setSpacing(5);
	qtLayout->setContentsMargins(5, 5, 5, 5);
	qtLayout->setAlignment(Qt::AlignTop);
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	
	{
		auto zone = new QLabel("Create Test Case");
		zone->setAlignment(Qt::AlignCenter);
		zone->setMargin(5);
		zone->setFont(QFont("Arial", 10, QFont::Bold));
		qtLayout->addWidget(zone);
	}

	{
		auto button = new QPushButton();
		button->setText("Clear List");
		button->setMinimumHeight(30);
		button->click();
		qtLayout->addWidget(button);
		QObject::connect(button, &QPushButton::clicked, this, &HeadphoneTestGenerator::Clear);
	}

	numberOfSamples = new SimpleFloatInput("Samples:", "samples", 0);
	numberOfSamples->AcceptNegativeValues(false);
	AddWidget(numberOfSamples);

	{
		testsSamples = new QLineEdit();
		testsSamples->setReadOnly(true);
		qtLayout->addWidget(testsSamples);
	}
}

void HeadphoneTestGenerator::Clear()
{
	testsSamples->clear();
	azimuthTestValues.clear();
	elevationTestValues.clear();
	numberOfSamples->SetValue(0);
}

void HeadphoneTestGenerator::SetupGenerator(vector<float> azimuthSamples, vector<float> elevationSamples)
{
	int width = azimuthSamples.size();
	int height = elevationSamples.size();

	if (width == 0 || height == 0)
		return;

	Clear();

	if (generatedPanel) {
		generatedPanel->DetachFromParent();
		generatedPanel->deleteLater();
	}

	generatedPanel = new CustomWidget(QBoxLayout::Direction::TopToBottom);
	generatedPanel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	buttons.resize(width * height);

	char label[64];

	for (int i = (height - 1) ; i >= 0; i--)
	{
		auto widget = new CustomWidget(QBoxLayout::Direction::LeftToRight);

		for (int j = 0; j < width; j++)
		{
			sprintf(label, "[%d , %d]", (int)azimuthSamples[j], (int)elevationSamples[i]);

			auto button = new QPushButton();
			button->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
			button->setText(label);

			unsigned int index = i * width + j;
			QObject::connect(button, &QPushButton::clicked, this, [&, index]() {
				ButtonClick(index);
			});

			buttons[i * width + j] = button;
			widget->AddWidget(button);
		}

		generatedPanel->AddWidget(widget);
	}

	qtLayout->addWidget(generatedPanel);
}

unsigned int HeadphoneTestGenerator::GetNumberOfSamples() const
{
	return azimuthTestValues.size();
}

const vector<float>& HeadphoneTestGenerator::GetAzimuthValues() const
{
	return azimuthTestValues;
}

const vector<float>& HeadphoneTestGenerator::GetElevationValues() const
{
	return elevationTestValues;
}

void HeadphoneTestGenerator::ButtonClick(unsigned int index)
{
	auto button = buttons[index];

	// get azimuth and elevation from button text
	auto value = button->text();
	value = value.mid(1, button->text().size() - 2);

	// split by comma
	auto values = value.split(",", QString::SkipEmptyParts);

	float azimuth = values[0].toFloat();
	float elevation = values[1].toFloat();
	azimuthTestValues.push_back(azimuth);
	elevationTestValues.push_back(elevation);

	testsSamples->insert(button->text() + " ");

	// Increase number of test samples
	numberOfSamples->SetValue(azimuthTestValues.size());
}
