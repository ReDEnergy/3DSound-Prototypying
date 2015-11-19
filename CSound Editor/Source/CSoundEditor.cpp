#include "CSoundEditor.h"

#include <templates/singleton.h>

#include <3DWorld/CSound/CSoundScene.h>
#include <3DWorld/Game.h>

#include <Core/Engine.h>

CSoundScene* CSoundEditor::Scene = nullptr;
Game * CSoundEditor::game = nullptr;


namespace RESOURCE_PATH {
	const string ROOT = "Resources\\";
	const string CONFIG = "Config\\";
	const string QTICONS = "Resources/Icons/";
	const string STYLESHEET = CONFIG + "StyleSheets\\";
}

void CSoundEditor::Init()
{
	Scene = new CSoundScene();
	game = new Game();
	game->Init();

	Scene->Init();

	Engine::SetWorldInstance(game);
}

CSoundScene * CSoundEditor::GetScene()
{
	return Scene;
}

Game * CSoundEditor::GetGame()
{
	return game;
}

string CSoundEditor::GetStyleSheetFilePath(const char * fileName)
{
	return RESOURCE_PATH::STYLESHEET + fileName;
}
