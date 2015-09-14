#pragma once

#include <Engine.h>

using namespace std;

class SoVInput : public ObjectInput {
	public:
		SoVInput();
		void Update(float deltaTime, int mods);
		void OnKeyPress(int key, int mods);
		void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY);
		void OnMouseBtnEvent(int mouseX, int mouseY, int button, int action, int mods);

		void BlendGUI(GLuint textureID);

	public:
		GameObject* audioEmitter;

	private:
		int plane;
		glm::ivec2 soundPos[2];
		glm::ivec2 center[2];
		bool moveEmitter;
};