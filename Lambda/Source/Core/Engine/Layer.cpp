#include "LambdaPch.h"

#include "Core/Engine/Layer.h"

namespace Lambda
{
	//------
	//CLayer
	//------

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	CLayer::CLayer(const char* pName)
		: m_Name()
	{
		if (pName)
		{
			m_Name = std::string(pName);
		}
	}
}
