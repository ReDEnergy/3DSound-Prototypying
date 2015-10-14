#include "PlaneIntersection.h"

#include <include/utils.h>
#include <include/math.h>

#include <Core/Engine.h>
#include <Core/Camera/Camera.h>
#include <Core/WindowObject.h>
#include <Component/Transform/Transform.h>

#include <Manager/Manager.h>
#include <Manager/ShaderManager.h>
#include <UI/ColorPicking/ColorPicking.h>

#include <GPU/Shader.h>
#include <GPU/SSBO.h>
#include <GPU/Texture.h>
#include <GPU/FrameBuffer.h>

PlaneIntersection::PlaneIntersection()
{
}

void PlaneIntersection::Update(const Transform* const planeTransform)
{
	if (planeTransform->GetMotionState() == false) return;
}