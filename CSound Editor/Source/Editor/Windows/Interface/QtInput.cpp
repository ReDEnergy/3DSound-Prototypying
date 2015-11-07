#include "QtInput.h"

#include <QStyledItemDelegate>
#include <QComboBox>

// ***************************************************************************
// Simple Float Input component

SimpleFloatInput::SimpleFloatInput(const char * label, const char * unit, unsigned int precision, bool readOnly)
{
	acceptNegativeValues = true;

	qtLayout->setDirection(QBoxLayout::LeftToRight);
	qtLayout->setSpacing(0);

	precision = min(precision, 6);
	toPrecision = pow(10, precision);

	auto inputLength = strlen(unit) + 10;
	format = new char[inputLength];
	inputBuffer = new char[inputLength];
	sprintf(format, "%%.%df %s", precision, unit);

	{
		qLabel = new QLabel(label);
		qLabel->setMinimumWidth(80);
		qLabel->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);

		prop = new QLineEdit();
		prop->setReadOnly(readOnly);
		qtLayout->addWidget(qLabel);
		qtLayout->addWidget(prop);

		QObject::connect(prop, &QLineEdit::editingFinished, this, [&]() {
			auto value = prop->text();
			value.replace(QRegExp("[a-zA-Z \\/]+"), "");
			SetValue(value.toFloat());
		});

		SetValue(0);
	}
}

SimpleFloatInput::~SimpleFloatInput()
{
	delete inputBuffer;
	delete format;
}

void SimpleFloatInput::SetLabelWidth(int width)
{
	qLabel->setMinimumWidth(width);
}

void SimpleFloatInput::SetReadOnly(bool value)
{
	prop->setReadOnly(value);
}

void SimpleFloatInput::OnUserEdit(function<void(float)> func)
{
	updateFunc = func;
}

void SimpleFloatInput::SetValue(float value)
{
	this->value = int(value * toPrecision) / (float)toPrecision;

	if (!acceptNegativeValues)
		this->value = abs(this->value);

	sprintf(inputBuffer, format, this->value);
	prop->setText(inputBuffer);

	if (updateFunc)
		updateFunc(this->value);
}

void SimpleFloatInput::AcceptNegativeValues(bool value)
{
	acceptNegativeValues = value;
}

float SimpleFloatInput::GetValue() {
	return value;
}

// ***************************************************************************
// Simple CheckBox component

SimpleCheckBox::SimpleCheckBox(const char * label, bool checked)
{
	setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Fixed);
	qtLayout->setDirection(QBoxLayout::LeftToRight);
	qtLayout->setSpacing(0);

	{
		qLabel = new QLabel(label);
		qLabel->setMinimumWidth(100);
		qLabel->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);

		prop = new QCheckBox();
		qtLayout->addWidget(qLabel);
		qtLayout->addWidget(prop);

		QObject::connect(prop, &QCheckBox::clicked, this, [&](bool checked) {
			SetValue(checked);
		});

		SetValue(checked);
	}
}

void SimpleCheckBox::SetLabelWidth(int width)
{
	qLabel->setFixedWidth(width);
}

void SimpleCheckBox::SetValue(bool checked)
{
	value = checked;
	prop->setCheckState(checked ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
	if (updateFunc)
		updateFunc(value);
}

void SimpleCheckBox::OnUserEdit(function<void(bool)> func)
{
	updateFunc = func;
}

bool SimpleCheckBox::GetValue()
{
	return value;
}

// ***************************************************************************
// Simple DropDown component


SimpleDropDown::SimpleDropDown(const char * label)
{
	qtLayout->setDirection(QBoxLayout::LeftToRight);
	qtLayout->setSpacing(0);

	qLabel = new QLabel(label);
	qLabel->setMinimumWidth(100);
	qLabel->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);

	qtLayout->addWidget(qLabel);

	dropdown = new QComboBox();

	QStyledItemDelegate* itemDelegate = new QStyledItemDelegate();
	dropdown->setItemDelegate(itemDelegate);
	qtLayout->addWidget(dropdown);

	void (QComboBox::* indexChangedSignal)(int index) = &QComboBox::currentIndexChanged;
	QObject::connect(dropdown, indexChangedSignal, this, [&](int index) {
		if (updateFunc)
			updateFunc(dropdown->itemData(index));
	});
}

void SimpleDropDown::AddOption(const char *name, QVariant value)
{
	dropdown->addItem(name, value);
}

void SimpleDropDown::SetLabelWidth(int width)
{
	qLabel->setFixedWidth(width);
}

void SimpleDropDown::SetValue(unsigned int index)
{
}

void SimpleDropDown::OnChange(function<void(QVariant)> func)
{
	updateFunc = func;
}

bool SimpleDropDown::GetValue()
{
	return false;
}
