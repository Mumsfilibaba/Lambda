#include "SandBox.h"

namespace Lambda
{
	SandBox::SandBox()
		: m_pList(nullptr)
	{
	}
	
	SandBox::~SandBox()
	{
	}

	void SandBox::OnLoad()
	{
		//Create commandlist
		{
			m_pList = ICommandList::Create(COMMAND_LIST_TYPE_GRAPHICS);
		}
	}

	void SandBox::OnUpdate(Time dt)
	{
	}

	void SandBox::OnRender(Time dt)
	{
		m_pList->Reset();
		m_pList->Close();
	}

	void SandBox::OnRelease()
	{
		SafeRelease(m_pList);
	}
}