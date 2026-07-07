#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
    public:
        // Atributos da câmera
        glm::vec3 Position;
        glm::vec3 Front;
        glm::vec3 Up;

        // Ângulos de Euler
        float Yaw;
        float Pitch;

        // Configurações da câmera
        float Speed;
        float Sensitivity;
        float Zoom;

        // Construtor com valores padrão
        Camera(glm::vec3 position = glm::vec3(0.0f, 1.0f, 5.0f)){
            Position = position;
            Front = glm::vec3(0.0f, 0.0f, -1.0f);
            Up = glm::vec3(0.0f, 1.0f, 0.0f);
            Yaw = -90.f;
            Pitch = 0.0f;
            Speed = 5.0f;
            Sensitivity = 0.1f;
            updateCameraVectors();
        }

        glm::mat4 GetViewMatrix(){
            return glm::lookAt(Position, Position + Front, Up);
        }

        void ProcessKeyboard(int direction, float deltaTime){
            float velocity = Speed * deltaTime;
            if(direction == GLFW_KEY_W)
                Position += Front * velocity;
            if(direction == GLFW_KEY_S)
                Position -= Front * velocity;
            if(direction == GLFW_KEY_A)
                Position -= glm::normalize(glm::cross(Front, Up)) * velocity;
            if(direction == GLFW_KEY_D)
                Position += glm::normalize(glm::cross(Front, Up)) * velocity;
        }

        void ProcessMouseMovement(float xoffset, float yoffset){
            xoffset *= Sensitivity;
            yoffset *= Sensitivity;

            Yaw += xoffset;
            Pitch += yoffset;

            if(Pitch > 89.0f)
                Pitch = 89.0f;
            if(Pitch < -89.0f)
                Pitch = -89.0f;
            
            updateCameraVectors();
        }

        void ProcessMouseScroll(float yoffset)
        {
            Zoom -= (float)yoffset;
            if (Zoom < 1.0f)
                Zoom = 1.0f;
            if (Zoom > 45.0f)
                Zoom = 45.0f;
        }

    private:
        void updateCameraVectors(){
            glm::vec3 front;
            front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
            front.y = sin(glm::radians(Pitch));
            front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
            Front = glm::normalize(front);
        }
};

#endif