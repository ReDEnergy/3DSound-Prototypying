#include "EditorMainWindow.h"

#include <functional> 

#include <Editor/Editor.h>

// QT objects
#include <QMenuBar>
#include <QToolBar>
#include <QToolButton>
#include <QComboBox>
#include <QStyledItemDelegate>
#include <QStatusBar>
#include <QFileDialog>
#include <QApplication>

// CSound Manager
#include <CSound/CSoundManager.h>
#include <CSound/SoundManager.h>

// Engine Library
#include <Manager/Manager.h>
#include <Core/Engine.h>

uint frame = 0;

EditorMainWindow::EditorMainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	Engine::Init();
	Manager::Init();
	SoundManager::Init();
	CSoundEditor::Init();

	SetupUI(this);
	show();

	for (auto &window : appWindows) {
		window.second->Init();
	}
}

EditorMainWindow::~EditorMainWindow()
{
}

void EditorMainWindow::Run(QApplication *app)
{
	float fps = 24;
	this->app = app;

	//qtTimer = new QTimer(0);
	//qtTimer->setInterval(1000 / fps);
	//qtTimer->start();

	//connect(qtTimer, &QTimer::timeout, this, &EditorMainWindow::Update);

	//QThread* thread = new QThread(this);
	//qtTimer->moveToThread(thread);
	//thread->start();

	// Move the processing to the engine...
	// Should suspends the current code... but somehow application still process events
	Engine::Run();
}

void EditorMainWindow::close()
{
	Engine::Exit();
}

void EditorMainWindow::closeEvent(QCloseEvent * event)
{
	Engine::Exit();
}

void EditorMainWindow::SetupUI(QMainWindow *MainWindow) {

	MainWindow->setWindowTitle("Sound Engine");

	MainWindow->resize(1280, 720);

	MainWindow->setDockOptions(QMainWindow::AnimatedDocks |
								QMainWindow::AllowNestedDocks |
								QMainWindow::AllowTabbedDocks);

	MainWindow->setTabPosition(Qt::AllDockWidgetAreas, QTabWidget::TabPosition::North);

	// ************************************************************************
	// File Picker

	auto *picker = new FilePicker();
	picker->setNameFilter("*.xml");
	GUI::Set(QT_INSTACE::FILE_PICKER, (void*) picker);

	// ************************************************************************
	// Menu Bar

	QMenuBar *menuBar = new QMenuBar(MainWindow);
	menuBar->setObjectName(QStringLiteral("menuBar"));
	menuBar->setGeometry(QRect(0, 0, 1051, 21));
	MainWindow->setMenuBar(menuBar);

	// Menu Entry
	{
		QMenu *menuEngine = new QMenu(menuBar);
		menuEngine->setObjectName(QStringLiteral("menuEngine"));
		menuEngine->setTitle("File");

		menuBar->addAction(menuEngine->menuAction());

		{
			QAction *action = new QAction(MainWindow);
			action->setText("Exit");
			menuEngine->addAction(action);
			QObject::connect(action, &QAction::triggered, this, &EditorMainWindow::close);
		}

		{
			QAction *action = new QAction(MainWindow);
			action->setText("Save As...");
			menuEngine->addAction(action);

			QObject::connect(action, &QAction::triggered, this, [picker]() {
				picker->OpenForSave();
			});

			QObject::connect(picker, &QFileDialog::fileSelected, this, [picker] (const QString & file) {
				if (picker->IsSaving()) {
					CSoundEditor::GetScene()->SaveSceneAs(file.toStdString().c_str());
				}
			});
		}
	}

	// Menu Entry
	{
		QMenu *menu = new QMenu(menuBar);
		menu->setTitle("Tools");
		menuBar->addAction(menu->menuAction());

		// Submenu buttons
		QAction *action = new QAction(MainWindow);
		action->setText("Reload StyleSheets");
		menu->addAction(action);
		QObject::connect(action, &QAction::triggered, this, &EditorMainWindow::ReloadStyleSheets);
	}

	// ************************************************************************
	// Toolbar
	QToolBar *toolbar = new QToolBar();
	toolbar->setWindowTitle("Toolbar");

	// Play Audio
	{
		QToolButton *button = new QToolButton();
		button->setText("Play");
		button->setMaximumWidth(50);
		button->setMinimumHeight(30);
		toolbar->addWidget(button);

		QObject::connect(button, &QToolButton::clicked, this, []() {
			CSoundEditor::GetScene()->Play();
		});
	}

	// Stop Audio
	{
		QToolButton *button = new QToolButton();
		button->setText("Stop");
		button->setMaximumWidth(50);
		button->setMinimumHeight(30);
		toolbar->addWidget(button);

		QObject::connect(button, &QToolButton::clicked, this, []() {
			CSoundEditor::GetScene()->Stop();
		});
	}

	// Save Scene
	{
		QToolButton *button = new QToolButton();
		button->setText("Save Scene");
		button->setMinimumHeight(30);
		toolbar->addWidget(button);

		QObject::connect(button, &QToolButton::clicked, this, []() {
			auto result = CSoundEditor::GetScene()->SaveScene();
			if (!result) {
				auto picker = GUI::Get<FilePicker>(QT_INSTACE::FILE_PICKER);
				if (picker) {
					picker->OpenForSave();
				}
			}
		});
	}

	// Load Scene
	{
		QToolButton *button = new QToolButton();
		button->setText("Load Scene");
		button->setMinimumHeight(30);
		toolbar->addWidget(button);

		QObject::connect(picker, &QFileDialog::fileSelected, this, [picker] (const QString & file) {
			if (!picker->IsSaving())
			{
				CSoundEditor::GetScene()->LoadScene(file.toStdString().c_str());
				GUI::Get<SceneWindow>(QT_INSTACE::SCENE_EDITOR)->Init();
			}
		});

		QObject::connect(button, &QToolButton::clicked, this, [picker]() {
			picker->OpenForLoad();
		});
	}

	// Clear Scene
	{
		QToolButton *button = new QToolButton();
		button->setText("Clear Scene");
		button->setMinimumHeight(30);
		toolbar->addWidget(button);

		QObject::connect(button, &QToolButton::clicked, this, []() {
			CSoundEditor::GetScene()->Clear();
			GUI::Get<SceneWindow>(QT_INSTACE::SCENE_EDITOR)->Clear();
		});
	}

	// Padding Scene
	{
		QWidget *empty = new QWidget();
		empty->setMinimumWidth(500);
		empty->setMinimumHeight(30);
		toolbar->addWidget(empty);
	}

	// Sound Output Mode
	{
		dropdown = new QComboBox();
		dropdown->setMinimumWidth(60);

		QStyledItemDelegate* itemDelegate = new QStyledItemDelegate();
		dropdown->setItemDelegate(itemDelegate);
		dropdown->addItem("None", QVariant("none"));
		dropdown->addItem("HRTF2", QVariant("hrtf-output"));
		dropdown->addItem("Stereo", QVariant("stereo-output"));

		QWidget* widget = Wrap("Output", 35, dropdown);
		toolbar->addWidget(widget);
	}
	{
		QToolButton *button = new QToolButton();
		button->setText("Apply");
		button->setMinimumHeight(30);
		toolbar->addWidget(button);

		QObject::connect(button, &QToolButton::clicked, this, &EditorMainWindow::ChnageOuputModel);
	}

	// Attach toobar to the main window
	MainWindow->addToolBar(toolbar);

	// ************************************************************************
	// Status Bar

	QStatusBar *statusBar = new QStatusBar(MainWindow);
	statusBar->setObjectName(QStringLiteral("statusBar"));
	MainWindow->setStatusBar(statusBar);

	//QMetaObject::connectSlotsByName(MainWindow);

	// ************************************************************************
	// Central Area

	QWidget *centeralWidget = new QWidget();
	centeralWidget->setFixedWidth(0);
	MainWindow->setCentralWidget(centeralWidget);

	// ************************************************************************
	// Attach windows
	appWindows["TextPreview"] = new TextPreviewWindow();
	appWindows["ComponentList"] = new ComponentList();
	appWindows["InstrumentList"] = new InstrumentList();
	appWindows["ScoreList"] = new ScoreList();

	appWindows["ComponentEditor"] = new ComponentEditor();
	appWindows["InstrumentEditor"] = new InstrumentEditor();
	appWindows["ScoreEditor"] = new ScoreEditor();
	//appWindows["PropertyEditor"] = new PropertyEditor();

	appWindows["SceneWindow"] = new SceneWindow();
	appWindows["GameWindow"] = new GameWindow();

	//// Left Dock
	MainWindow->addDockWidget(Qt::DockWidgetArea::LeftDockWidgetArea, appWindows["SceneWindow"], Qt::Orientation::Vertical);
	MainWindow->addDockWidget(Qt::DockWidgetArea::LeftDockWidgetArea, appWindows["ScoreEditor"], Qt::Orientation::Horizontal);
	MainWindow->addDockWidget(Qt::DockWidgetArea::LeftDockWidgetArea, appWindows["InstrumentEditor"], Qt::Orientation::Horizontal);
	MainWindow->addDockWidget(Qt::DockWidgetArea::LeftDockWidgetArea, appWindows["ComponentEditor"], Qt::Orientation::Vertical);
	//MainWindow->addDockWidget(Qt::DockWidgetArea::LeftDockWidgetArea, appWindows["PropertyEditor"], Qt::Orientation::Vertical);
	MainWindow->addDockWidget(Qt::DockWidgetArea::LeftDockWidgetArea, appWindows["TextPreview"], Qt::Orientation::Horizontal);
	MainWindow->tabifyDockWidget(appWindows["TextPreview"], appWindows["GameWindow"]);

	appWindows["GameWindow"]->setFloating(true);
	appWindows["GameWindow"]->setFloating(false);

	// Right Dock
	MainWindow->addDockWidget(Qt::DockWidgetArea::RightDockWidgetArea, appWindows["ComponentList"], Qt::Orientation::Vertical);
	MainWindow->addDockWidget(Qt::DockWidgetArea::RightDockWidgetArea, appWindows["InstrumentList"], Qt::Orientation::Vertical);
	MainWindow->addDockWidget(Qt::DockWidgetArea::RightDockWidgetArea, appWindows["ScoreList"], Qt::Orientation::Vertical);
}

void EditorMainWindow::Update()
{
	app->processEvents();
}

void EditorMainWindow::ReloadStyleSheets()
{
	for (auto &window : appWindows) {
		window.second->ReloadStyleSheet();
	}
}

void EditorMainWindow::ChnageOuputModel()
{
	auto data = dropdown->currentData().toString().toStdString();
	CSoundEditor::GetScene()->SetOutputModel(data.c_str());
}

void EngineThread::run()
{
	Engine::Init();
	Manager::Init();
	Engine::Run();
}
