#include "pch.h"
#include "SoVInput.h"

SoVInput::SoVInput()
	: ObjectInput(InputGroup::IG_GAMEPLAY)
{

	auto x = Manager::GetScene()->activeObjects;

	audioEmitter = Manager::GetScene()->GetGameObject("oildrum", 1);
	audioEmitter->input = this;
	audioEmitter->transform->SetMoveSpeed(5.0f);
	moveEmitter = false;

	plane = 0;
}

void SoVInput::Update(float deltaTime, int mods)
{
	if (InputSystem::KeyHold(GLFW_KEY_KP_6))
		audioEmitter->transform->Move(glm::vec3_right, deltaTime);
	if (InputSystem::KeyHold(GLFW_KEY_KP_8))
		audioEmitter->transform->Move(glm::vec3_forward, deltaTime);
	if (InputSystem::KeyHold(GLFW_KEY_KP_4))
		audioEmitter->transform->Move(glm::vec3_left, deltaTime);
	if (InputSystem::KeyHold(GLFW_KEY_KP_5))
		audioEmitter->transform->Move(glm::vec3_backward, deltaTime);
	if (InputSystem::KeyHold(GLFW_KEY_KP_7))
		audioEmitter->transform->Move(glm::vec3_down, deltaTime);
	if (InputSystem::KeyHold(GLFW_KEY_KP_9))
		audioEmitter->transform->Move(glm::vec3_up, deltaTime);
}

void SetNewAudioSource(const string &name) {
	//audioEmitter->audioSource->Stop();
	//audioEmitter->SetAudioSource(Manager::GetAudio()->GetAudioSource(name));
	//audioEmitter->audioSource->SetLoop(true);
	//audioEmitter->audioSource->Play();
}

void SoVInput::OnKeyPress(int key, int mods)
{
	switch (key)
	{
	case GLFW_KEY_1:
		SetNewAudioSource("truck");
		break;
	case GLFW_KEY_2:
		SetNewAudioSource("wind");
		break;
	case GLFW_KEY_3:
		SetNewAudioSource("woodsaw");
		break;
	case GLFW_KEY_4:
		SetNewAudioSource("chainsaw");
		break;
	case GLFW_KEY_5:
		SetNewAudioSource("greynoise");
		break;
	case GLFW_KEY_6:
		SetNewAudioSource("bluenoise");
		break;
	case GLFW_KEY_7:
		SetNewAudioSource("brownnoise");
		break;
	case GLFW_KEY_8:
		SetNewAudioSource("whitenoise");
		break;
	case GLFW_KEY_9:
		SetNewAudioSource("pinknoise");
		break;
	case GLFW_KEY_0:
		SetNewAudioSource("violetnoise");
		break;
	default:
		break;
	}
}

void SoVInput::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
	glm::vec2 diff;
	if (plane == 1) {
		diff = glm::vec2(glm::ivec2(mouseX, mouseY) - center[plane]);
		diff /= 30.0f;
		audioEmitter->transform->SetWorldPosition(glm::vec3(diff.x, audioEmitter->transform->GetWorldPosition().y, diff.y));
		soundPos[0] = glm::ivec2(mouseX, mouseY);
		soundPos[1].x = mouseX + Engine::Window->resolution.x / 2;
	}
	if (plane == 2) {
		diff = glm::vec2(center[plane] - glm::ivec2(mouseX, mouseY));
		diff /= 30.0f;
		audioEmitter->transform->SetWorldPosition(glm::vec3(-diff.x, diff.y, audioEmitter->transform->GetWorldPosition().z));
		soundPos[0].x = mouseX - Engine::Window->resolution.x / 2;
		soundPos[1] = glm::ivec2(mouseX, mouseY);
	}
	if (plane) {
		//cout << plane << " " << mouseX << " " << mouseY << " " << diff << endl;
	}
}

void SoVInput::OnMouseBtnEvent(int mouseX, int mouseY, int button, int action, int mods)
{
	moveEmitter = (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_PRESS && mods == 0);
	if (moveEmitter) {
		auto rez = Engine::Window->resolution;
		plane = (mouseX < (rez.x / 2)) ? 1 : 2;
		center[plane] = glm::ivec2((2 * plane - 1) * (rez.x / 4), rez.y / 2);
	} else {
		plane = 0;
	}
}

void SoVInput::BlendGUI(GLuint textureID)
{
	Shader *sha = Manager::GetShader()->GetShader("SoVInput");
	sha->Use();

	glUniform2i(sha->GetUniformLocation("resolution"), Engine::Window->resolution.x, Engine::Window->resolution.y);
  	glUniform2i(sha->GetUniformLocation("leftCuePosition"), soundPos[0].x, soundPos[0].y);
	glUniform2i(sha->GetUniformLocation("rightCuePosition"), soundPos[1].x, soundPos[1].y);
	glBindImageTexture(0, textureID, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

	{
		glDispatchCompute(GLuint(UPPER_BOUND(Engine::Window->resolution.x, 16)), GLuint(UPPER_BOUND(Engine::Window->resolution.y, 16)), 1);
		glFinish();
		glMemoryBarrier(GL_ALL_BARRIER_BITS);
	}
}
