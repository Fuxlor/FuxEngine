#pragma once

#include "Core/Camera.h"

#include <GLFW/glfw3.h>

#include <cstddef>
#include <memory>
#include <vector>

namespace FuxEngine
{
    class CameraController
    {
    public:
        explicit CameraController(GLFWwindow* window, bool captureMouse = true);
        ~CameraController();

        CameraController(const CameraController&) = delete;
        CameraController& operator=(const CameraController&) = delete;

        Camera& CreateCamera(const glm::vec3& position = glm::vec3(0.0f, 0.0f, 3.0f));
        void SetActiveCamera(std::size_t index);
        Camera& GetActiveCamera();
        const Camera& GetActiveCamera() const;
        std::size_t GetActiveCameraIndex() const;
        std::size_t GetCameraCount() const;

        void Update(float deltaTime);

    private:
        static void MouseCallback(GLFWwindow* window, double x, double y);
        void HandleMouseMove(double x, double y);
        void UpdateAspectRatio();

        GLFWwindow* m_Window;
        void* m_PreviousUserPointer = nullptr;
        GLFWcursorposfun m_PreviousCursorCallback = nullptr;
        int m_PreviousCursorMode = GLFW_CURSOR_NORMAL;
        std::vector<std::unique_ptr<Camera>> m_Cameras;
        std::size_t m_ActiveCameraIndex = 0;
        float m_LastMouseX = 0.0f;
        float m_LastMouseY = 0.0f;
        float m_AspectRatio = 16.0f / 9.0f;
        bool m_FirstMouse = true;
    };
}
