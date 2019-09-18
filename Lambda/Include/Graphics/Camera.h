#pragma once
#include "Defines.h"
#include <glm/glm.hpp>

namespace Lambda
{
    //------
    //Camera
    //------
    
    class LAMBDA_API Camera
    {
    public:
        Camera();
        ~Camera() = default;
        
        void Rotate(const glm::vec3& rotation);
        void Translate(const glm::vec3& translation);
        
        void SetPosition(const glm::vec3& position);
        void SetRotation(const glm::vec3& rotation);
        void SetAspect(float width, float height);
        
        const glm::mat4& GetView() const;
        const glm::mat4& GetProjection() const;
        const glm::vec3& GetPosition() const;
        
        void CreateView();
        void CreateProjection();
        
    private:
        glm::mat4   m_View;
        glm::mat4   m_Projection;
        glm::vec3   m_Position;
        glm::vec3   m_Rotation;
        glm::vec3   m_Forward;
        glm::vec3   m_Right;
        glm::vec3   m_Up;
        float       m_Aspect;
    };
    
    
    inline const glm::mat4& Camera::GetView() const
    {
        return m_View;
    }
    
    
    inline const glm::mat4& Camera::GetProjection() const
    {
        return m_Projection;
    }
    
    
    inline const glm::vec3& Camera::GetPosition() const
    {
        return m_Position;
    }
}
