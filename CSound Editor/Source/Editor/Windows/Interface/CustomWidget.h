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
		virtual void SetOnTop();

		void AddWidget(QWidget *widget);
		void DetachFromParent();
		void SetAsToolWindow();
		void SetIcon(const char* fileName);
		void SetStyleID(const char* ID);

		void ReloadStyleSheet();
		void LoadStyleSheet(const char* fileLocation);

	protected:
		string styleSheet;
		QBoxLayout* qtLayout;
};