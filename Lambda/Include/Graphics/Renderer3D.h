#pragma once
#include "ICommandList.h"
#include "IRenderPass.h"
#include <vector>

namespace Lambda
{
	class LAMBDA_API Renderer3D
	{
		friend class Application;

	public:
		LAMBDA_NO_COPY(Renderer3D);

		Renderer3D();
		~Renderer3D() = default;

		void Init();
		void Begin();
		void Submit();
		void End();
		void Swapbuffers();
		void Release();
		IRenderPass* GetRenderPass() const;
	private:
		ICommandList*				m_pCurrentList;
		std::vector<ICommandList*>	m_Lists;
		IRenderPass*				m_pRenderPass;
	};
}