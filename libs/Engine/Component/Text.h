#pragma once
#include <string>

#include <Core/GameObject.h>

class DLLExport Text : virtual public GameObject
{
	public:
		Text();
		virtual ~Text();

		const char* GetText() const;

		void SetColor(glm::vec3& color);
		void SetText(const string &text);
		void AppendText(const char *text);
		void Render(const Shader *shader) const;

	private:
		void InitText();

	public:
		Transform *offset;
		string content;

	private:
		glm::vec3 color;
		uint atlasTextureID;
};

