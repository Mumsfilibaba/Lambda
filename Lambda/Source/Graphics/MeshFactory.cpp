#include "LambdaPch.h"
#include "Graphics/MeshFactory.h"
#define GLM_FORCE_RADIANS
#include "glm/gtc/constants.hpp"

namespace Lambda
{

	MeshData MeshFactory::CreateFromFile(const std::string& filename, bool rightHanded) noexcept
	{
		using namespace std;

		MeshData data;
		//FILE* file = fopen(filename.c_str(), "rb");
		//if (file == nullptr)
		//{
		//	Re_LogDebugError(ERROR_LEVEL_ERROR, "'MeshFactory::CreateFromFile': Could not open file '" + filename + "'");
		//	return data;
		//}

		//fclose(file);

		return data;
	}


	MeshData MeshFactory::CreateCube() noexcept
	{
		using namespace glm;

		MeshData data;
		data.Vertices =
		{
			//FRONT FACE
			{ vec3(-0.5f,  0.5f, -0.5f), vec3(0.0f,  0.0f, -1.0f), vec2(0.0f, 0.0f) },
			{ vec3(0.5f,  0.5f, -0.5f), vec3(0.0f,  0.0f, -1.0f), vec2(1.0f, 0.0f) },
			{ vec3(-0.5f, -0.5f, -0.5f), vec3(0.0f,  0.0f, -1.0f), vec2(0.0f, 1.0f) },
			{ vec3(0.5f, -0.5f, -0.5f), vec3(0.0f,  0.0f, -1.0f), vec2(1.0f, 1.0f) },

			//BACK FACE
			{ vec3(0.5f,  0.5f,  0.5f), vec3(0.0f,  0.0f,  1.0f), vec2(0.0f, 0.0f) },
			{ vec3(-0.5f,  0.5f,  0.5f), vec3(0.0f,  0.0f,  1.0f), vec2(1.0f, 0.0f) },
			{ vec3(0.5f, -0.5f,  0.5f), vec3(0.0f,  0.0f,  1.0f), vec2(0.0f, 1.0f) },
			{ vec3(-0.5f, -0.5f,  0.5f), vec3(0.0f,  0.0f,  1.0f), vec2(1.0f, 1.0f) },

			//RIGHT FACE
			{ vec3(0.5f,  0.5f, -0.5f), vec3(1.0f,  0.0f,  0.0f), vec2(0.0f, 0.0f) },
			{ vec3(0.5f,  0.5f,  0.5f), vec3(1.0f,  0.0f,  0.0f), vec2(1.0f, 0.0f) },
			{ vec3(0.5f, -0.5f, -0.5f), vec3(1.0f,  0.0f,  0.0f), vec2(0.0f, 1.0f) },
			{ vec3(0.5f, -0.5f,  0.5f), vec3(1.0f,  0.0f,  0.0f), vec2(1.0f, 1.0f) },

			//LEFT FACE
			{ vec3(-0.5f,  0.5f, -0.5f), vec3(-1.0f,  0.0f,  0.0f), vec2(0.0f, 0.0f) },
			{ vec3(-0.5f,  0.5f,  0.5f), vec3(-1.0f,  0.0f,  0.0f), vec2(1.0f, 0.0f) },
			{ vec3(-0.5f, -0.5f, -0.5f), vec3(-1.0f,  0.0f,  0.0f), vec2(0.0f, 1.0f) },
			{ vec3(-0.5f, -0.5f,  0.5f), vec3(-1.0f,  0.0f,  0.0f), vec2(1.0f, 1.0f) },

			//ROOF FACE
			{ vec3(-0.5f,  0.5f,  0.5f), vec3(0.0f,  1.0f,  0.0f), vec2(0.0f, 0.0f) },
			{ vec3(0.5f,  0.5f,  0.5f), vec3(0.0f,  1.0f,  0.0f), vec2(1.0f, 0.0f) },
			{ vec3(-0.5f,  0.5f, -0.5f), vec3(0.0f,  1.0f,  0.0f), vec2(0.0f, 1.0f) },
			{ vec3(0.5f,  0.5f, -0.5f), vec3(0.0f,  1.0f,  0.0f), vec2(1.0f, 1.0f) },

			//FLOOR FACE
			{ vec3(-0.5f, -0.5f, -0.5f), vec3(0.0f, -1.0f,  0.0f), vec2(0.0f, 0.0f) },
			{ vec3(0.5f, -0.5f, -0.5f), vec3(0.0f, -1.0f,  0.0f), vec2(1.0f, 0.0f) },
			{ vec3(-0.5f, -0.5f,  0.5f), vec3(0.0f, -1.0f,  0.0f), vec2(0.0f, 1.0f) },
			{ vec3(0.5f, -0.5f,  0.5f), vec3(0.0f, -1.0f,  0.0f), vec2(1.0f, 1.0f) },
		};

		data.Indices =
		{
			//FRONT FACE
			0, 1, 2,
			1, 3, 2,

			//BACK FACE
			4, 5, 6,
			5, 7, 6,

			//RIGHT FACE
			8, 9, 10,
			9, 11, 10,

			//LEFT FACE
			14, 13, 12,
			14, 15, 13,

			//ROOF FACE
			16, 17, 18,
			17, 19, 18,

			//FLOOR FACE
			20, 21, 22,
			21, 23, 22
		};

		return data;
	}



	MeshData MeshFactory::CreatePlane(uint8 width, uint8 height) noexcept
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
				int v = ((1 + height) * x) + y;
				data.Vertices[v].Position = vec3(0.5f - (quadSize.x * x), 0.5f - (quadSize.y * y), 0.0f);
				data.Vertices[v].Normal = vec3(0.0f, 0.0f, 1.0f);
				data.Vertices[v].TexCoord = vec2(0.0f + (uvQuadSize.x * x), 0.0f + (uvQuadSize.y * y));
			}
		}

		//Initialize Indices
		for (unsigned char x = 0; x < width; x++)
		{
			for (unsigned char y = 0; y < height; y++)
			{
				int quad = (height * x) + y;
				data.Indices[quad * 6] = (x * (1 + height)) + y + 1;
				data.Indices[(quad * 6) + 1] = (data.Indices[quad * 6] + 2 + (height - 1));
				data.Indices[(quad * 6) + 2] =  data.Indices[quad * 6] - 1;
				data.Indices[(quad * 6) + 3] =  data.Indices[(quad * 6) + 1];
				data.Indices[(quad * 6) + 4] =  data.Indices[(quad * 6) + 1] - 1;
				data.Indices[(quad * 6) + 5] =  data.Indices[(quad * 6) + 2];
			}
		}

		data.Vertices.shrink_to_fit();
		data.Indices.shrink_to_fit();
		return data;
	}



	MeshData MeshFactory::CreateSphere(uint8 subdivisions) noexcept
	{
		using namespace glm;

		MeshData data;
		data.Vertices.resize(12);

		//VERTICES
		data.Vertices[0].Position = vec3(-0.525731f, 0.0f, 0.850651f);
		data.Vertices[1].Position = vec3(0.525731f, 0.0f, 0.850651f);
		data.Vertices[2].Position = vec3(-0.525731f, 0.0f, -0.850651f);
		data.Vertices[3].Position = vec3(0.525731f, 0.0f, -0.850651f);
		data.Vertices[4].Position = vec3(0.0f, 0.850651f, 0.525731f);
		data.Vertices[5].Position = vec3(0.0f, 0.850651f, -0.525731f);
		data.Vertices[6].Position = vec3(0.0f, -0.850651f, 0.525731f);
		data.Vertices[7].Position = vec3(0.0f, -0.850651f, -0.525731f);
		data.Vertices[8].Position = vec3(0.850651f, 0.525731f, 0.0f);
		data.Vertices[9].Position = vec3(-0.850651f, 0.525731f, 0.0f);
		data.Vertices[10].Position = vec3(0.850651f, -0.525731f, 0.0f);
		data.Vertices[11].Position = vec3(-0.850651f, -0.525731f, 0.0f);

		//INDICIES
		data.Indices =
		{
			1, 4, 0,
			4, 9, 0,
			4, 5, 9,
			8, 5, 4,
			1, 8, 4,
			1, 10, 8,
			10, 3, 8,
			8, 3, 5,
			3, 2, 5,
			3, 7, 2,
			3, 10, 7,
			10, 6, 7,
			6, 11, 7,
			6, 0, 11,
			6, 1, 0,
			10, 1, 6,
			11, 0, 9,
			2, 11, 9,
			5, 2, 9,
			11, 2, 7
		};


		if (subdivisions > 0)
		{
			Subdivide(data, subdivisions);
		}

		for (uint32 i = 0; i < data.Vertices.size(); i++)
		{
			data.Vertices[i].Position = normalize(data.Vertices[i].Position);
			data.Vertices[i].Normal = data.Vertices[i].Position;

			//Calculate uvs
			vec2 uv;
			uv.x = (atan2f(data.Vertices[i].Position.z, data.Vertices[i].Position.x) / (2.0f * pi<float>()));
			uv.y = (asin(data.Vertices[i].Position.y) / pi<float>()) + 0.5f;
			data.Vertices[i].TexCoord = uv;

			//Scale down the sphere
			data.Vertices[i].Position *= 0.5f;
		}

		data.Indices.shrink_to_fit();
		data.Vertices.shrink_to_fit();

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
		data.Indices[0] = 0;
		data.Indices[1] = 1;
		data.Indices[2] = 2;
		data.Indices[3] = 1;
		data.Indices[4] = 3;
		data.Indices[5] = 2;

		//BACK FACE
		data.Indices[6] = 4;
		data.Indices[7] = 9;
		data.Indices[8] = 8;

		//LEFT FACE
		data.Indices[9] = 5;
		data.Indices[10] = 12;
		data.Indices[11] = 13;

		//FRONT FACE
		data.Indices[12] = 6;
		data.Indices[13] = 10;
		data.Indices[14] = 11;

		//RIGHT FACE
		data.Indices[15] = 7;
		data.Indices[16] = 15;
		data.Indices[17] = 14;

		data.Indices.shrink_to_fit();
		data.Vertices.shrink_to_fit();

		CalculateNormalsHardened(data);
		return data;
	}


	MeshData MeshFactory::CreateCylinder(uint8 sides) noexcept
	{
		using namespace glm;
		MeshData data;
		if (sides < 5)
		{
			sides = 5;
		}

		data.Vertices.resize(22);
		data.Indices.resize(60);

		constexpr float angle = (pi<float>() * 2.0f) / 5.0f;
		//TOP CAP VERTICES
		data.Vertices[0].Position = vec3(0.0f, 0.5f, 0.0f);
		data.Vertices[1].Position = normalize(vec3(cosf( pi<float>() / 2.0f),				 0.0f, sinf( pi<float>() / 2.0f)))				  * 0.5f + vec3(0.0f, 0.5f, 0.0f);
		data.Vertices[2].Position = normalize(vec3(cosf((pi<float>() / 2.0f) +	angle),		 0.0f, sinf((pi<float>() / 2.0f) + angle)))		  * 0.5f + vec3(0.0f, 0.5f, 0.0f);
		data.Vertices[3].Position = normalize(vec3(cosf((pi<float>() / 2.0f) + (angle * 2)), 0.0f, sinf((pi<float>() / 2.0f) + (angle * 2)))) * 0.5f + vec3(0.0f, 0.5f, 0.0f);
		data.Vertices[4].Position = normalize(vec3(cosf((pi<float>() / 2.0f) + (angle * 3)), 0.0f, sinf((pi<float>() / 2.0f) + (angle * 3)))) * 0.5f + vec3(0.0f, 0.5f, 0.0f);
		data.Vertices[5].Position = normalize(vec3(cosf((pi<float>() / 2.0f) + (angle * 4)), 0.0f, sinf((pi<float>() / 2.0f) + (angle * 4)))) * 0.5f + vec3(0.0f, 0.5f, 0.0f);
		data.Vertices[0].Normal = vec3(0.0f, -1.0f, 0.0f);
		data.Vertices[1].Normal = vec3(0.0f, -1.0f, 0.0f);
		data.Vertices[2].Normal = vec3(0.0f, -1.0f, 0.0f);
		data.Vertices[3].Normal = vec3(0.0f, -1.0f, 0.0f);
		data.Vertices[4].Normal = vec3(0.0f, -1.0f, 0.0f);
		data.Vertices[5].Normal = vec3(0.0f, -1.0f, 0.0f);

		//BOTTOM CAP VERTICES
		data.Vertices[6].Position = data.Vertices[0].Position - vec3(0.0f, 1.0f, 0.0f);
		data.Vertices[7].Position = data.Vertices[1].Position - vec3(0.0f, 1.0f, 0.0f);
		data.Vertices[8].Position = data.Vertices[2].Position - vec3(0.0f, 1.0f, 0.0f);
		data.Vertices[9].Position = data.Vertices[3].Position - vec3(0.0f, 1.0f, 0.0f);
		data.Vertices[10].Position = data.Vertices[4].Position - vec3(0.0f, 1.0f, 0.0f);
		data.Vertices[11].Position = data.Vertices[5].Position - vec3(0.0f, 1.0f, 0.0f);
		data.Vertices[6].Normal = vec3(0.0f, 1.0f, 0.0f);
		data.Vertices[7].Normal = vec3(0.0f, 1.0f, 0.0f);
		data.Vertices[8].Normal = vec3(0.0f, 1.0f, 0.0f);
		data.Vertices[9].Normal = vec3(0.0f, 1.0f, 0.0f);
		data.Vertices[10].Normal = vec3(0.0f, 1.0f, 0.0f);
		data.Vertices[11].Normal = vec3(0.0f, 1.0f, 0.0f);

		//TOP SIDE VERTICES
		data.Vertices[12].Position = data.Vertices[1].Position;
		data.Vertices[12].Normal = normalize(data.Vertices[12].Position);
		data.Vertices[13].Position = data.Vertices[2].Position;
		data.Vertices[13].Normal = normalize(data.Vertices[13].Position);
		data.Vertices[14].Position = data.Vertices[3].Position;
		data.Vertices[14].Normal = normalize(data.Vertices[14].Position);
		data.Vertices[15].Position = data.Vertices[4].Position;
		data.Vertices[15].Normal = normalize(data.Vertices[15].Position);
		data.Vertices[16].Position = data.Vertices[5].Position;
		data.Vertices[16].Normal = normalize(data.Vertices[16].Position);

		//BOTTOM SIDE VERTICES
		data.Vertices[17].Position = data.Vertices[7].Position;
		data.Vertices[17].Normal = normalize(data.Vertices[17].Position);
		data.Vertices[18].Position = data.Vertices[8].Position;
		data.Vertices[18].Normal = normalize(data.Vertices[18].Position);
		data.Vertices[19].Position = data.Vertices[9].Position;
		data.Vertices[19].Normal = normalize(data.Vertices[19].Position);
		data.Vertices[20].Position = data.Vertices[10].Position;
		data.Vertices[20].Normal = normalize(data.Vertices[20].Position);
		data.Vertices[21].Position = data.Vertices[11].Position;
		data.Vertices[21].Normal = normalize(data.Vertices[21].Position);

		//TOP CAP Indices
		data.Indices[0] = 0;
		data.Indices[1] = 2;
		data.Indices[2] = 1;

		data.Indices[3] = 0;
		data.Indices[4] = 3;
		data.Indices[5] = 2;

		data.Indices[6] = 0;
		data.Indices[7] = 4;
		data.Indices[8] = 3;

		data.Indices[9] = 0;
		data.Indices[10] = 5;
		data.Indices[11] = 4;

		data.Indices[12] = 0;
		data.Indices[13] = 1;
		data.Indices[14] = 5;

		//BOTTOM CAP Indices
		data.Indices[15] = 6;
		data.Indices[16] = 7;
		data.Indices[17] = 8;

		data.Indices[18] = 6;
		data.Indices[19] = 8;
		data.Indices[20] = 9;

		data.Indices[21] = 6;
		data.Indices[22] = 9;
		data.Indices[23] = 10;

		data.Indices[24] = 6;
		data.Indices[25] = 10;
		data.Indices[26] = 11;

		data.Indices[27] = 6;
		data.Indices[28] = 11;
		data.Indices[29] = 7;

		//SIDES
		data.Indices[30] = 17;
		data.Indices[31] = 12;
		data.Indices[32] = 13;
		data.Indices[33] = 18;
		data.Indices[34] = 17;
		data.Indices[35] = 13;

		data.Indices[36] = 18;
		data.Indices[37] = 13;
		data.Indices[38] = 14;
		data.Indices[39] = 14;
		data.Indices[40] = 19;
		data.Indices[41] = 18;

		data.Indices[42] = 19;
		data.Indices[43] = 14;
		data.Indices[44] = 15;
		data.Indices[45] = 15;
		data.Indices[46] = 20;
		data.Indices[47] = 19;

		data.Indices[48] = 20;
		data.Indices[49] = 15;
		data.Indices[50] = 16;
		data.Indices[51] = 16;
		data.Indices[52] = 21;
		data.Indices[53] = 20;

		data.Indices[54] = 21;
		data.Indices[55] = 16;
		data.Indices[56] = 17;
		data.Indices[57] = 17;
		data.Indices[58] = 16;
		data.Indices[59] = 12;

		return data;
	}


	void MeshFactory::Subdivide(MeshData& data, uint8 subdivisions) noexcept
	{
		using namespace glm;
		if (subdivisions < 1)
		{
			return;
		}

		Vertex v[3];
		int j = 0;
		int indexCount = 0;
		int vertexCount = 0;
		int oldVertexCount = 0;
		data.Vertices.reserve((data.Vertices.size() * (uint32)pow(2, subdivisions)));
		data.Indices.reserve((data.Indices.size() * (uint32)pow(4, subdivisions)));

		for (int i = 0; i < subdivisions; i++)
		{
			oldVertexCount = (int)data.Vertices.size();
			indexCount = (int)data.Indices.size();

			for (j = 0; j < indexCount; j += 3)
			{
				v[0].Position = (data.Vertices[data.Indices[j]].Position + data.Vertices[data.Indices[j + 1]].Position) * 0.5f;
				v[1].Position = (data.Vertices[data.Indices[j]].Position + data.Vertices[data.Indices[j + 2]].Position) * 0.5f;
				v[2].Position = (data.Vertices[data.Indices[j + 1]].Position + data.Vertices[data.Indices[j + 2]].Position) * 0.5f;

				v[0].TexCoord = (data.Vertices[data.Indices[j]].TexCoord + data.Vertices[data.Indices[j + 1]].TexCoord) * 0.5f;
				v[1].TexCoord = (data.Vertices[data.Indices[j]].TexCoord + data.Vertices[data.Indices[j + 2]].TexCoord) * 0.5f;
				v[2].TexCoord = (data.Vertices[data.Indices[j + 1]].TexCoord + data.Vertices[data.Indices[j + 2]].TexCoord) * 0.5f;

				v[0].Normal = normalize((data.Vertices[data.Indices[j]].Normal + data.Vertices[data.Indices[j + 1]].Normal) * 0.5f);
				v[1].Normal = normalize((data.Vertices[data.Indices[j]].Normal + data.Vertices[data.Indices[j + 2]].Normal) * 0.5f);
				v[2].Normal = normalize((data.Vertices[data.Indices[j + 1]].Normal + data.Vertices[data.Indices[j + 2]].Normal) * 0.5f);

				//Push the new Vertices
				data.Vertices.emplace_back(v[0]);
				data.Vertices.emplace_back(v[1]);
				data.Vertices.emplace_back(v[2]);

				//Push index of the new triangles
				vertexCount = (int)data.Vertices.size();
				data.Indices.emplace_back((vertexCount - 3));
				data.Indices.emplace_back((vertexCount - 1));
				data.Indices.emplace_back((vertexCount - 2));

				data.Indices.emplace_back((vertexCount - 3));
				data.Indices.emplace_back(data.Indices[j + 1]);
				data.Indices.emplace_back((vertexCount - 1));

				data.Indices.emplace_back((vertexCount - 2));
				data.Indices.emplace_back((vertexCount - 1));
				data.Indices.emplace_back(data.Indices[j + 2]);


				//Reassign the old indexes
				data.Indices[j + 1] = ((vertexCount - 3));
				data.Indices[j + 2] = ((vertexCount - 2));
			}

			Optimize(data, oldVertexCount);
		}

		data.Vertices.shrink_to_fit();
		data.Indices.shrink_to_fit();
	}


	void MeshFactory::Optimize(MeshData& data, uint32 startVertex) noexcept
	{
		using namespace std;

		uint32 vertexSize = (uint32)data.Vertices.size();
		uint32 indexSize = (uint32)data.Indices.size();
		assert(startVertex < (uint32)data.Vertices.size());
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


	void MeshFactory::CalculateNormalsHardened(MeshData& data) noexcept
	{
		using namespace glm;
		vec3 e1;
		vec3 e2;
		vec3 n;

		for (int i = 0; i < data.Indices.size(); i += 3)
		{
			e1 = data.Vertices[data.Indices[i + 1]].Position - data.Vertices[data.Indices[i]].Position;
			e2 = data.Vertices[data.Indices[i + 2]].Position - data.Vertices[data.Indices[i]].Position;
			n = cross(e1, e2);

			data.Vertices[data.Indices[i]].Normal = n;
			data.Vertices[data.Indices[i + 1]].Normal = n;
			data.Vertices[data.Indices[i + 2]].Normal = n;
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

	//void Mesh::calcTangent(MeshData& data)
	//{
	//	using namespace Math;

	//	for (uint32 i = 0; i < data.Indices.size(); i += 3)
	//	{
	//		vec3 modelVec = data.Vertices[data.Indices[i + 1]].Position - data.Vertices[data.Indices[i]].Position;
	//		vec3 modelVec2 = data.Vertices[data.Indices[i + 2]].Position - data.Vertices[data.Indices[i]].Position;
	//		vec2 tangentVec = data.Vertices[data.Indices[i + 1]].TexCoord - data.Vertices[data.Indices[i]].TexCoord;
	//		vec2 tangentVec2 = data.Vertices[data.Indices[i + 2]].TexCoord - data.Vertices[data.Indices[i]].TexCoord;

	//		float denominator = 1.0f / ((tangentVec.x * tangentVec2.y) - (tangentVec2.x * tangentVec.y));
	//		vec3 tangent = ((modelVec * tangentVec2.y) - (modelVec2 * tangentVec.y)) * denominator;
	//		tangent.Normalize();

	//		data.Vertices[data.Indices[i]].tangent = tangent;
	//		data.Vertices[data.Indices[i + 1]].tangent = tangent;
	//		data.Vertices[data.Indices[i + 2]].tangent = tangent;
	//	}
	//}
}
