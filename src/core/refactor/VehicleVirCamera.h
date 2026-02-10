#pragma once

#include "gl/gl_headers.h"
#include <vector>

// TODO: Flesh out camera controls and inputs.
// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    SPACE,
    R,
    K1,
    K2,
    UPROLL,
    DOWNROLL
};

// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class VehicleVirCamera
{
public:

public:
    // constructor with vectors
    VehicleVirCamera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = -90.0f, float pitch = 0.0f);
    // constructor with scalar values
    VehicleVirCamera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch);

    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix()
    {
        //return glm::lookAt(Position, Position + Front, Up);
        return glm::lookAt(Position, Position + Front, Up);
    }

    // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void ProcessKeyboard(Camera_Movement direction, float deltaTime);
    // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);
    // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void ProcessMouseScroll(float yoffset);
    void ProcessJump(float deltaTime);

private:
    // calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors();

public:
    // camera Attributes
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    // euler Angles
    float Yaw;
    float Pitch;
    // camera options
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;

    float jumpStrength; // Jump impulse strength.
    float gravity; // Gravity acceleration.
    bool isJumping; // Whether the avatar is mid-jump.
    bool isOnGround; // Whether the avatar is grounded.
    glm::vec3 playerVelocity;

    bool updateEvent = true;
private:
    // // Default camera values
    // const float YAW = -90.0f;
    // const float PITCH = 0.0f;
    // const float SPEED = 1.0f;
    // //const float SPEED = 3.0f;
    // const float SENSITIVITY = 0.05f;
    // const float ZOOM = 45.0f;

    // glm::vec3 rPos = glm::vec3(0.0f, 0.0f, 5.0f);
    // //glm::vec3 rPos = glm::vec3(0.0f, 0.0f, 16.0f);

    // glm::vec3 K1Pos = glm::vec3(0.0f, 150.0f, 10.0f);

};