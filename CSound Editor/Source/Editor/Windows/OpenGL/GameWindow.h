#pragma once
#include <Editor/Windows/Interface/DockWindow.h>
#include <Editor/EditorForward.h>

#include <include/gl.h>
#include <QtWidgets/QOpenGLWidget>

class Game;

class GameWindow : public DockWindow
{
	public:
		GameWindow();
		virtual ~GameWindow() {};

		void Init();
		void Update();
		void Render();
		void InitRendering();

	private:
		void InitUI();

	protected:
		void DockedEvent(bool state);

	private:
		bool useContext;
		OpenGLWindow *qtOpenGLWindow;
		QGLWidget *widget;
		Game *game;
};
