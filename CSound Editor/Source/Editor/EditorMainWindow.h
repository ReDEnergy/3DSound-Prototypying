#pragma once
#include <iostream>
#include <string>
#include <unordered_map>

#include <QMainWindow>
#include <QThread>
#include <Editor/QTForward.h>

using namespace std;

class DockWindow;
class CustomWidget;
class GameWindow;

class EditorMainWindow : public QMainWindow
{
	protected:
		EditorMainWindow(QWidget *parent = 0);
		~EditorMainWindow();

	public:
		void Run(QApplication * app);
		void Update();

	private:
		void close();
		void closeEvent(QCloseEvent *event);
		void Config();

		void SetupUI(QMainWindow * MainWindow);
		void TestEvent();

		// Toolbar actions
		void ReloadStyleSheets();

	private:
		bool shouldQuit = false;
		QTime *qtTime;
		QTimer *qtTimer;
		QApplication *app;
		QComboBox *dropdown;

		unordered_map<string, DockWindow*> dockWindows;
		unordered_map<string, CustomWidget*> appWindows;
};

class EngineThread : public QThread 
{
	public:
		void run();
};