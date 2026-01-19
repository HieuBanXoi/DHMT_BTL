#include <glad/glad.h>  // GLAD must be included BEFORE GLFW
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <iostream>
#include <stdexcept>
#include <memory>

#include "Camera.h"
#include "Shader.h"
#include "GLUtils.h"
#include "SchoolBuilder.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Callback when the window is resized
static void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

// Globals for mouse handling and camera integration
static Camera g_camera(glm::vec3(0.0f, 1.5f, 8.0f)); // start a bit above ground and back
static bool g_firstMouse = true;
static float g_lastX = 1280.0f / 2.0f;
static float g_lastY = 720.0f / 2.0f;

// Mouse callback to forward movement to the Camera
static void mouse_callback(GLFWwindow* /*window*/, double xpos, double ypos)
{
    float xf = static_cast<float>(xpos);
    float yf = static_cast<float>(ypos);

    if (g_firstMouse)
    {
        g_lastX = xf;
        g_lastY = yf;
        g_firstMouse = false;
    }

    float xoffset = xf - g_lastX;
    float yoffset = g_lastY - yf; // reversed since y-coordinates go from bottom to top

    g_lastX = xf;
    g_lastY = yf;

    g_camera.ProcessMouseMovement(xoffset, yoffset);
}

// Process keyboard input for WASD movement using Camera::Movement
static void processKeyboardInput(GLFWwindow* window, float deltaTime)
{
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        g_camera.ProcessKeyboard(Camera::Movement::Forward, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        g_camera.ProcessKeyboard(Camera::Movement::Backward, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        g_camera.ProcessKeyboard(Camera::Movement::Left, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        g_camera.ProcessKeyboard(Camera::Movement::Right, deltaTime);
}

// Recursive render of SceneNode tree. Uses MeshNode metadata from SchoolBuilder.h
static void RenderNode(const SceneNode::Ptr& node, Shader& shader, GLuint cubeVAO, GLuint planeVAO)
{
    if (!node) return;

    // If MeshNode, set material and model and draw appropriate mesh
    if (auto meshNode = std::dynamic_pointer_cast<MeshNode>(node))
    {
        shader.SetMat4("model", meshNode->GetGlobalTransform());
        shader.SetVec3("albedo", meshNode->material.albedo);

        if (meshNode->mesh == MeshType::Cube)
        {
            glBindVertexArray(cubeVAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        else if (meshNode->mesh == MeshType::Plane)
        {
            glBindVertexArray(planeVAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
        glBindVertexArray(0);
    }

    // Recurse children
    for (auto& c : node->children)
        RenderNode(c, shader, cubeVAO, planeVAO);
}

int main() {
    // 1. Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // Context: OpenGL 4.6 Core Profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // 2. Create window
    GLFWwindow* window = glfwCreateWindow(1280, 720, "School Scene", NULL, NULL);
    if (window == NULL) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Capture and hide the cursor for FPS-style camera
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);

    // 3. Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);

    // 4. Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460");

    // Build resources: shader, geometry VAO and the school scene
    Shader sceneShader("shaders/scene.vs", "shaders/scene_lighting.fs");

    // Create cube VAO 
    GLuint cubeVAO = createCubeVAO();
    // Create plane VAO
    GLuint planeVAO = createPlaneVAO();
    // Create sphere VAO for sun and moon
    GLuint sphereVAO = createSphereVAO();

    // Build school scene
    auto root = SchoolBuilder::generateSchool(1.0f);
    // Ensure transforms are propagated (just in case SchoolBuilder didn't do it)
    root->updateGlobalTransform();
    
    // Increase camera movement speed
    g_camera.MovementSpeed = 8.0f; // Faster movement

    // Timing variables for delta time
    float lastFrame = 0.0f;
    float deltaTime = 0.0f;

    // Sun / lighting defaults
    glm::vec3 sceneCenter = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 sunColor = glm::vec3(1.0f, 0.95f, 0.85f);
    float sunIntensity = 1.0f;

    // 5. Main render loop
    while (!glfwWindowShouldClose(window)) {
        // Per-frame time logic
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Poll events and basic input
        glfwPollEvents();
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        // Forward keyboard movement to camera
        processKeyboardInput(window, deltaTime);

        // Start ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // UI Demo
        ImGui::Begin("OpenGL Configuration");
        static float clear_color[4] = { 0.45f, 0.55f, 0.60f, 1.00f };
        ImGui::ColorEdit3("Clear Color", clear_color);
        ImGui::Text("FPS: %.1f", io.Framerate);
        ImGui::End();

        // Render
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);

        glClearColor(clear_color[0], clear_color[1], clear_color[2], clear_color[3]);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Use scene shader and set global uniforms
        sceneShader.Use();

        glm::mat4 view = g_camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(45.0f),
                                                display_w > 0 ? static_cast<float>(display_w) / static_cast<float>(display_h) : 1.0f,
                                                0.1f, 100.0f);

        sceneShader.SetMat4("view", view);
        sceneShader.SetMat4("projection", projection);

        // Animate sun and moon in VERTICAL orbit (perpendicular - up and down)
        float orbitRadius = 80.0f;  // Extremely large orbit
        float speed = 0.1f; // Even slower for realism
        float angle = currentFrame * speed;
        
        // Sun position - vertical orbit (Y changes, X stays fixed)
        // Orbit in YZ plane instead of XZ plane
        float sunX = 0.0f; // Fixed X position
        float sunY = orbitRadius * sin(angle); // Y goes up and down
        float sunZ = orbitRadius * cos(angle); // Z goes forward/back
        glm::vec3 sunPos = glm::vec3(sunX, sunY, sunZ);
        glm::mat4 sunModel = glm::translate(glm::mat4(1.0f), sunPos);
        
        // Moon position (opposite side)
        float moonY = orbitRadius * sin(angle + 3.14159f);
        float moonZ = orbitRadius * cos(angle + 3.14159f);
        glm::vec3 moonPos = glm::vec3(sunX, moonY, moonZ);
        glm::mat4 moonModel = glm::translate(glm::mat4(1.0f), moonPos);

        // Determine day/night based on sun Y position
        bool isDay = sunY > 0.0f; // Day when sun is above horizon (Y > 0)
        
        // Set sun lighting
        glm::vec3 sunColor = glm::vec3(1.0f, 0.95f, 0.7f);
        float sunIntensity = isDay ? (1.5f * (sunY / orbitRadius)) : 0.0f; // Intensity based on height
        
        sceneShader.SetMat4("sunModel", sunModel);
        sceneShader.SetVec3("sunColor", sunColor);
        sceneShader.SetFloat("sunIntensity", sunIntensity);
        sceneShader.SetVec3("sceneCenter", sceneCenter);
        
        // Adjust ambient light based on time of day (more dramatic difference)
        glm::vec3 ambientColor;
        if (isDay) {
            ambientColor = glm::vec3(0.4f, 0.4f, 0.45f); // Bright ambient during day
        } else {
            ambientColor = glm::vec3(0.02f, 0.02f, 0.05f); // Very dark at night
        }
        sceneShader.SetVec3("ambientColor", ambientColor);
        
        // Set up streetlight point lights (symmetric pairs) - weaker intensity
        int numPairs = 3;
        float lightHeight = 4.0f;
        float spacing = 7.0f;
        sceneShader.SetInt("numPointLights", numPairs * 2);
        
        for (int i = 0; i < numPairs; ++i)
        {
            float z = 14.0f - i * spacing;
            
            // Left light
            std::string baseNameL = "pointLights[" + std::to_string(i * 2) + "]";
            sceneShader.SetVec3(baseNameL + ".position", glm::vec3(-2.5f, lightHeight, z));
            sceneShader.SetVec3(baseNameL + ".color", glm::vec3(1.0f, 0.9f, 0.7f));
            sceneShader.SetFloat(baseNameL + ".intensity", 3.5f); // Reduced from 5.0
            
            // Right light
            std::string baseNameR = "pointLights[" + std::to_string(i * 2 + 1) + "]";
            sceneShader.SetVec3(baseNameR + ".position", glm::vec3(2.5f, lightHeight, z));
            sceneShader.SetVec3(baseNameR + ".color", glm::vec3(1.0f, 0.9f, 0.7f));
            sceneShader.SetFloat(baseNameR + ".intensity", 3.5f); // Reduced from 5.0
        }

        // Update global transforms if any dynamic transforms exist (static in our simple builder)
        root->updateGlobalTransform();

        // Render scene graph
        RenderNode(root, sceneShader, cubeVAO, planeVAO);
        
        // Render Sun and Moon as visible spheres (high in sky)
        {
            // Render Sun (bright yellow sphere) - only visible during day
            if (isDay)
            {
                glm::mat4 sunSphereModel = glm::translate(glm::mat4(1.0f), sunPos);
                sunSphereModel = glm::scale(sunSphereModel, glm::vec3(3.0f)); // Large sun
                
                sceneShader.SetMat4("model", sunSphereModel);
                sceneShader.SetVec3("albedo", glm::vec3(1.0f, 1.0f, 0.6f)); // Bright yellow
                
                glBindVertexArray(sphereVAO);
                glDrawArrays(GL_TRIANGLES, 0, 16 * 32 * 6); // lat * lon * 6 vertices per quad
                glBindVertexArray(0);
            }
            
            // Render Moon (bright white sphere) - only visible at night
            if (!isDay)
            {
                glm::mat4 moonSphereModel = glm::translate(glm::mat4(1.0f), moonPos);
                moonSphereModel = glm::scale(moonSphereModel, glm::vec3(2.0f)); // Smaller moon
                
                sceneShader.SetMat4("model", moonSphereModel);
                sceneShader.SetVec3("albedo", glm::vec3(1.0f, 1.0f, 1.0f)); // Bright white
                
                glBindVertexArray(sphereVAO);
                glDrawArrays(GL_TRIANGLES, 0, 16 * 32 * 6);
                glBindVertexArray(0);
            }
        }

        // Render ImGui draw data
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // Cleanup
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &planeVAO);

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}