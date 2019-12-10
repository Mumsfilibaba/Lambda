#include "SandBox.h"
#include "LambdaEntryPoint.h"
#include "Core/CLogManager.h"
#include "Graphics/CMeshFactory.h"
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
	CLayer* CreateGameLayer()
	{
		return new SandBoxLayer();
	}

	//------------
	//SandBoxLayer
	//------------

	SandBoxLayer::SandBoxLayer()
         : CLayer("SandBoxLayer")
	{
	}


	void SandBoxLayer::OnLoad()
	{
	}


	void SandBoxLayer::OnUpdate(const CTime& dt)
	{
	}


	void SandBoxLayer::OnRender(Renderer3D& renderer, const CTime& dt)
	{
	}


	void SandBoxLayer::OnRelease()
	{
	}


    void SandBoxLayer::OnRenderUI(const CTime& dt)
    {
    }
}
