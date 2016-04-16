#pragma once
#include <Editor/include/QtForward.h>

using namespace std;

QWidget* Wrap(const char* labelText, unsigned int labelWidth, QWidget *W);
QWidget* Wrap(QLabel* label, QWidget *W);

class QtTextValidator
{
	public:
		static QRegExpValidator* GetFloatValidator();

	private:
		static QRegExpValidator* floatValidator;
};
