#pragma once

class Transform;
class Shader;
class FrameBuffer;
class Texture;
template <class T>
class SSBO;

#include <include/glm.h>
#include <Event/EventListener.h>

class PlaneIntersection
	: public EventListener
{
	public:
		PlaneIntersection();
		~PlaneIntersection() {};

		void SetPlaneTransform(const Transform* const planeTransform);

	private:
		void Update();
		void OnEvent(EventType eventID, void *data);

	private:
		SSBO<glm::vec3> *ssbo;

		const Transform* planeTransform;
		Shader *computeShader;
		unsigned int plane_origin;
		unsigned int plane_direction;
		FrameBuffer *gameFBO;
		Texture* visualization;
};