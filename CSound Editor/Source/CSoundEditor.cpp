#include "CSoundEditor.h"

#include <templates/singleton.h>

#include <3DWorld/CSound/CSoundScene.h>
#include <3DWorld/Game.h>

#include <Core/Engine.h>

CSoundScene* CSoundEditor::Scene = nullptr;
Game * CSoundEditor::game = nullptr;


void CSoundEditor::Init()
{
	Scene = new CSoundScene();
	Scene->Init();

	game = new Game();
	game->Init();

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
