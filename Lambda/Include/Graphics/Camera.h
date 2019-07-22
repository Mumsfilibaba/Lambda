#pragma once
#include "glm/glm.hpp"

namespace Lambda
{
    class Camera
    {
    public:
        Camera();
        ~Camera() = default;
        
        void Rotate(const glm::vec3& rotation);
        void Translate(const glm::vec3& translation);
        
        void SetPosition(const glm::vec3& position);
        void SetRotation(const glm::vec3& rotation);
        
        const glm::mat4& GetView() const { return m_View; }
        const glm::mat4& GetProjection() const { return m_Projection; }
        
        void CreateView();
        void CreateProjection();
        
    private:
        glm::mat4 m_View;
        glm::mat4 m_Projection;
        glm::vec3 m_Position;
        glm::vec3 m_Rotation;
        glm::vec3 m_Forward;
        glm::vec3 m_Right;
        glm::vec3 m_Up;
    };
}
