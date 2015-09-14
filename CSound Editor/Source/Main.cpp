#include <iostream>
#include <stdio.h>

#include <Editor/EditorMainWindow.h>

#include <QApplication>

using namespace std;

class CSoundEditorApp : public QApplication
{
public:
	CSoundEditorApp(int argc, char *argv[])
		: QApplication(argc, argv)
	{
	}

	void Init() {
		EditorMainWindow window;
		window.Run(this);
		//this->exec();
	};

	void lastWindowClosed() {
		cout << "LAST WINDOW CLOSED" << endl;
		int a;
		cin >> a;
	};
};

int main(int argc, char *argv[])
{
	CSoundEditorApp app(argc, argv);
	app.Init();
}