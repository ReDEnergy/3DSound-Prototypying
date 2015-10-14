#pragma once

#include <vector>
#include <string>
#include <functional>

#include <Editor/Windows/Interface/CustomWidget.h>
#include <Editor/QT/Utils.h>

#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QEvent>
#include <QRegExp>
#include <QRegExpValidator>

#include <include/glm.h>
#include <include/math.h>

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

		void CellEdit() {
			updateFunc(value);
		}

		void Update(const T &vec)
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
			qtLayout->setDirection(QBoxLayout::LeftToRight);
			qtLayout->setSpacing(0);

			precision = min(precision, 6);
			format = new char[20];
			sprintf(format, "%%.%df %s", precision, unit);

			{
				auto l = new QLabel(label);
				l->setMinimumWidth(80);
				l->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);

				prop = new QLineEdit();
				prop->setReadOnly(readOnly);
				qtLayout->addWidget(l);
				qtLayout->addWidget(prop);
			}
		}

		~SimpleFloatInput() {};

		void SetReadOnly(bool value)
		{
			prop->setReadOnly(value);
		}

		void Update(float value)
		{
			char buff[20];
			sprintf(buff, format, value);
			prop->setText(buff);
		}

		private:
			char *format;
			QLineEdit *prop;
};