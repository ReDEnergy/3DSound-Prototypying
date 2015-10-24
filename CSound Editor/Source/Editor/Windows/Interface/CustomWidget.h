#pragma once
#include <string>

#include <Editor/QTForward.h>
#include <QWidget>
#include <QBoxLayout>

using namespace std;

class CustomWidget
	: public QWidget
{
	public:
		CustomWidget(QBoxLayout::Direction direction = QBoxLayout::Direction::TopToBottom);
		virtual ~CustomWidget() {};

		virtual void Init();
		virtual void Hide();
		virtual void Toggle();
		virtual void Close() {};

		void AddWidget(QWidget *widget);
		void DetachFromParent();

		void ReloadStyleSheet();
		void LoadStyleSheet(const char* fileLocation);

	protected:
		string styleSheet;
		QBoxLayout* qtLayout;
};