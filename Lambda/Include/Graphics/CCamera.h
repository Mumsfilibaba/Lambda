#pragma once
#include "LambdaCore.h"
#include <glm/glm.hpp>

namespace Lambda
{
    //-------
    //CCamera
    //-------
    
    class LAMBDA_API CCamera
    {
    public:
        CCamera();
        ~CCamera() = default;
        
        void CreateView();
        void CreateProjection();

        void Rotate(const glm::vec3& rotation);
        void Translate(const glm::vec3& translation);
        
        void SetPosition(const glm::vec3& position);
        void SetRotation(const glm::vec3& rotation);
        void SetAspect(float width, float height);
        

		_forceinline const glm::mat4& GetView() const
		{
			return m_View;
		}
		
		
		_forceinline const glm::mat4& GetProjection() const
		{
			return m_Projection;
		}
        
		
		_forceinline const glm::vec3& GetPosition() const
		{
			return m_Position;
		}
    private:
        glm::mat4 m_View;
        glm::mat4 m_Projection;
        glm::vec3 m_Position;
        glm::vec3 m_Rotation;
        glm::vec3 m_Forward;
        glm::vec3 m_Right;
        glm::vec3 m_Up;
        float m_Aspect;
    };
}
