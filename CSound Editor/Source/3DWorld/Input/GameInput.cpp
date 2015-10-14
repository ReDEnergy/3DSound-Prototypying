#include "GameInput.h"

#include <3DWorld/Game.h>
#include <3DWorld/CSound/CSound3DSource.h>

#include <Manager/Manager.h>
#include <Manager/SceneManager.h>
#include <UI/ColorPicking/ColorPicking.h>
#include <Manager/EventSystem.h>
#include <Core/InputSystem.h>
#include <Core/Camera/Camera.h>
#include <Component/Transform/Transform.h>


GameInput::GameInput(Game *game)
	: ObjectInput(InputGroup::IG_GAMEPLAY), game(game)
{
}

void GameInput::Update(float deltaTime, int mods)
{
	Camera *activeCamera = Manager::GetScene()->GetActiveCamera();

	if (InputSystem::KeyHold(GLFW_KEY_RIGHT))
		activeCamera->MoveRight(deltaTime);
	if (InputSystem::KeyHold(GLFW_KEY_UP))
		activeCamera->MoveForward(deltaTime);
	if (InputSystem::KeyHold(GLFW_KEY_LEFT))
		activeCamera->MoveRight(-deltaTime);
	if (InputSystem::KeyHold(GLFW_KEY_DOWN))
		activeCamera->MoveBackward(deltaTime);

	if (activeCamera->transform->GetMotionState())
		activeCamera->Update();
}

inline ostream &operator<< (ostream &out, const glm::mediump_vec3 &vec) {
	out << "[" << vec.x << ", " << vec.y << ", " << vec.z << "]";
	return out;
}

void GameInput::OnKeyPress(int key, int mods)
{
	if (key == GLFW_KEY_ESCAPE) {
		Manager::GetEvent()->EmitAsync(EventType::OPEN_GAME_MENU, nullptr);
		return;
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