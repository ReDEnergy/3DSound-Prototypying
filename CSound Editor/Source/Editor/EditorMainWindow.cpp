#include "EditorMainWindow.h"

#include <functional> 
#include <stdlib.h>
#include <Editor/Editor.h>

// QT objects
#include <QMenuBar>
#include <QToolBar>
#include <QTimer>
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

EditorMainWindow::EditorMainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	Engine::Init();
	Manager::Init();
	SoundManager::Init();
	CSoundEditor::Init();

	// Create the saving directory if it doesn't exist
	bool exist = QDir("Saves").exists();
	if (!exist) {
		QDir().mkdir("Saves");
	}

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
	this->app = app;

	//float fps = 24;
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

	cout << "exit" << endl;
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

	setWindowIcon(*QtConfig::GetIcon("colorwheel.png"));

	MainWindow->setWindowTitle("Sound Engine v0.2.4.2");

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
			action->setIcon(*QtConfig::GetIcon("power.png"));
			menuEngine->addAction(action);
			QObject::connect(action, &QAction::triggered, this, &EditorMainWindow::close);
		}

		{
			QAction *action = new QAction(MainWindow);
			action->setText("Save As...");
			action->setIcon(*QtConfig::GetIcon("memorycard.png"));
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
		menu->setTitle("Scene");
		menuBar->addAction(menu->menuAction());

		// Submenu buttons
		QAction *action = new QAction(MainWindow);
		action->setText("Clear Scene");
		action->setIcon(*QtConfig::GetIcon("denied.png"));
		menu->addAction(action);

		QObject::connect(action, &QAction::triggered, this, []() {
			CSoundEditor::GetScene()->Clear();
			GUI::Get<SceneWindow>(QT_INSTACE::SCENE_EDITOR)->Clear();
		});
	}

	// Menu Entry
	{
		QMenu *menu = new QMenu(menuBar);
		menu->setTitle("Tools");
		menuBar->addAction(menu->menuAction());

		// Submenu buttons
		{
			QAction *action = new QAction(MainWindow);
			action->setText("Reload StyleSheets");
			action->setIcon(*QtConfig::GetIcon("cmyk.png"));
			menu->addAction(action);
			QObject::connect(action, &QAction::triggered, this, &EditorMainWindow::ReloadStyleSheets);
		}

		// Submenu buttons
		{
			QAction *action = new QAction(MainWindow);
			action->setText("Reload CSound Config");
			action->setIcon(*QtConfig::GetIcon("loading.png"));
			menu->addAction(action);
			QObject::connect(action, &QAction::triggered, this, []() {
				SoundManager::Init();
			});
		}
	}

	// Menu Entry
	{
		QMenu *menu = new QMenu(menuBar);
		menu->setTitle("CSound");
		menuBar->addAction(menu->menuAction());

		// Submenu buttons
		QAction *action = new QAction(MainWindow);
		action->setText("Options");
		action->setIcon(*QtConfig::GetIcon("speaker.png"));
		menu->addAction(action);

		appWindows["CSoundOptions"] = new CSoundOptionsWindow();

		QObject::connect(action, &QAction::triggered, this, [&]() {
			appWindows["CSoundOptions"]->Toggle();
		});
	}
	// Menu Entry
	{
		QMenu *menu = new QMenu(menuBar);
		menu->setTitle("Help");
		menuBar->addAction(menu->menuAction());

		// Submenu buttons
		QAction *action = new QAction(MainWindow);
		action->setText("About");
		action->setIcon(*QtConfig::GetIcon("chat.png"));
		menu->addAction(action);

		appWindows["About"] = new AboutWindow();

		QObject::connect(action, &QAction::triggered, this, [&]() {
			appWindows["About"]->Toggle();
		});
	}

	// ************************************************************************
	// Toolbar

	QToolBar *toolbar = new QToolBar();
	toolbar->setWindowTitle("Toolbar");

	// Play Audio
	{
		QToolButton *button = new QToolButton();
		button->setText("Play");
		button->setMaximumWidth(80);
		button->setIcon(*QtConfig::GetIcon("play.png"));
		button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
		QObject::connect(button, &QToolButton::clicked, this, []() {
			CSoundEditor::GetScene()->Play();
		});
		toolbar->addWidget(button);
	}

	// Stop Audio
	{
		QToolButton *button = new QToolButton();
		button->setText("Stop");
		button->setMaximumWidth(80);
		button->setIcon(*QtConfig::GetIcon("volume_disabled.png"));
		button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
		QObject::connect(button, &QToolButton::clicked, this, []() {
			CSoundEditor::GetScene()->Stop();
		});
		toolbar->addWidget(button);
	}

	// Save Scene
	{
		QToolButton *button = new QToolButton();
		button->setText("Save Scene");
		button->setIcon(*QtConfig::GetIcon("download.png"));
		button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
		QObject::connect(button, &QToolButton::clicked, this, []() {
			auto result = CSoundEditor::GetScene()->SaveScene();
			if (!result) {
				auto picker = GUI::Get<FilePicker>(QT_INSTACE::FILE_PICKER);
				if (picker) {
					picker->OpenForSave();
				}
			}
		});
		toolbar->addWidget(button);
	}

	// Load Scene
	{
		QToolButton *button = new QToolButton();
		button->setText("Load Scene");
		button->setIcon(*QtConfig::GetIcon("upload.png"));
		button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

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
		toolbar->addWidget(button);
	}

	// Headphone Test
	{
		QToolButton *button = new QToolButton();
		button->setText("Headphone Test");
		button->setIcon(*QtConfig::GetIcon("speaker.png"));
		button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

		appWindows["HeadphoneTest"] = new HeadphoneTestWindow();
		QObject::connect(button, &QToolButton::clicked, this, [this]() {
			appWindows["HeadphoneTest"]->Toggle();
		});

		toolbar->addWidget(button);
	}

	// Impact Sound
	{
		QToolButton *button = new QToolButton();
		button->setText("Impact Sound");
		button->setIcon(*QtConfig::GetIcon("focus.png"));
		button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

		appWindows["ImpactSound"] = new ImpactSoundWindow();
		QObject::connect(button, &QToolButton::clicked, this, [this]() {
			appWindows["ImpactSound"]->Toggle();
		});

		toolbar->addWidget(button);
	}

	// Moving Plane
	{
		QToolButton *button = new QToolButton();
		button->setText("Moving Plane");
		button->setIcon(*QtConfig::GetIcon("frames.png"));
		button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
		toolbar->addWidget(button);

		appWindows["MovingPlane"] = new MovingPlaneWindow();
		QObject::connect(button, &QToolButton::clicked, this, [this]() {
			appWindows["MovingPlane"]->Toggle();
		});
	}

	// Sweeping Plane
	{
		QToolButton *button = new QToolButton();
		button->setText("Horizontal Sweep");
		button->setIcon(*QtConfig::GetIcon("half-loading.png"));
		button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
		toolbar->addWidget(button);

		appWindows["SweepingPlane"] = new SweepingPlaneWindow();
		QObject::connect(button, &QToolButton::clicked, this, [this]() {
			appWindows["SweepingPlane"]->Toggle();
		});
	}

	// Expanding Sphere
	{
		QToolButton *button = new QToolButton();
		button->setText("Expanding Sphere");
		button->setIcon(*QtConfig::GetIcon("target.png"));
		button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

		appWindows["ExpandingPlane"] = new ExpandingSphereWindow();
		QObject::connect(button, &QToolButton::clicked, this, [this]() {
			appWindows["ExpandingPlane"]->Toggle();
		});

		toolbar->addWidget(button);
	}

	// Depth Perception
	{
		QToolButton *button = new QToolButton();
		button->setText("Depth Perception");
		button->setIcon(*QtConfig::GetIcon("countdown.png"));
		button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

		appWindows["DepthPerception"] = new DepthPerceptionWindow();
		QObject::connect(button, &QToolButton::clicked, this, [this]() {
			appWindows["DepthPerception"]->Toggle();
		});

		toolbar->addWidget(button);
	}

	// Padding Scene
	{
		QWidget *empty = new QWidget();
		empty->setObjectName("ToolWiteSpace");
		toolbar->addWidget(empty);
	}

	// Sound Output Mode
	{
		dropdown = new QComboBox();
		dropdown->setMinimumWidth(60);

		QStyledItemDelegate* itemDelegate = new QStyledItemDelegate();
		dropdown->setItemDelegate(itemDelegate);

		dropdown->addItem("None", QVariant(-1));
		dropdown->addItem("Mono", QVariant(0));
		dropdown->addItem("HRTF2", QVariant(1));
		dropdown->addItem("Stereo", QVariant(2));
		dropdown->addItem("ind-HRTF", QVariant(4));
		dropdown->addItem("PAN2", QVariant(8));
		dropdown->addItem("Quad-HRTF", QVariant(16));
		dropdown->addItem("Multi-8", QVariant(32));
		dropdown->addItem("Quad-Stereo", QVariant(64));

		// Default is HRTF
		dropdown->setCurrentIndex(2);
		SoundManager::SetGlobalOutputModelIndex(1);

		// Add widget
		auto widget = new CustomWidget(QBoxLayout::LeftToRight);
		widget->setObjectName("GlobalOutputDropdown");
		auto label = new QLabel("Global output");
		widget->AddWidget(label);
		widget->AddWidget(dropdown);
		toolbar->addWidget(widget);

		void (QComboBox::* indexChangedSignal)(int index) = &QComboBox::currentIndexChanged;
		QObject::connect(dropdown, indexChangedSignal, this, [&](int index) {
			if (index == 0) {
				SoundManager::SetGlobalOutputModelIndex(1024);
				CSoundEditor::GetScene()->SetOutputModel("none");
				return;
			} else if (SoundManager::GetGlobalOutputModelIndex() == 1024) {
				CSoundEditor::GetScene()->SetOutputModel("global-output");
			}
			auto data = dropdown->currentData().toUInt();
			SoundManager::SetGlobalOutputModelIndex(data);
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
	dockWindows["CodeEditor"] = new CodeEditorWindow();
	dockWindows["ComponentList"] = new ComponentList();
	dockWindows["InstrumentList"] = new InstrumentList();
	dockWindows["ScoreList"] = new ScoreList();

	dockWindows["ScoreEditor"] = new ScoreEditor();
	dockWindows["InstrumentEditor"] = new InstrumentEditor();
	dockWindows["ComponentEditor"] = new ComponentEditor();
	
	dockWindows["SceneWindow"] = new SceneWindow();
	dockWindows["GameWindow"] = new GameWindow();

	dockWindows["ObjectProperty"] = new SceneObjectProperties();
	dockWindows["CameraProperty"] = new CameraPropertyEditor();
	dockWindows["CSoundControl"] = new CSoundControlWindow();

	//// Left Dock
	MainWindow->addDockWidget(Qt::DockWidgetArea::LeftDockWidgetArea, dockWindows["SceneWindow"], Qt::Orientation::Vertical);
	MainWindow->addDockWidget(Qt::DockWidgetArea::LeftDockWidgetArea, dockWindows["ScoreEditor"], Qt::Orientation::Horizontal);
	MainWindow->addDockWidget(Qt::DockWidgetArea::LeftDockWidgetArea, dockWindows["InstrumentEditor"], Qt::Orientation::Horizontal);
	MainWindow->addDockWidget(Qt::DockWidgetArea::LeftDockWidgetArea, dockWindows["ComponentEditor"], Qt::Orientation::Vertical);
	MainWindow->tabifyDockWidget(dockWindows["ComponentEditor"], dockWindows["ObjectProperty"]);
	MainWindow->tabifyDockWidget(dockWindows["ComponentEditor"], dockWindows["CameraProperty"]);
	MainWindow->tabifyDockWidget(dockWindows["ComponentEditor"], dockWindows["CSoundControl"]);

	MainWindow->addDockWidget(Qt::DockWidgetArea::LeftDockWidgetArea, dockWindows["TextPreview"], Qt::Orientation::Horizontal);
	MainWindow->tabifyDockWidget(dockWindows["TextPreview"], dockWindows["CodeEditor"]);
	MainWindow->tabifyDockWidget(dockWindows["TextPreview"], dockWindows["GameWindow"]);

	dockWindows["GameWindow"]->setFloating(true);
	dockWindows["GameWindow"]->setFloating(false);
	dockWindows["GameWindow"]->raise();

	// Right Dock
	MainWindow->addDockWidget(Qt::DockWidgetArea::RightDockWidgetArea, dockWindows["ComponentList"], Qt::Orientation::Vertical);
	MainWindow->addDockWidget(Qt::DockWidgetArea::RightDockWidgetArea, dockWindows["InstrumentList"], Qt::Orientation::Vertical);
	MainWindow->addDockWidget(Qt::DockWidgetArea::RightDockWidgetArea, dockWindows["ScoreList"], Qt::Orientation::Vertical);

	// Activate Windows
	dockWindows["ObjectProperty"]->raise();
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

	for (auto &window : appWindows) {
		window.second->ReloadStyleSheet();
	}

	// Set general app stylesheet
	QFile File(CSoundEditor::GetStyleSheetFilePath("app.css").c_str());
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
