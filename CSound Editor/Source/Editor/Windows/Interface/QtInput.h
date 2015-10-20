#pragma once

#include <vector>
#include <string>
#include <functional>
#include <iostream>

#include <Editor/Windows/Interface/CustomWidget.h>
#include <Editor/QT/Utils.h>

#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QEvent>
#include <QCheckBox>
#include <QRegExp>
#include <QRegExpValidator>

#include <include/glm.h>
#include <include/math.h>

using namespace std;

// ***************************************************************************
// GLM UI component

template <class T>
class GLMVecComponent
	: public CustomWidget
{
	public:
		GLMVecComponent(const char* label, T vec)
		{
			updateFunc = nullptr;

			qtLayout->setDirection(QBoxLayout::LeftToRight);
			qtLayout->setSpacing(0);

			{
				auto l = new QLabel(label);
				l->setMinimumWidth(65);
				l->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
				qtLayout->addWidget(l);
			}

			char *format[4] = { "x", "y", "z", "w" };

			for (unsigned int i = 0; i < vec.length(); i++)
			{
				auto l = new QLabel(format[i]);
				l->setMinimumWidth(15);
				l->setAlignment(Qt::AlignCenter);
				prop[i] = new QLineEdit();
				prop[i]->setMinimumWidth(40);
				prop[i]->setValidator(QtTextValidator::GetFloatValidator());
				qtLayout->addWidget(l);
				qtLayout->addWidget(prop[i]);

				auto lineEdit = prop[i];
				QObject::connect(prop[i], &QLineEdit::editingFinished, this, [&, i](){
					value[i] = prop[i]->text().toFloat();
					if (updateFunc)
						updateFunc(value);
				});
			}
		}

		~GLMVecComponent() {};

		void OnUserEdit(function<void(T)> func) {
			updateFunc = func;
		}

		void SetValue(const T &vec)
		{
			value = vec;
			char buff[20];
			for (unsigned int i = 0; i < vec.length(); i++)
			{
				sprintf(buff, "%.3f", vec[i]);
				prop[i]->setText(buff);
			}
		}

	private:
		T value;
		function<void(T)> updateFunc;
		QLineEdit *prop[4];
};


// ***************************************************************************
// Simple Input component

class SimpleFloatInput
	: public CustomWidget
{
	public:
		SimpleFloatInput(const char* label, const char* unit = "", unsigned int precision = 2, bool readOnly = false)
		{
			acceptNegativeValues = true;

			qtLayout->setDirection(QBoxLayout::LeftToRight);
			qtLayout->setSpacing(0);

			precision = min(precision, 6);
			toPrecision = pow(10, precision);

			format = new char[20];
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
					value.replace(QRegExp("[a-zA-Z ]+"), "");
					SetValue(value.toFloat());
				});

				SetValue(0);
			}
		}

		~SimpleFloatInput() {};

		void SetLabelWidth(int width)
		{
			qLabel->setMinimumWidth(width);
		}

		void SetReadOnly(bool value)
		{
			prop->setReadOnly(value);
		}

		void OnUserEdit(function<void(float)> func)
		{
			updateFunc = func;
		}

		void SetValue(float value)
		{
			this->value = int(value * toPrecision) / (float)toPrecision;

			if (!acceptNegativeValues && this->value < 0)
				this->value = abs(this->value);

			char buff[20];
			sprintf(buff, format, this->value);
			prop->setText(buff);

			if (updateFunc)
				updateFunc(this->value);
		}

		void AcceptNegativeValues(bool value)
		{
			acceptNegativeValues = value;
		}

		float GetValue() {
			return value;
		}

		private:
			bool acceptNegativeValues;
			float value;
			int toPrecision;
			char *format;
			QLabel *qLabel;
			QLineEdit *prop;
			function<void(float)> updateFunc;
};


class SimpleCheckBox
	: public CustomWidget
{
	public:
		SimpleCheckBox(const char* label, bool checked = false)
		{
			qtLayout->setDirection(QBoxLayout::LeftToRight);
			qtLayout->setSpacing(0);

			{
				qLabel = new QLabel(label);
				qLabel->setMinimumWidth(80);
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

		~SimpleCheckBox() {};

		void SetLabelWidth(int width)
		{
			qLabel->setMinimumWidth(width);
		}

		void OnUserEdit(function<void(bool)> func)
		{
			updateFunc = func;
		}

		void SetValue(bool checked)
		{
			value = checked;
			prop->setCheckState(checked ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
			if (updateFunc)
				updateFunc(value);
		}

		bool GetValue()
		{
			return value;
		}

	private:
		bool value;
		QLabel *qLabel;
		QCheckBox *prop;
		function<void(bool)> updateFunc;
};