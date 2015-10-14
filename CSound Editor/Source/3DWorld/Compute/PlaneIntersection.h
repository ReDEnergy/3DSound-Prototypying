#pragma once

class Transform;

class PlaneIntersection
{
	public:
		PlaneIntersection();
		~PlaneIntersection() {};
		void Update(const Transform* const planeTransform);
};