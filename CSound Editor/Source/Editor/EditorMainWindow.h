#pragma once
#include <iostream>
#include <string>
#include <unordered_map>

#include <QMainWindow>
#include <QThread>
#include <Editor/QTForward.h>

using namespace std;

class DockWindow;
class GameWindow;

class EditorMainWindow : public QMainWindow
{
	public:
		EditorMainWindow(QWidget *parent = 0);
		~EditorMainWindow();
		void Run(QApplication * app);

	private:
		void close();
		void closeEvent(QCloseEvent *event);
		void Update();
		void Config();

		void SetupUI(QMainWindow * MainWindow);
		void TestEvent();

		// Toolbar actions
		void ReloadStyleSheets();
		void ChnageOuputModel();

	private:
		bool shouldQuit = false;
		QTime *qtTime;
		QTimer *qtTimer;
		QApplication *app;
		QComboBox *dropdown;

	private:
		unordered_map<string, DockWindow*> appWindows;
};

class EngineThread : public QThread 
{
	public:
		void run();
};