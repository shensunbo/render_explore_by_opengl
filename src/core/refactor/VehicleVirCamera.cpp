#include "VehicleVirCamera.h"

//TODO
// Default camera values
// const float YAW = -90.0f;
// const float PITCH = 0.0f;
const float SPEED = 1.0f;
//const float SPEED = 3.0f;
const float SENSITIVITY = 0.05f;
const float ZOOM = 45.0f;

glm::vec3 rPos = glm::vec3(0.0f, 0.0f, 0.8f);
//glm::vec3 rPos = glm::vec3(0.0f, 0.0f, 16.0f);

glm::vec3 K1Pos = glm::vec3(0.0f, 150.0f, 10.0f);

VehicleVirCamera::VehicleVirCamera(glm::vec3 position, glm::vec3 up, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
{
    Position = position;
    WorldUp = up;
    Yaw = yaw;
    Pitch = pitch;
    updateCameraVectors();

    jumpStrength = 5.0f; 
    gravity = 9.8f; 
    isJumping = false; 
    isOnGround = true; 
    playerVelocity = glm::vec3(0.0f, 0.0f, 0.0f);
}
// constructor with scalar values
VehicleVirCamera::VehicleVirCamera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
{
    Position = glm::vec3(posX, posY, posZ);
    WorldUp = glm::vec3(upX, upY, upZ);
    Yaw = yaw;
    Pitch = pitch;
    updateCameraVectors();

    jumpStrength = 5.0f; // 跳跃力量
    gravity = 9.8f; // 重力加速度
    isJumping = false; // 是否正在跳跃
    isOnGround = true; // 角色是否在地面上
    playerVelocity = glm::vec3(0.0f, 0.0f, 0.0f);
}

// processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
void VehicleVirCamera::ProcessKeyboard(Camera_Movement direction, float deltaTime)
{
    updateEvent = true;

    float velocity = MovementSpeed * deltaTime;
    if (direction == FORWARD)
        Position += Front * velocity;
    if (direction == BACKWARD)
        Position -= Front * velocity;
    if (direction == LEFT)
        Position -= Right * velocity;
    if (direction == RIGHT)
        Position += Right * velocity;
    if (direction == UPROLL)
        Position -= Up * velocity;
    if (direction == DOWNROLL)
        Position += Up * velocity;
    if (direction == R)
    {
        Position = rPos;
        //Front = glm::normalize(glm::vec3(0.0f, 0.0f, 0.0f));
        Front = -Position;
        Yaw = -90.0f;
        Pitch = 0.0f;
    }

    if (direction == K1)
    {
        Position = K1Pos;
        //Front = glm::normalize(glm::vec3(0.0f, 0.0f, 0.0f));
        Front = -Position;
        Yaw =  -90.0f;
        Pitch = 0.0f;
    }
        
    if (direction == SPACE)
    {
        // 触发跳跃
        isJumping = true;
        isOnGround = false;
        playerVelocity.y = jumpStrength;
    }

}

// processes input received from a mouse input system. Expects the offset value in both the x and y direction.
void VehicleVirCamera::ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch)
{
    updateEvent = true;

    xoffset *= MouseSensitivity;
    yoffset *= MouseSensitivity;

    Yaw += xoffset;
    Pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (constrainPitch)
    {
        if (Pitch > 89.0f)
            Pitch = 89.0f;
        if (Pitch < -89.0f)
            Pitch = -89.0f;
    }

    // update Front, Right and Up Vectors using the updated Euler angles
    updateCameraVectors();
}

// processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
void VehicleVirCamera::ProcessMouseScroll(float yoffset)
{
    updateEvent = true;
    
    Zoom -= (float)yoffset;
    if (Zoom < 1.0f)
        Zoom = 1.0f;
    if (Zoom > 45.0f)
        Zoom = 45.0f;
}

void VehicleVirCamera::ProcessJump(float deltaTime)
{
    if (isJumping)
    {
        // 应用重力
        playerVelocity.y -= gravity * deltaTime;

        // 更新角色的位置
        Position += playerVelocity * deltaTime;

        // 检查是否落地
        if (Position.y <= 0.0f)
        {
            Position.y = 0.0f;
            isJumping = false;
            isOnGround = true;
            playerVelocity.y = 0.0f;
        }
    }
}

void VehicleVirCamera::updateCameraVectors()
{
    // calculate the new Front vector
    glm::vec3 front;
    front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    front.y = sin(glm::radians(Pitch));
    front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    Front = glm::normalize(front);
    // also re-calculate the Right and Up vector
    Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    Up = glm::normalize(glm::cross(Right, Front));
}