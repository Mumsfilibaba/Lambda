#include "Graphics/Camera.h"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/euler_angles.hpp"

namespace Lambda
{
    constexpr glm::vec3 FORWARD = glm::vec3(0.0f, 0.0f, 1.0f);
    constexpr glm::vec3 RIGHT   = glm::vec3(1.0f, 0.0f, 0.0f);
    constexpr glm::vec3 UP      = glm::vec3(0.0f, -1.0f, 0.0f);
    
    
    Camera::Camera()
        : m_View(1.0f),
        m_Projection(1.0f),
        m_Position(0.0f, 0.0f, 0.0f),
        m_Rotation(0.0f, 0.0f, 0.0f),
        m_Forward(FORWARD),
        m_Right(RIGHT),
        m_Up(UP)
    {
        CreateView();
        CreateProjection();
    }
    
    
    void Camera::Rotate(const glm::vec3& rotation)
    {
        //Add to rotation
        m_Rotation += rotation;
        
        //Rotate forward vector
        glm::mat3 rotationMat = glm::eulerAngleYXZ(glm::radians(rotation.y), glm::radians(rotation.x), glm::radians(rotation.z));
        m_Forward = rotationMat * m_Forward;
        
        //Rotate right vector
        m_Right = rotationMat * m_Right;
        
        //Rotate up vector
        m_Up = rotationMat * m_Up;
    }
    
    
    void Camera::Translate(const glm::vec3& translation)
    {
        //Move camera
        m_Position += translation.x * m_Right + translation.y * m_Up + translation.z * m_Forward;
    }
    
    
    void Camera::SetPosition(const glm::vec3& position)
    {
        m_Position = position;
    }
    
    
    void Camera::SetRotation(const glm::vec3& rotation)
    {
        m_Rotation = rotation;
        
        //Rotate forward vector
        glm::mat3 rotationMat = glm::eulerAngleYXZ(glm::radians(m_Rotation.y), glm::radians(m_Rotation.x), glm::radians(m_Rotation.z));
        m_Forward = rotationMat * FORWARD;
        
        //Rotate right vector
        m_Right = rotationMat * RIGHT;
    }
    
    
    void Camera::CreateView()
    {
        //Calculate new lookat
        glm::vec3 lookAt = m_Position + m_Forward;
        m_View = glm::lookAt(m_Position, lookAt, m_Up);
    }
    
    
    void Camera::CreateProjection()
    {
        m_Projection = glm::perspective(glm::radians(90.0f), 16.0f / 9.0f, 0.1f, 100.0f);
    }
}
