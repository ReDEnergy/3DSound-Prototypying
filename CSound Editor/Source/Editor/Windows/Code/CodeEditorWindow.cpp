#include "CodeEditorWindow.h"

#include <iostream>
#include <include/math.h>

#include <Editor/GUI.h>
#include <Manager/Manager.h>
#include <Manager/EventSystem.h>

#include <QLayout>
#include <QTextEdit>
#include <QTimer>

#include <CSound/CSoundSynthesizer.h>


static unsigned int windowNumber = 0;

CodeEditorWindow::CodeEditorWindow()
{
	setWindowTitle("Code Editor");
	InitUI();
}

void CodeEditorWindow::InitUI()
{
	qtTextEdit = new QTextEdit(this);
	qtTextEdit->setBaseSize(QSize(300, 200));

	QFont font;
	QFontMetrics metrics(font);
	qtTextEdit->setTabStopWidth(8 * metrics.width(' '));
	qtTextEdit->setFont(QFont("Arial", 10));

	codeUpdateTimer = new QTimer();
	codeUpdateTimer->setInterval(1000);
	codeUpdateTimer->setSingleShot(true);

	QObject::connect(qtTextEdit, &QTextEdit::textChanged, this, [this]() {
		disableFirstUpdate ? (disableFirstUpdate = false) : codeUpdateTimer->start();
	});

	QObject::connect(codeUpdateTimer, &QTimer::timeout, this, [this]() {
		activeContext->SetSourceCode(qtTextEdit->toPlainText().toStdString());
		Manager::GetEvent()->EmitAsync("model-changed", activeContext);
	});

	qtLayout->addWidget(qtTextEdit);

	GUI::Set(QT_INSTACE::CODE_EDITOR_WINDOW, (void*)this);
}

void CodeEditorWindow::SetContext(CSoundSynthesizer *score)
{
	if (activeContext == score)
		return;
	activeContext = score;
	disableFirstUpdate = true;
	qtTextEdit->setPlainText(score->GetSourceCode().c_str());
}