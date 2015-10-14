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
#include <Manager/SceneManager.h>
#include <Manager/ResourceManager.h>
#include <Manager/EventSystem.h>
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
	new UIEvents();

	for (auto &window : dockWindows) {
		window.second->Init();
	}

	for (auto &window : appWindows) {
		window.second->Init();
	}

	show();
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
	close();
}

void EditorMainWindow::SetupUI(QMainWindow *MainWindow) {

	MainWindow->setWindowTitle("Sound Engine");

	MainWindow->resize(1280, 720);

	MainWindow->setDockOptions(QMainWindow::AnimatedDocks |
								QMainWindow::AllowNestedDocks |
								QMainWindow::AllowTabbedDocks);

	MainWindow->setTabPosition(Qt::AllDockWidgetAreas, QTabWidget::TabPosition::North);

	// ************************************************************************
	// Load styling

	ReloadStyleSheets();

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
		toolbar->addWidget(button);

		QObject::connect(button, &QToolButton::clicked, this, []() {
			CSoundEditor::GetScene()->Stop();
		});
	}

	// Save Scene
	{
		QToolButton *button = new QToolButton();
		button->setText("Save Scene");
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
		toolbar->addWidget(button);

		QObject::connect(button, &QToolButton::clicked, this, []() {
			CSoundEditor::GetScene()->Clear();
			GUI::Get<SceneWindow>(QT_INSTACE::SCENE_EDITOR)->Clear();
		});
	}

	// Record HRTF Test
	{
		QToolButton *button = new QToolButton();
		button->setText("Record HRTF");
		toolbar->addWidget(button);

		appWindows["HRTFTest"] = new HrtfTest();
		auto HRTFWindow = appWindows["HRTFTest"];
		QObject::connect(button, &QToolButton::clicked, this, [HRTFWindow]() {
			if (HRTFWindow)
				HRTFWindow->show();
		});
	}

	// Moving Plane
	{
		QToolButton *button = new QToolButton();
		button->setText("Moving Plane");
		toolbar->addWidget(button);

		bool *state = new bool(false);
		QObject::connect(button, &QToolButton::clicked, this, [state]() {
			*state = !(*state);
			auto event = *state ? "start-moving-plane" : "stop-moving-plane";
			Manager::GetEvent()->EmitAsync(event);
		});
	}

	// Expanding Sphere
	{
		QToolButton *button = new QToolButton();
		button->setText("Expanding Sphere");
		toolbar->addWidget(button);

		bool *state = new bool(false);
		QObject::connect(button, &QToolButton::clicked, this, [state]() {
			*state = !(*state);
			auto event = *state ? "start-expanding-sphere" : "stop-expanding-sphere";
			Manager::GetEvent()->EmitAsync(event);
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

		void (QComboBox::* indexChangedSignal)(int index) = &QComboBox::currentIndexChanged;
		QObject::connect(dropdown, indexChangedSignal, this, [&](int index) {
			auto data = dropdown->currentData().toString().toStdString();
			CSoundEditor::GetScene()->SetOutputModel(data.c_str());
		});

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
	dockWindows["TextPreview"] = new TextPreviewWindow();
	dockWindows["ComponentList"] = new ComponentList();
	dockWindows["InstrumentList"] = new InstrumentList();
	dockWindows["ScoreList"] = new ScoreList();

	dockWindows["ScoreEditor"] = new ScoreEditor();
	dockWindows["InstrumentEditor"] = new InstrumentEditor();
	dockWindows["ComponentEditor"] = new ComponentEditor();
	
	dockWindows["SceneWindow"] = new SceneWindow();
	dockWindows["GameWindow"] = new GameWindow();

	dockWindows["ObjectProperty"] = new SceneObjectProperties();

	//// Left Dock
	MainWindow->addDockWidget(Qt::DockWidgetArea::LeftDockWidgetArea, dockWindows["SceneWindow"], Qt::Orientation::Vertical);
	MainWindow->addDockWidget(Qt::DockWidgetArea::LeftDockWidgetArea, dockWindows["ScoreEditor"], Qt::Orientation::Horizontal);
	MainWindow->addDockWidget(Qt::DockWidgetArea::LeftDockWidgetArea, dockWindows["InstrumentEditor"], Qt::Orientation::Horizontal);
	MainWindow->addDockWidget(Qt::DockWidgetArea::LeftDockWidgetArea, dockWindows["ComponentEditor"], Qt::Orientation::Vertical);
	MainWindow->tabifyDockWidget(dockWindows["ComponentEditor"], dockWindows["ObjectProperty"]);

	MainWindow->addDockWidget(Qt::DockWidgetArea::LeftDockWidgetArea, dockWindows["TextPreview"], Qt::Orientation::Horizontal);
	MainWindow->tabifyDockWidget(dockWindows["TextPreview"], dockWindows["GameWindow"]);

	dockWindows["GameWindow"]->setFloating(true);
	dockWindows["GameWindow"]->setFloating(false);

	// Right Dock
	MainWindow->addDockWidget(Qt::DockWidgetArea::RightDockWidgetArea, dockWindows["ComponentList"], Qt::Orientation::Vertical);
	MainWindow->addDockWidget(Qt::DockWidgetArea::RightDockWidgetArea, dockWindows["InstrumentList"], Qt::Orientation::Vertical);
	MainWindow->addDockWidget(Qt::DockWidgetArea::RightDockWidgetArea, dockWindows["ScoreList"], Qt::Orientation::Vertical);
}

void EditorMainWindow::Update()
{
	//app->processEvents();
	auto SceneWindow = GUI::Get<GameWindow>(QT_INSTACE::_3D_SCENE_WINDOW);
	if (SceneWindow) {
		SceneWindow->Update();
		SceneWindow->Render();
	}
	dockWindows["ObjectProperty"]->Update();
}

void EditorMainWindow::ReloadStyleSheets()
{
	for (auto &window : dockWindows) {
		window.second->ReloadStyleSheet();
	}

	// Set general app stylesheet
	QFile File(CSoundEditor::GetStyleSheetFilePath("app.qss").c_str());
	File.open(QFile::ReadOnly);
	QString StyleSheet = QLatin1String(File.readAll());
	setStyleSheet(StyleSheet);
}

void EngineThread::run()
{
	Engine::Init();
	Manager::Init();
	Engine::Run();
}
