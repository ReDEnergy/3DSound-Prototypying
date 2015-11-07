#include "AboutWindow.h"

#include <Editor/Windows/Interface/QtInput.h>
#include <Editor/Windows/Interface/QtSortableInput.h>
#include <Editor/Windows/Features/Headphone Test/HeadphoneTestGenerator.h>
#include <Editor/Windows/Features/Headphone Test/HeadphoneTestAnswerPanel.h>
#include <3DWorld/Scripts/HeadphoneTestScript.h>

#include <Manager/Manager.h>
#include <Manager/EventSystem.h>

// QT
#include <QPushButton>
#include <QLabel>
#include <QKeyEvent>
#include <QPlainTextEdit>
#include <QDir>

static HeadphoneTestScript *recorder;

AboutWindow::AboutWindow()
{
	LoadStyleSheet("about-window.qss");
	setWindowTitle("About");
	SetIcon("chat.png");
	setFixedSize(300, 150);

	InitUI();
}

void AboutWindow::InitUI()
{
	qtLayout->setSpacing(5);
	qtLayout->setContentsMargins(5, 5, 5, 5);
	qtLayout->setAlignment(Qt::AlignTop);

}