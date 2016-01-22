#pragma once
#include <vector>

#ifdef ENGINE_DLL_EXPORTS
#include <include/assimp.h>
#endif
#include <include/gl.h>
#include <include/glm.h>
#include <include/utils.h>

#include <Component/Mesh.h>

#include <unordered_map>

using namespace std;

#define NUM_BONES_PER_VEREX 4

class Shader;
class SkeletalJoint;
class Transform;

struct VertexBoneData
{
	unsigned int IDs[NUM_BONES_PER_VEREX];
	float Weights[NUM_BONES_PER_VEREX];

	VertexBoneData()
	{
		Reset();
	};

	void Reset()
	{
		ZERO_MEM(IDs);
		ZERO_MEM(Weights);
	}

	uint AddBoneData(uint BoneID, float Weight);
};


// TODO
// Move the aniamtion logic from the class to an animation class - that's rated to a game object
// Problem: Instantiating the same mesh with different animations is not possible right know
// Animation logic is liked to the SkinnedMesh - It shouldn't

class DLLExport SkinnedMesh : public Mesh
{
	public:
		SkinnedMesh(const char* meshID = NULL);
		~SkinnedMesh();

		bool LoadMesh(const std::string& fileLocation, const std::string& fileName);
		void ScaleAnimationTime(const string &animation, float timeScale);
		
		// Return a new copy of the Skeleton
		SkeletalJoint* GetSkeletonInfo(unordered_map<string, SkeletalJoint*> &joints) const;

		#ifdef ENGINE_DLL_EXPORTS
		aiAnimation * GetAnimation(const char *name) const;
		#endif

		uint GetNumberOfBones() const;
		glm::mat4 GetRootTransform() const;
		glm::mat4 GetGlobalInverse() const;

	private:
		#ifdef ENGINE_DLL_EXPORTS
		bool InitFromScene(const aiScene* pScene);
		void InitMesh(const aiMesh* paiMesh, uint index);
		#endif

	private:
		unordered_map<string, SkeletalJoint*> skeletalJoints;
		vector<VertexBoneData> boneData;
		vector<glm::mat4> boneTransform;
		unsigned short nrBones;

		// TODO - Support for multiple skinned meshes
		// Right now only a single ROOT joint is considered
		SkeletalJoint *rootJoint;
		glm::mat4 rootTransform;
		glm::mat4 globalInvTransform;

		#ifdef ENGINE_DLL_EXPORTS
		Assimp::Importer Importer;
		const aiScene* pScene;

		unordered_map<string, aiAnimation*> animations;
		aiAnimation *animationState;
		#endif
};