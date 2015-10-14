#pragma once

class Texture;
class Camera;

template <class T>
class SSBO;

class SurfaceArea
{
	public:
		SurfaceArea();
		~SurfaceArea() {};
		void Update(const Camera* const camera);
		unsigned int GetValue(unsigned int ID) const;

	public:
		void ReadData();

	private:
		SSBO<unsigned int> *ssbo;
		const unsigned int *counter;
};