#pragma once
#include "Defines.h"
#include "Types.h"
#include <string>
#include <vector>
#include <glm/glm.hpp>

namespace Lambda
{
	struct Vertex
	{
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec2 TexCoord;

		bool operator==(const Vertex& other) const;
		bool operator!=(const Vertex& other) const;
	};


	inline bool Vertex::operator==(const Vertex& other) const
	{
		return	Position	== other.Position	&&
				Normal		== other.Normal		&&
				TexCoord	== other.TexCoord;
	}


	inline bool Vertex::operator!=(const Vertex& other) const
	{
		return !(*this == other);
	}


	struct MeshData
	{
		std::vector<Vertex> Vertices;
		std::vector<uint32> Indices;
	};


	class LAMBDA_API MeshFactory
	{
	public:
		LAMBDA_STATIC_CLASS(MeshFactory);

		static MeshData CreateFromFile(const std::string& filename, bool rightHanded = true) noexcept;
		static MeshData CreateCube(float width = 1.0f, float height = 1.0f, float depth = 1.0f) noexcept;
		static MeshData CreatePlane(uint32 width = 1, uint32 height = 1) noexcept;
		static MeshData CreateSphere(uint32 subdivisions = 0, float radius = 0.5f) noexcept;
		static MeshData CreateCone(uint32 sides = 5, float radius = 0.5f, float height = 1.0f) noexcept;
		//static MeshData createTorus() noexcept;
		//static MeshData createTeapot() noexcept;
		static MeshData CreatePyramid() noexcept; 
		static MeshData CreateCylinder(uint32 sides = 5, float radius = 0.5f, float height = 1.0f) noexcept;

		static void Subdivide(MeshData& data, uint32 subdivisions = 1) noexcept;
		static void Optimize(MeshData& data, uint32 startVertex = 0) noexcept;
		static void CalculateHardNormals(MeshData& data) noexcept;
	};
}
