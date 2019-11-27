#include "SandBox.h"
#include <LambdaEntryPoint.h>
#include "Core/LogManager.h"
#include "Graphics/MeshFactory.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/euler_angles.hpp>

#define SINGLE_CUBE
#define RGB_F(r, g, b) float(r) / 255.0f, float(g) / 255.0f, float(b) / 255.0f

namespace Lambda
{
	//------------------------
	//Define CreateGameLayer()
	//------------------------
	Layer* CreateGameLayer()
	{
		return new SandBoxLayer();
	}

	//------------
	//SandBoxLayer
	//------------

	SandBoxLayer::SandBoxLayer()
         : Layer("SandBoxLayer")
	{
	}


	void SandBoxLayer::OnLoad()
	{
	}


	void SandBoxLayer::OnUpdate(Timestep dt)
	{
	}


	void SandBoxLayer::OnRender(Renderer3D& renderer, Timestep dt)
	{
	}


	void SandBoxLayer::OnRelease()
	{
	}


    void SandBoxLayer::OnRenderUI(Timestep dt)
    {
    }
}
