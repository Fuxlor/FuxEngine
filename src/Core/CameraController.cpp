#include "Core/CameraController.h"

#include <stdexcept>

namespace FuxEngine
{
    CameraController::CameraController(GLFWwindow* window, bool captureMouse)
        : m_Window(window)
    {
        if (!m_Window)
            throw std::invalid_argument("CameraController requires a valid GLFW window");

        m_PreviousUserPointer = glfwGetWindowUserPointer(m_Window);
        m_PreviousCursorMode = glfwGetInputMode(m_Window, GLFW_CURSOR);

        int width = 0;
        int height = 0;
        glfwGetFramebufferSize(m_Window, &width, &height);
        if (width > 0 && height > 0)
            m_AspectRatio = static_cast<float>(width) / static_cast<float>(height);

        glfwSetWindowUserPointer(m_Window, this);
        m_PreviousCursorCallback = glfwSetCursorPosCallback(m_Window, MouseCallback);
        if (captureMouse)
            glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }

    CameraController::~CameraController()
    {
        if (m_Window && glfwGetWindowUserPointer(m_Window) == this)
        {
            glfwSetCursorPosCallback(m_Window, m_PreviousCursorCallback);
            glfwSetWindowUserPointer(m_Window, m_PreviousUserPointer);
            glfwSetInputMode(m_Window, GLFW_CURSOR, m_PreviousCursorMode);
        }
    }

    Camera& CameraController::CreateCamera(const glm::vec3& position)
    {
        auto camera = std::make_unique<Camera>(position);
        camera->SetAspectRatio(m_AspectRatio);
        m_Cameras.push_back(std::move(camera));
        return *m_Cameras.back();
    }

    void CameraController::SetActiveCamera(std::size_t index)
    {
        if (index >= m_Cameras.size())
            throw std::out_of_range("Active camera index is out of range");
        m_ActiveCameraIndex = index;
        m_FirstMouse = true;
    }

    Camera& CameraController::GetActiveCamera()
    {
        if (m_Cameras.empty())
            throw std::logic_error("CameraController has no cameras");
        return *m_Cameras[m_ActiveCameraIndex];
    }

    const Camera& CameraController::GetActiveCamera() const
    {
        if (m_Cameras.empty())
            throw std::logic_error("CameraController has no cameras");
        return *m_Cameras[m_ActiveCameraIndex];
    }

    std::size_t CameraController::GetActiveCameraIndex() const
    {
        return m_ActiveCameraIndex;
    }

    std::size_t CameraController::GetCameraCount() const
    {
        return m_Cameras.size();
    }

    void CameraController::Update(float deltaTime)
    {
        UpdateAspectRatio();

        for (std::size_t index = 0; index < m_Cameras.size() && index < 9; ++index)
        {
            const int key = GLFW_KEY_1 + static_cast<int>(index);
            if (glfwGetKey(m_Window, key) == GLFW_PRESS && index != m_ActiveCameraIndex)
            {
                SetActiveCamera(index);
                break;
            }
        }

        if (m_Cameras.empty())
            return;

        Camera& camera = GetActiveCamera();
        if (glfwGetKey(m_Window, GLFW_KEY_W) == GLFW_PRESS)
            camera.ProcessKeyboard(CameraMovement::Forward, deltaTime);
        if (glfwGetKey(m_Window, GLFW_KEY_S) == GLFW_PRESS)
            camera.ProcessKeyboard(CameraMovement::Backward, deltaTime);
        if (glfwGetKey(m_Window, GLFW_KEY_A) == GLFW_PRESS)
            camera.ProcessKeyboard(CameraMovement::Left, deltaTime);
        if (glfwGetKey(m_Window, GLFW_KEY_D) == GLFW_PRESS)
            camera.ProcessKeyboard(CameraMovement::Right, deltaTime);
    }

    void CameraController::MouseCallback(GLFWwindow* window, double x, double y)
    {
        auto* controller = static_cast<CameraController*>(glfwGetWindowUserPointer(window));
        if (controller)
            controller->HandleMouseMove(x, y);
    }

    void CameraController::HandleMouseMove(double x, double y)
    {
        if (m_Cameras.empty())
            return;

        if (m_FirstMouse)
        {
            m_LastMouseX = static_cast<float>(x);
            m_LastMouseY = static_cast<float>(y);
            m_FirstMouse = false;
            return;
        }

        const float xOffset = static_cast<float>(x) - m_LastMouseX;
        const float yOffset = m_LastMouseY - static_cast<float>(y);
        m_LastMouseX = static_cast<float>(x);
        m_LastMouseY = static_cast<float>(y);

        GetActiveCamera().ProcessMouseMovement(xOffset, yOffset);
    }

    void CameraController::UpdateAspectRatio()
    {
        int width = 0;
        int height = 0;
        glfwGetFramebufferSize(m_Window, &width, &height);
        if (width <= 0 || height <= 0)
            return;

        m_AspectRatio = static_cast<float>(width) / static_cast<float>(height);
        for (const auto& camera : m_Cameras)
            camera->SetAspectRatio(m_AspectRatio);
    }
}
