#pragma once
#include "Defines.h"
#include "Types.h"
#include <string>
#include <vector>
#include <glm/glm.hpp>

namespace Lambda
{
    //-------
    //SVertex
    //-------
    
	struct SVertex
	{
		glm::vec3 Position;
		glm::vec3 Normal;
        glm::vec3 Tangent;
		glm::vec2 TexCoord;

		
		_forceinline bool operator==(const SVertex& other) const
		{
			return	Position == other.Position &&
				Normal == other.Normal &&
				Tangent == other.Tangent &&
				TexCoord == other.TexCoord;
		}


		_forceinline bool operator!=(const SVertex& other) const
		{
			return !(*this == other);
		}
	};

    //---------
    //SMeshData
    //---------

	struct SMeshData
	{
		std::vector<SVertex> Vertices;
		std::vector<uint32> Indices;
	};

    //------------
    //CMeshFactory
    //------------

	class LAMBDA_API CMeshFactory
	{
	public:
		LAMBDA_STATIC_CLASS(CMeshFactory);

		static SMeshData CreateFromFile(const std::string& filename, bool mergeMeshes = true, bool leftHanded = true) noexcept;
		static SMeshData CreateCube(float width = 1.0f, float height = 1.0f, float depth = 1.0f) noexcept;
		static SMeshData CreatePlane(uint32 width = 1, uint32 height = 1) noexcept;
		static SMeshData CreateSphere(uint32 subdivisions = 0, float radius = 0.5f) noexcept;
		static SMeshData CreateCone(uint32 sides = 5, float radius = 0.5f, float height = 1.0f) noexcept;
		//static MeshData createTorus() noexcept;
		//static MeshData createTeapot() noexcept;
		static SMeshData CreatePyramid() noexcept; 
		static SMeshData CreateCylinder(uint32 sides = 5, float radius = 0.5f, float height = 1.0f) noexcept;

		static void Subdivide(SMeshData& data, uint32 subdivisions = 1) noexcept;
		static void Optimize(SMeshData& data, uint32 startVertex = 0) noexcept;
		static void CalculateHardNormals(SMeshData& data) noexcept;
		static void CalculateTangents(SMeshData& data) noexcept;
	};
}
