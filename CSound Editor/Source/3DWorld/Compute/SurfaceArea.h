#pragma once

class Texture;
class Camera;
class Shader;

template <class T>
class SSBO;

#include <Event/EventListener.h>

class SurfaceArea
	: public EventListener
{
	public:
		SurfaceArea();
		~SurfaceArea() {};
		unsigned int GetValue(unsigned int ID) const;

	private:
		void Update();
		void ReadData();
		void OnEvent(EventType Event, void *data);

	private:
		SSBO<unsigned int> *ssbo;
		const unsigned int *counter;
		const Camera* gameCamera;
		const Shader* computeShader;
};