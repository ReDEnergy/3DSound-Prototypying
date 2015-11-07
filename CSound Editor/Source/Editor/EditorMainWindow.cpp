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

	setWindowIcon(QIcon("Resources/Icons/colorwheel.png"));

	MainWindow->setWindowTitle("Sound Engine (v0.1.8)");

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
		menu->setTitle("Scene");
		menuBar->addAction(menu->menuAction());

		// Submenu buttons
		QAction *action = new QAction(MainWindow);
		action->setText("Clear Scene");
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
		QAction *action = new QAction(MainWindow);
		action->setText("Reload StyleSheets");
		menu->addAction(action);
		QObject::connect(action, &QAction::triggered, this, &EditorMainWindow::ReloadStyleSheets);
	}

	// Menu Entry
	{
		QMenu *menu = new QMenu(menuBar);
		menu->setTitle("Help");
		menuBar->addAction(menu->menuAction());

		// Submenu buttons
		QAction *action = new QAction(MainWindow);
		action->setText("About");
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

	// Headphone Test
	{
		QToolButton *button = new QToolButton();
		button->setText("Headphone Test");
		button->setIcon(QIcon("Resources/Icons/speaker.png"));
		button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
		toolbar->addWidget(button);

		appWindows["HeadphoneTest"] = new HeadphoneTestWindow();
		QObject::connect(button, &QToolButton::clicked, this, [this]() {
			appWindows["HeadphoneTest"]->Toggle();
		});
	}

	// Moving Plane
	//{
	//	QToolButton *button = new QToolButton();
	//	button->setText("Moving Plane");
	//	toolbar->addWidget(button);

	//	appWindows["MovingPlane"] = new MovingPlaneWindow();
	//	QObject::connect(button, &QToolButton::clicked, this, [this]() {
	//		appWindows["MovingPlane"]->Toggle();
	//	});
	//}

	// Expanding Sphere
	{
		QToolButton *button = new QToolButton();
		button->setText("Expanding Sphere");
		button->setIcon(QIcon("Resources/Icons/target.png"));
		button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
		toolbar->addWidget(button);

		appWindows["ExpandingPlane"] = new ExpandingSphereWindow();
		QObject::connect(button, &QToolButton::clicked, this, [this]() {
			appWindows["ExpandingPlane"]->Toggle();
		});
	}

	// Padding Scene
	{
		QWidget *empty = new QWidget();
		empty->setMinimumWidth(600);
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
		dropdown->addItem("Stereo", QVariant("stereo-panning"));
		dropdown->addItem("PAN2", QVariant("pan2"));
		dropdown->setCurrentIndex(1);

		QWidget* widget = Wrap("Global output", 65, dropdown);
		toolbar->addWidget(widget);

		void (QComboBox::* indexChangedSignal)(int index) = &QComboBox::currentIndexChanged;
		QObject::connect(dropdown, indexChangedSignal, this, [&](int index) {
			auto data = dropdown->currentData().toString().toStdString();
			CSoundEditor::GetScene()->SetOutputModel(data.c_str());
			dockWindows["ScoreEditor"]->Update();
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

	for (auto &window : appWindows) {
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
