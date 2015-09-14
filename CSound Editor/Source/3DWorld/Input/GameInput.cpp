#include "GameInput.h"

#include <3DWorld/Game.h>
#include <Manager/Manager.h>
#include <Manager/SceneManager.h>
#include <Manager/EventSystem.h>
#include <Core/InputSystem.h>
#include <Core/Camera/Camera.h>


GameInput::GameInput(Game *game)
	: ObjectInput(InputGroup::IG_GAMEPLAY), game(game)
{
}

void GameInput::Update(float deltaTime, int mods) {

	Camera *activeCamera = Manager::GetScene()->GetActiveCamera();

	if (InputSystem::KeyHold(GLFW_KEY_RIGHT))
		activeCamera->MoveRight(deltaTime);
	if (InputSystem::KeyHold(GLFW_KEY_UP))
		activeCamera->MoveForward(deltaTime);
	if (InputSystem::KeyHold(GLFW_KEY_LEFT))
		activeCamera->MoveRight(-deltaTime);
	if (InputSystem::KeyHold(GLFW_KEY_DOWN))
		activeCamera->MoveBackward(deltaTime);

	activeCamera->Update();
}

void GameInput::OnKeyPress(int key, int mods)
{
	if (key == GLFW_KEY_ESCAPE) {
		Manager::GetEvent()->EmitSync(EventType::OPEN_GAME_MENU, nullptr);
	}
}

void GameInput::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
//	cout << mouseY << "-" << mouseX << endl;
}

void GameInput::OnMouseBtnEvent(int mouseX, int mouseY, int button, int action, int mods)
{
	//printf("pos: %d %d \n button: %d \n action: %d \n mods: %d \n", mouseX, mouseY, button, action, mods);
}