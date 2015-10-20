#include "HrtfTestAnswerPanel.h"

#include <Editor/Windows/Interface/QtInput.h>

#include <3DWorld/Scripts/HrtfRecorder.h>


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

HrtfRecorder *recorder;


HrtfTestAnswerPanel::HrtfTestAnswerPanel()
{
	recorder = new HrtfRecorder();

	generatedPanel = nullptr;
	InitUI();
}

void HrtfTestAnswerPanel::InitUI()
{
	qtLayout->setSpacing(5);
	qtLayout->setContentsMargins(5, 5, 5, 5);
	qtLayout->setAlignment(Qt::AlignTop);
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

}

void HrtfTestAnswerPanel::SetupAnswerPanel(vector<float> azimuthSamples, vector<float> elevationSamples)
{
	if (generatedPanel) {
		generatedPanel->DetachFromParent();
		generatedPanel->deleteLater();
	}

	generatedPanel = new CustomWidget(QBoxLayout::Direction::TopToBottom);
	generatedPanel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	int width = azimuthSamples.size();
	int height = elevationSamples.size();
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

void HrtfTestAnswerPanel::ButtonClick(unsigned int index)
{
	auto button = buttons[index];

	// get azimuth and elevation from button text
	auto value = button->text();
	value = value.mid(1, button->text().size() - 2);

	// split by comma
	auto values = value.split(",", QString::SkipEmptyParts);

	float azimuth = values[0].toFloat();
	float elevation = values[1].toFloat();

	recorder->VerifyAnswer(azimuth, elevation);
}
