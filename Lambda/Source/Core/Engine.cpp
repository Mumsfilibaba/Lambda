#include "LambdaPch.h"

#include "Core/Engine.h"

#include "Platform/Platform.h"

namespace Lambda
{
    LAMBDA_API CEngine g_Engine;

    CEngine::CEngine()
        : m_pWindow(nullptr)
    {
    }

    void CEngine::Init()
    {
        SWindowDesc desc = {};
        desc.pTitle = "Lambda Engine";
        desc.Width = 1440;
        desc.Height = 900;

        m_pWindow = Platform::CreateWindow(desc);
    }

    void CEngine::Release()
    {
        //TODO: Release Window
    }
}

