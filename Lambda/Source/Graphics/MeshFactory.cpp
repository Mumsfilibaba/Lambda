#include "LambdaPch.h"
#include "Graphics/MeshFactory.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#define GLM_FORCE_RADIANS
#include <glm/gtc/constants.hpp>

namespace Lambda
{
	//-----------
	//MeshFactory
	//-----------

	MeshData MeshFactory::CreateFromFile(const std::string& filename, bool mergeMeshes, bool leftHanded) noexcept
	{
		using namespace std;
        using namespace glm;

		//Set import flags
		uint32 flags = aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_ImproveCacheLocality | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace;
		if (leftHanded)
		{
			flags |= aiProcess_ConvertToLeftHanded;
		}
		flags = flags & ~aiProcess_FlipWindingOrder;

		//Load scene
        Assimp::Importer importer;
        const aiScene* pScene = importer.ReadFile(filename, flags);
        
		//Extract scene-data
		MeshData data;
        if (pScene)
        {
            if(pScene->HasMeshes())
            {
                LOG_DEBUG_INFO("[LAMBDA ENGINE] Loading Scene with '%u' meshes\n", pScene->mNumMeshes);
                
				size_t vertexOffset = 0;
				size_t indexOffset = 0;
				uint32 numMeshesToLoad = (mergeMeshes) ? pScene->mNumMeshes : 1;
				for (uint32 m = 0; m < numMeshesToLoad; m++)
				{
					const aiMesh* pMesh = pScene->mMeshes[m];
					if (!pMesh->HasNormals())
					{
						LOG_DEBUG_WARNING("[LAMBDA ENGINE] Mesh does not have normals\n");
					}
					if (!pMesh->HasTextureCoords(0))
					{
						LOG_DEBUG_WARNING("[LAMBDA ENGINE] Mesh does not have texcoords\n");
					}

					if (pMesh)
					{
						if (pMesh->HasFaces())
						{
							//Get number of vertices and resize buffer
							size_t vertCount = pMesh->mNumVertices;
							//Vertexoffset is used when there are more than one mesh in the scene and we want to merge all the meshes
							data.Vertices.resize(vertexOffset + vertCount);
							for (size_t i = 0; i < vertCount; i++)
							{
								data.Vertices[vertexOffset + i].Position = vec3(pMesh->mVertices[i].x, pMesh->mVertices[i].y, pMesh->mVertices[i].z);
								if (pMesh->HasNormals())
								{
									data.Vertices[vertexOffset + i].Normal = vec3(pMesh->mNormals[i].x, pMesh->mNormals[i].y, pMesh->mNormals[i].z);
								}
								if (pMesh->HasTangentsAndBitangents())
								{
									data.Vertices[vertexOffset + i].Tangent = vec3(pMesh->mTangents[i].x, pMesh->mTangents[i].y, pMesh->mTangents[i].z);
								}
								if (pMesh->HasTextureCoords(0))
								{
									data.Vertices[vertexOffset + i].TexCoord = vec2(pMesh->mTextureCoords[0][i].x, pMesh->mTextureCoords[0][i].y);
								}
							}
                        
							//Get number of indices and resize indexbuffer
							size_t triCount = pMesh->mNumFaces;
							//Indexoffset is used when there are more than one mesh in the scene and we want to merge all the meshes
							data.Indices.resize(indexOffset + (triCount * 3));
							for (size_t i = 0; i < triCount; i++)
							{
								data.Indices[indexOffset + (i*3) + 0] = uint32(vertexOffset + pMesh->mFaces[i].mIndices[0]);
								data.Indices[indexOffset + (i*3) + 1] = uint32(vertexOffset + pMesh->mFaces[i].mIndices[1]);
								data.Indices[indexOffset + (i*3) + 2] = uint32(vertexOffset + pMesh->mFaces[i].mIndices[2]);
							}

							//Increase offsets
							if (mergeMeshes)
							{
								vertexOffset	+= vertCount;
								indexOffset		+= triCount * 3;
							}
						}
					}
				}
            }
            else
            {
                LOG_DEBUG_ERROR("[LAMBDA ENGINE] File '%s' does not have any meshes\n", filename.c_str());
				return data;
            }
        }
        else
        {
			const char* pErrorMessage = importer.GetErrorString();
            LOG_SYSTEM_PRINT("[LAMBDA ENGINE] Failed to load file '%s'. Message: %s\n", filename.c_str(), pErrorMessage);
			return data;
        }
        
		LOG_SYSTEM_PRINT("[LAMBDA ENGINE] Loaded mesh with %d vertices and %d indices. Triangles: %d\n", data.Vertices.size(), data.Indices.size(), data.Indices.size() / 3);
		return data;
	}


	MeshData MeshFactory::CreateCube(float width, float height, float depth) noexcept
	{
		using namespace glm;

		float w = width  * 0.5f;
		float h = height * 0.5f;
		float d = depth  * 0.5f;

		MeshData data;
		data.Vertices =
		{
			//FRONT FACE
			{ vec3(-w,  h, -d), vec3(0.0f,  0.0f, -1.0f), vec3(1.0f,  0.0f, 0.0f), vec2(0.0f, 0.0f) },
			{ vec3( w,  h, -d), vec3(0.0f,  0.0f, -1.0f), vec3(1.0f,  0.0f, 0.0f), vec2(1.0f, 0.0f) },
			{ vec3(-w, -h, -d), vec3(0.0f,  0.0f, -1.0f), vec3(1.0f,  0.0f, 0.0f), vec2(0.0f, 1.0f) },
			{ vec3( w, -h, -d), vec3(0.0f,  0.0f, -1.0f), vec3(1.0f,  0.0f, 0.0f), vec2(1.0f, 1.0f) },

			//BACK FACE
			{ vec3( w,  h,  d), vec3(0.0f,  0.0f,  1.0f), vec3(-1.0f,  0.0f, 0.0f), vec2(0.0f, 0.0f) },
			{ vec3(-w,  h,  d), vec3(0.0f,  0.0f,  1.0f), vec3(-1.0f,  0.0f, 0.0f), vec2(1.0f, 0.0f) },
			{ vec3( w, -h,  d), vec3(0.0f,  0.0f,  1.0f), vec3(-1.0f,  0.0f, 0.0f), vec2(0.0f, 1.0f) },
			{ vec3(-w, -h,  d), vec3(0.0f,  0.0f,  1.0f), vec3(-1.0f,  0.0f, 0.0f), vec2(1.0f, 1.0f) },

			//RIGHT FACE
			{ vec3( w,  h, -d), vec3(1.0f,  0.0f,  0.0f), vec3(0.0f,  0.0f, 1.0f), vec2(0.0f, 0.0f) },
			{ vec3( w,  h,  d), vec3(1.0f,  0.0f,  0.0f), vec3(0.0f,  0.0f, 1.0f), vec2(1.0f, 0.0f) },
			{ vec3( w, -h, -d), vec3(1.0f,  0.0f,  0.0f), vec3(0.0f,  0.0f, 1.0f), vec2(0.0f, 1.0f) },
			{ vec3( w, -h,  d), vec3(1.0f,  0.0f,  0.0f), vec3(0.0f,  0.0f, 1.0f), vec2(1.0f, 1.0f) },

			//LEFT FACE
			{ vec3(-w,  h, -d), vec3(-1.0f,  0.0f,  0.0f), vec3(0.0f,  0.0f, -1.0f), vec2(0.0f, 0.0f) },
			{ vec3(-w,  h,  d), vec3(-1.0f,  0.0f,  0.0f), vec3(0.0f,  0.0f, -1.0f), vec2(1.0f, 0.0f) },
			{ vec3(-w, -h, -d), vec3(-1.0f,  0.0f,  0.0f), vec3(0.0f,  0.0f, -1.0f), vec2(0.0f, 1.0f) },
			{ vec3(-w, -h,  d), vec3(-1.0f,  0.0f,  0.0f), vec3(0.0f,  0.0f, -1.0f), vec2(1.0f, 1.0f) },

			//TOP FACE
			{ vec3(-w,  h,  d), vec3(0.0f,  1.0f,  0.0f), vec3(1.0f,  0.0f, 0.0f), vec2(0.0f, 0.0f) },
			{ vec3( w,  h,  d), vec3(0.0f,  1.0f,  0.0f), vec3(1.0f,  0.0f, 0.0f), vec2(1.0f, 0.0f) },
			{ vec3(-w,  h, -d), vec3(0.0f,  1.0f,  0.0f), vec3(1.0f,  0.0f, 0.0f), vec2(0.0f, 1.0f) },
			{ vec3( w,  h, -d), vec3(0.0f,  1.0f,  0.0f), vec3(1.0f,  0.0f, 0.0f), vec2(1.0f, 1.0f) },

			//BOTTOM FACE
			{ vec3(-w, -h, -d), vec3(0.0f, -1.0f,  0.0f), vec3(-1.0f,  0.0f, 0.0f), vec2(0.0f, 0.0f) },
			{ vec3( w, -h, -d), vec3(0.0f, -1.0f,  0.0f), vec3(-1.0f,  0.0f, 0.0f), vec2(1.0f, 0.0f) },
			{ vec3(-w, -h,  d), vec3(0.0f, -1.0f,  0.0f), vec3(-1.0f,  0.0f, 0.0f), vec2(0.0f, 1.0f) },
			{ vec3( w, -h,  d), vec3(0.0f, -1.0f,  0.0f), vec3(-1.0f,  0.0f, 0.0f), vec2(1.0f, 1.0f) },
		};

		data.Indices =
		{
			//FRONT FACE
			2, 1, 0,
			2, 3, 1,

			//BACK FACE
			6, 5, 4,
			6, 7, 5,

			//RIGHT FACE
			10, 9, 8,
			10, 11, 9,

			//LEFT FACE
			12, 13, 14,
			13, 15, 14,

			//TOP FACE
			18, 17, 16,
			18, 19, 17,

			//BOTTOM FACE
			22, 21, 20,
			22, 23, 21
		};

		return data;
	}


	MeshData MeshFactory::CreatePlane(uint32 width, uint32 height) noexcept
	{
		using namespace glm;
		using namespace std;

		MeshData data;
		if (width < 1)
			width = 1;
		if (height < 1)
			height = 1;

		data.Vertices.resize((width + 1) * (height + 1));
		data.Indices.resize((width * height) * 6);

		//Size of each quad, size of the plane will always be between -0.5 and 0.5
		vec2 quadSize = vec2(1.0f / float(width), 1.0f / float(height));
		vec2 uvQuadSize = vec2(1.0f / float(width), 1.0f / float(height));

		//Initialize Vertices
		for (uint32 x = 0; x <= width; x++)
		{
			for (uint32 y = 0; y <= height; y++)
			{
				int32 v = ((1 + height) * x) + y;
				data.Vertices[v].Position	= vec3(0.5f - (quadSize.x * x), 0.5f - (quadSize.y * y), 0.0f);
				data.Vertices[v].Normal		= vec3(0.0f, 0.0f, 1.0f);
                data.Vertices[v].Tangent    = vec3(1.0f, 0.0f, 0.0f);
				data.Vertices[v].TexCoord	= vec2(0.0f + (uvQuadSize.x * x), 0.0f + (uvQuadSize.y * y));
			}
		}

		//Initialize Indices
		for (uint8 x = 0; x < width; x++)
		{
			for (uint8 y = 0; y < height; y++)
			{
				int32 quad = (height * x) + y;
				data.Indices[(quad * 6) + 0] = (x * (1 + height)) + y + 1;
				data.Indices[(quad * 6) + 1] = (data.Indices[quad * 6] + 2 + (height - 1));
				data.Indices[(quad * 6) + 2] =  data.Indices[quad * 6] - 1;
				data.Indices[(quad * 6) + 3] =  data.Indices[(quad * 6) + 1];
				data.Indices[(quad * 6) + 4] =  data.Indices[(quad * 6) + 1] - 1;
				data.Indices[(quad * 6) + 5] =  data.Indices[(quad * 6) + 2];
			}
		}

        //Resize array
		data.Vertices.shrink_to_fit();
		data.Indices.shrink_to_fit();
                
		return data;
	}


	MeshData MeshFactory::CreateSphere(uint32 subdivisions, float radius) noexcept
	{
		using namespace glm;

		MeshData data;
		data.Vertices.resize(12);

		//VERTICES
		float t = (1.0f + sqrt(5.0f)) / 2.0f;
		data.Vertices[0].Position	= vec3(-1.0f, t, 0.0f);
		data.Vertices[1].Position	= vec3( 1.0f, t, 0.0f);
		data.Vertices[2].Position	= vec3(-1.0f, -t, 0.0f);
		data.Vertices[3].Position	= vec3( 1.0f, -t, 0.0f);
		data.Vertices[4].Position	= vec3(0.0f, -1.0f, t);
		data.Vertices[5].Position	= vec3(0.0f,  1.0f, t);
		data.Vertices[6].Position	= vec3(0.0f, -1.0f, -t);
		data.Vertices[7].Position	= vec3(0.0f,  1.0f, -t);
		data.Vertices[8].Position	= vec3(t, 0.0f, -1.0f);
		data.Vertices[9].Position	= vec3(t, 0.0f, 1.0f);
		data.Vertices[10].Position	= vec3(-t, 0.0f, -1.0f);
		data.Vertices[11].Position	= vec3(-t, 0.0f, 1.0f);

		//INDICIES
		data.Indices =
		{
			5, 11, 0,
			1, 5, 0,
			7, 1, 0,
			10, 7, 0,
			11, 10, 0,

			9, 5, 1,
			4, 11, 5,
			2, 10, 11,
			6, 7, 10,
			8, 1, 7,

			4, 9, 3,
			2, 4, 3,
			6, 2, 3,
			8, 6, 3,
			9, 8, 3,

			5, 9, 4,
			11, 4, 2,
			10, 2, 6,
			7, 6, 8,
			1, 8, 9
		};

		if (subdivisions > 0)
		{
			Subdivide(data, subdivisions);
		}

		for (uint32 i = 0; i < data.Vertices.size(); i++)
		{
			data.Vertices[i].Position	= normalize(data.Vertices[i].Position);
			data.Vertices[i].Normal		= data.Vertices[i].Position;

			//Calculate uvs
			vec2 uv;
			uv.y = (asin(data.Vertices[i].Position.y) / pi<float>()) + 0.5f;
			uv.x = (atan2f(data.Vertices[i].Position.z, data.Vertices[i].Position.x) + pi<float>()) / (2.0f * pi<float>());
			data.Vertices[i].TexCoord = uv;

			//Scale down the sphere
			data.Vertices[i].Position *= radius;
		}

		CalculateTangents(data);

		data.Indices.shrink_to_fit();
		data.Vertices.shrink_to_fit();

		return data;
	}


	MeshData MeshFactory::CreateCone(uint32 sides, float radius, float height) noexcept
	{
		using namespace glm;

		MeshData data;
		//Num verts = (Sides*2)	(Bottom, since we need unique normals)
		//		    +  Sides	(1 MiddlePoint per side)
		//			+  1		(One middlepoint on the underside)
		size_t vertSize = size_t(sides) * 3 + 1;
		data.Vertices.resize(vertSize);

		//Num indices = (Sides*3*2) (Cap has 'sides' number of tris + sides tris for the sides, each tri has 3 verts)
		size_t indexSize = size_t(sides) * 6;
		data.Indices.resize(indexSize);

		//Angle between verts
		float angle = (pi<float>() * 2.0f) / float(sides);
		float uOffset = 1.0f / float(sides - 1);

		//CREATE VERTICES
		data.Vertices[0].Position	= vec3(0.0f, 0.0f, 0.0f);
		data.Vertices[0].Normal		= vec3(0.0f, -1.0f, 0.0f);
		data.Vertices[0].TexCoord	= vec2(0.25f, 0.25f);

		size_t offset = size_t(sides) + 1;
		size_t topOffset = offset + size_t(sides);
		for (size_t i = 0; i < sides; i++)
		{
			//BOTTOM CAP VERTICES
			float x = cosf((pi<float>() / 2.0f) + (angle * i));
			float z = sinf((pi<float>() / 2.0f) + (angle * i));

			vec3 pos = normalize(vec3(x, 0.0f, z));
			data.Vertices[i + 1].Position	= (pos * radius);
			data.Vertices[i + 1].Normal		= vec3(0.0f, -1.0f, 0.0f);
			data.Vertices[i + 1].TexCoord	= (vec2(x + 1.0f, z + 1.0f) * 0.25f);

			//BOTTOM SIDE VERTICES
			vec3 normal = normalize(pos + vec3(0.0f, sin(atan(height / radius)), 0.0f));
			data.Vertices[offset + i].Position	= data.Vertices[i + 1].Position;
			data.Vertices[offset + i].Normal	= normal;
			data.Vertices[offset + i].TexCoord	= vec2(0.0f + (uOffset * i), 1.0f);

			//TOP
			data.Vertices[topOffset + i].Position	= vec3(0.0f, height, 0.0f);
			data.Vertices[topOffset + i].Normal		= normal;
			data.Vertices[topOffset + i].TexCoord	= vec2(0.0f + (uOffset * i), 0.25f);
		}

		//BOTTOM CAP INDICES
		size_t index = 0;
		for (uint32 i = 0; i < sides; i++)
		{
			data.Indices[index + 0] = ((i + 1) % sides) + 10;
			data.Indices[index + 1] = i + 1;
			data.Indices[index + 2] = 0;
			index += 3;
		}

		//SIDES INDICES
		for (uint32 i = 0; i < sides; i++)
		{
			data.Indices[index + 0] = uint32(offset) + i;
			data.Indices[index + 1] = uint32(offset) + ((i + 1) % sides);
			data.Indices[index + 2] = uint32(topOffset) + i;
			index += 3;
		}

		//Get tangents
		CalculateTangents(data);

		return data;
	}


	MeshData MeshFactory::CreatePyramid() noexcept
	{
		using namespace glm;

		MeshData data;
		data.Vertices.resize(16);
		data.Indices.resize(18);

		//FLOOR FACE (Seen from FRONT FACE)
		data.Vertices[0].TexCoord = vec2(0.33f, 0.33f);
		data.Vertices[0].Position = vec3(-0.5f, -0.5f, -0.5f);
		data.Vertices[1].TexCoord = vec2(0.66f, 0.33f);
		data.Vertices[1].Position = vec3(0.5f, -0.5f, -0.5f);
		data.Vertices[2].TexCoord = vec2(0.33f, 0.66f);
		data.Vertices[2].Position = vec3(-0.5f, -0.5f, 0.5f);
		data.Vertices[3].TexCoord = vec2(0.66f, 0.66f);
		data.Vertices[3].Position = vec3(0.5f, -0.5f, 0.5f);

		//TOP VERTICES
		data.Vertices[4].Position = 
			data.Vertices[5].Position = 
			data.Vertices[6].Position = 
			data.Vertices[7].Position = vec3(0.0f, 0.5f, 0.0f);
		data.Vertices[4].TexCoord = vec2(0.495f, 0.0f);
		data.Vertices[5].TexCoord = vec2(0.0f, 0.495f);
		data.Vertices[6].TexCoord = vec2(0.495f, 0.99f);
		data.Vertices[7].TexCoord = vec2(0.99f, 0.495f);

		//BACK
		data.Vertices[8].TexCoord = vec2(0.33f, 0.33f);
		data.Vertices[8].Position = vec3(-0.5f, -0.5f, -0.5f);
		data.Vertices[9].TexCoord = vec2(0.66f, 0.33f);
		data.Vertices[9].Position = vec3(0.5f, -0.5f, -0.5f);

		//FRONT
		data.Vertices[10].TexCoord = vec2(0.33f, 0.66f);
		data.Vertices[10].Position = vec3(-0.5f, -0.5f, 0.5f);
		data.Vertices[11].TexCoord = vec2(0.66f, 0.66f);
		data.Vertices[11].Position = vec3(0.5f, -0.5f, 0.5f);

		//LEFT
		data.Vertices[12].TexCoord = vec2(0.33f, 0.33f);
		data.Vertices[12].Position = vec3(-0.5f, -0.5f, -0.5f);
		data.Vertices[13].TexCoord = vec2(0.33f, 0.66f);
		data.Vertices[13].Position = vec3(-0.5f, -0.5f, 0.5f);

		//RIGHT
		data.Vertices[14].TexCoord = vec2(0.66f, 0.33f);
		data.Vertices[14].Position = vec3(0.5f, -0.5f, -0.5f);
		data.Vertices[15].TexCoord = vec2(0.66f, 0.66f);
		data.Vertices[15].Position = vec3(0.5f, -0.5f, 0.5f);

		//FLOOR FACE
		data.Indices[0] = 2;
		data.Indices[1] = 1;
		data.Indices[2] = 0;
		data.Indices[3] = 2;
		data.Indices[4] = 3;
		data.Indices[5] = 1;

		//BACK FACE
		data.Indices[6] = 8;
		data.Indices[7] = 9;
		data.Indices[8] = 4;

		//LEFT FACE
		data.Indices[9] = 13;
		data.Indices[10] = 12;
		data.Indices[11] = 5;

		//FRONT FACE
		data.Indices[12] = 11;
		data.Indices[13] = 10;
		data.Indices[14] = 6;

		//RIGHT FACE
		data.Indices[15] = 14;
		data.Indices[16] = 15;
		data.Indices[17] = 7;

		data.Indices.shrink_to_fit();
		data.Vertices.shrink_to_fit();

		CalculateHardNormals(data);
		CalculateTangents(data);

		return data;
	}


	MeshData MeshFactory::CreateCylinder(uint32 sides, float radius, float height) noexcept
	{
		using namespace glm;
		
		MeshData data;
		if (sides < 5)
			sides = 5;
		if (height < 0.1f)
			height = 0.1f;
		if (radius < 0.1f)
			radius = 0.1f;

		//Num verts = (Sides*2)	(Top, since we need unique normals) 
		//          + (Sides*2)	(Bottom)
		//		    + 2			(MiddlePoints)
		size_t vertSize = size_t(sides) * 4 + 2;
		data.Vertices.resize(vertSize);

		//Num indices = (Sides*3*2) (Each cap has 'sides' number of tris, each tri has 3 verts)
		//			  + (Sides*6)	(Each side has 6 verts)
		size_t indexSize = size_t(sides) * 12;
		data.Indices.resize(indexSize);

		//Angle between verts
		float angle = (pi<float>() * 2.0f) / float(sides);
		float uOffset = 1.0f / float(sides - 1);
		float halfHeight = height * 0.5f;
		
		//CREATE VERTICES
		data.Vertices[0].Position	= vec3(0.0f, halfHeight, 0.0f);
		data.Vertices[0].Normal		= vec3(0.0f, 1.0f, 0.0f);
		data.Vertices[0].TexCoord	= vec2(0.25f, 0.25f);

		size_t offset = size_t(sides) + 1;
		data.Vertices[offset].Position	= vec3(0.0f, -halfHeight, 0.0f);
		data.Vertices[offset].Normal	= vec3(0.0f, -1.0f, 0.0f);
		data.Vertices[offset].TexCoord	= vec2(0.75f, 0.25f);

		size_t doubleOffset = offset * 2;
		size_t trippleOffset = doubleOffset + size_t(sides);
		for (size_t i = 0; i < sides; i++)
		{
			//TOP CAP VERTICES
			float x = cosf((pi<float>() / 2.0f) + (angle * i));
			float z = sinf((pi<float>() / 2.0f) + (angle * i));
			vec3 pos = normalize(vec3(x, 0.0f, z));
			data.Vertices[i + 1].Position	= (pos * radius) + vec3(0.0f, halfHeight, 0.0f);
			data.Vertices[i + 1].Normal		= vec3(0.0f, 1.0f, 0.0f);
			data.Vertices[i + 1].TexCoord	= vec2(x + 1.0f, z + 1.0f) * 0.25f;

			//BOTTOM CAP VERTICES
			data.Vertices[offset + i + 1].Position	= data.Vertices[i + 1].Position - vec3(0.0f, height, 0.0f);
			data.Vertices[offset + i + 1].Normal	= vec3(0.0f, -1.0f, 0.0f);
			data.Vertices[offset + i + 1].TexCoord	= data.Vertices[i + 1].TexCoord + vec2(0.5f, 0.5f);

			//TOP SIDE VERTICES
			data.Vertices[doubleOffset + i].Position	= data.Vertices[i + 1].Position;
			data.Vertices[doubleOffset + i].Normal		= pos;
			data.Vertices[doubleOffset + i].TexCoord	= vec2(0.0f + (uOffset * i), 1.0f);

			//BOTTOM SIDE VERTICES
			data.Vertices[trippleOffset + i].Position	= data.Vertices[offset + i + 1].Position;
			data.Vertices[trippleOffset + i].Normal		= pos;
			data.Vertices[trippleOffset + i].TexCoord	= vec2(0.0f + (uOffset * i), 0.25f);
		}

		//TOP CAP INDICES
		size_t index = 0;
		for (uint32 i = 0; i < sides; i++)
		{
			data.Indices[index + 0] = i + 1;
			data.Indices[index + 1] = (i + 1) % (sides) + 1;
			data.Indices[index + 2] = 0;
			index += 3;
		}

		//BOTTOM CAP INDICES
		for (uint32 i = 0; i < sides; i++)
		{
			uint32 base = uint32(sides) + 1;
			data.Indices[index + 0] = base + ((i + 1) % (sides)+1);
			data.Indices[index + 1] = base + i + 1;
			data.Indices[index + 2] = base;
			index += 3;
		}

		//SIDES
		for (uint32 i = 0; i < sides; i++)
		{
			uint32 base = (uint32(sides) + 1) * 2;
			data.Indices[index + 0] = base + i + 1;
			data.Indices[index + 1] = base + i;
			data.Indices[index + 2] = base + i + sides;
			data.Indices[index + 3] = base + ((i + 1) % sides);
			data.Indices[index + 4] = (base + sides - 1) + ((i + 1) % sides);
			data.Indices[index + 5] = (base + sides) + ((i + 1) % sides);
			index += 6;
		}

		CalculateTangents(data);
		return data;
	}


	void MeshFactory::Subdivide(MeshData& data, uint32 subdivisions) noexcept
	{
		using namespace glm;
		
		if (subdivisions < 1)
		{
			return;
		}

		Vertex v[3];
		size_t j = 0;
		uint32 indexCount		= 0;
		uint32 vertexCount		= 0;
		uint32 oldVertexCount	= 0;
		data.Vertices.reserve((data.Vertices.size() * size_t(pow(2, subdivisions))));
		data.Indices.reserve((data.Indices.size() * size_t(pow(4, subdivisions))));

		for (uint32 i = 0; i < subdivisions; i++)
		{
			oldVertexCount	= uint32(data.Vertices.size());
			indexCount		= uint32(data.Indices.size());

			for (j = 0; j < indexCount; j += 3)
			{
				v[0].Position = (data.Vertices[data.Indices[j]].Position + data.Vertices[data.Indices[j+1]].Position) * 0.5f;
				v[1].Position = (data.Vertices[data.Indices[j]].Position + data.Vertices[data.Indices[j+2]].Position) * 0.5f;
				v[2].Position = (data.Vertices[data.Indices[j+1]].Position + data.Vertices[data.Indices[j+2]].Position) * 0.5f;

				v[0].TexCoord = (data.Vertices[data.Indices[j]].TexCoord + data.Vertices[data.Indices[j+1]].TexCoord) * 0.5f;
				v[1].TexCoord = (data.Vertices[data.Indices[j]].TexCoord + data.Vertices[data.Indices[j+2]].TexCoord) * 0.5f;
				v[2].TexCoord = (data.Vertices[data.Indices[j+1]].TexCoord + data.Vertices[data.Indices[j+2]].TexCoord) * 0.5f;

				v[0].Normal = normalize((data.Vertices[data.Indices[j]].Normal + data.Vertices[data.Indices[j+1]].Normal) * 0.5f);
				v[1].Normal = normalize((data.Vertices[data.Indices[j]].Normal + data.Vertices[data.Indices[j+2]].Normal) * 0.5f);
				v[2].Normal = normalize((data.Vertices[data.Indices[j+1]].Normal + data.Vertices[data.Indices[j+2]].Normal) * 0.5f);

				v[0].Tangent = normalize((data.Vertices[data.Indices[j]].Tangent + data.Vertices[data.Indices[j+1]].Tangent) * 0.5f);
				v[1].Tangent = normalize((data.Vertices[data.Indices[j]].Tangent + data.Vertices[data.Indices[j+2]].Tangent) * 0.5f);
				v[2].Tangent = normalize((data.Vertices[data.Indices[j+1]].Tangent + data.Vertices[data.Indices[j+2]].Tangent) * 0.5f);

				//Push the new Vertices
				data.Vertices.emplace_back(v[0]);
				data.Vertices.emplace_back(v[1]);
				data.Vertices.emplace_back(v[2]);

				//Push index of the new triangles
				vertexCount = uint32(data.Vertices.size());
				data.Indices.emplace_back(vertexCount - 3);
				data.Indices.emplace_back(vertexCount - 1);
				data.Indices.emplace_back(vertexCount - 2);

				data.Indices.emplace_back(vertexCount - 3);
				data.Indices.emplace_back(data.Indices[j + 1]);
				data.Indices.emplace_back(vertexCount - 1);

				data.Indices.emplace_back(vertexCount - 2);
				data.Indices.emplace_back(vertexCount - 1);
				data.Indices.emplace_back(data.Indices[j + 2]);


				//Reassign the old indexes
				data.Indices[j + 1] = vertexCount - 3;
				data.Indices[j + 2] = vertexCount - 2;
			}

			Optimize(data, oldVertexCount);
		}

		data.Vertices.shrink_to_fit();
		data.Indices.shrink_to_fit();
	}


	void MeshFactory::Optimize(MeshData& data, uint32 startVertex) noexcept
	{
		using namespace std;

		uint32 vertexSize	= uint32(data.Vertices.size());
		uint32 indexSize	= uint32(data.Indices.size());
		LAMBDA_ASSERT(startVertex < (uint32)data.Vertices.size());
		uint32 k = 0;
		uint32 j = 0;

		for (uint32 i = startVertex; i < vertexSize; i++)
		{
			for (j = 0; j < vertexSize; j++)
			{
				if (data.Vertices[i] == data.Vertices[j])
				{
					if (i != j)
					{
						data.Vertices.erase(data.Vertices.begin() + i);
						vertexSize--;
						j--;

						for (k = 0; k < indexSize; k++)
						{
							if (data.Indices[k] == i)
								data.Indices[k] = j;
							else if (data.Indices[k] > i)
								data.Indices[k]--;
						}

						i--;
						break;
					}
				}
			}
		}
	}


	void MeshFactory::CalculateHardNormals(MeshData& data) noexcept
	{
		using namespace glm;
		vec3 e1;
		vec3 e2;
		vec3 n;

		for (size_t i = 0; i < data.Indices.size(); i += 3)
		{
			e1 = data.Vertices[data.Indices[i+2]].Position - data.Vertices[data.Indices[i]].Position;
			e2 = data.Vertices[data.Indices[i+1]].Position - data.Vertices[data.Indices[i]].Position;
			n = cross(e1, e2);

			data.Vertices[data.Indices[i]].Normal = n;
			data.Vertices[data.Indices[i+1]].Normal = n;
			data.Vertices[data.Indices[i+2]].Normal = n;
		}
	}


	void MeshFactory::CalculateTangents(MeshData& data) noexcept
	{
		using namespace glm;

		for (uint32 i = 0; i < data.Indices.size(); i += 3)
		{
			vec3 modelVec		= data.Vertices[data.Indices[i + 1]].Position - data.Vertices[data.Indices[i]].Position;
			vec3 modelVec2		= data.Vertices[data.Indices[i + 2]].Position - data.Vertices[data.Indices[i]].Position;
			vec2 tangentVec		= data.Vertices[data.Indices[i + 1]].TexCoord - data.Vertices[data.Indices[i]].TexCoord;
			vec2 tangentVec2	= data.Vertices[data.Indices[i + 2]].TexCoord - data.Vertices[data.Indices[i]].TexCoord;

			float denominator	= 1.0f / ((tangentVec.x * tangentVec2.y) - (tangentVec2.x * tangentVec.y));
			vec3 tangent		= normalize(((modelVec * tangentVec2.y) - (modelVec2 * tangentVec.y)) * denominator);

			data.Vertices[data.Indices[i+0]].Tangent = tangent;
			data.Vertices[data.Indices[i+1]].Tangent = tangent;
			data.Vertices[data.Indices[i+2]].Tangent = tangent;
		}
	}


	/*void Mesh::calcNormal()
	{
		using namespace Math;

		for (uint32 i = 0; i < indexBuffer->size(); i += 3)
		{
			vec3 edge1 = (*vertexBuffer)[(*indexBuffer)[i + 2]].Position - (*vertexBuffer)[(*indexBuffer)[i + 1]].Position;
			vec3 edge2 = (*vertexBuffer)[(*indexBuffer)[i]].Position - (*vertexBuffer)[(*indexBuffer)[i + 2]].Position;
			vec3 edge3 = (*vertexBuffer)[(*indexBuffer)[i + 1]].Position - (*vertexBuffer)[(*indexBuffer)[i + 0]].Position;

			vec3 Normal = edge1.Cross(edge2);
			Normal.Normalize();

			(*vertexBuffer)[(*indexBuffer)[i]].Normal = Normal;
			(*vertexBuffer)[(*indexBuffer)[i + 1]].Normal = Normal;
			(*vertexBuffer)[(*indexBuffer)[i + 2]].Normal = Normal;
		}
	}

	void Mesh::calcTangent()
	{
		using namespace Math;

		for (uint32 i = 0; i < indexBuffer->size(); i += 3)
		{
			vec3 modelVec = (*vertexBuffer)[(*indexBuffer)[i + 1]].Position - (*vertexBuffer)[(*indexBuffer)[i]].Position;
			vec3 modelVec2 = (*vertexBuffer)[(*indexBuffer)[i + 2]].Position - (*vertexBuffer)[(*indexBuffer)[i]].Position;
			vec2 tangentVec = (*vertexBuffer)[(*indexBuffer)[i + 1]].TexCoord - (*vertexBuffer)[(*indexBuffer)[i]].TexCoord;
			vec2 tangentVec2 = (*vertexBuffer)[(*indexBuffer)[i + 2]].TexCoord - (*vertexBuffer)[(*indexBuffer)[i]].TexCoord;

			float denominator = 1.0f / ((tangentVec.x * tangentVec2.y) - (tangentVec2.x * tangentVec.y));
			vec3 tangent = ((modelVec * tangentVec2.y) - (modelVec2 * tangentVec.y)) * denominator;
			tangent.Normalize();

			(*vertexBuffer)[(*indexBuffer)[i]].tangent = tangent;
			(*vertexBuffer)[(*indexBuffer)[i + 1]].tangent = tangent;
			(*vertexBuffer)[(*indexBuffer)[i + 2]].tangent = tangent;
		}
	}*/

}
