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
static Camera g_camera(glm::vec3(0.0f, 15.0f, 50.0f)); // Very high and far - panoramic view
static bool g_firstMouse = true;
static float g_lastX = 1280.0f / 2.0f;
static float g_lastY = 720.0f / 2.0f;

// Light control
static bool g_lightsEnabled = true;
static bool g_lightKeyPressed = false;
static float g_lightBrightness = 1.0f;  // Brightness multiplier (0.0 to 2.0)
static bool g_brightnessUpPressed = false;
static bool g_brightnessDownPressed = false;

// Control panel button positions (world space)
struct ButtonBounds {
    glm::vec3 min;
    glm::vec3 max;
    int buttonId; // 1=toggle, 2=plus, 3=minus
};
static std::vector<ButtonBounds> g_controlPanelButtons;

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

// Ray-AABB intersection test
static bool rayAABBIntersection(const glm::vec3& rayOrigin, const glm::vec3& rayDir, 
                                 const glm::vec3& boxMin, const glm::vec3& boxMax, float& t)
{
    glm::vec3 invDir = 1.0f / rayDir;
    glm::vec3 t0 = (boxMin - rayOrigin) * invDir;
    glm::vec3 t1 = (boxMax - rayOrigin) * invDir;
    
    glm::vec3 tmin = glm::min(t0, t1);
    glm::vec3 tmax = glm::max(t0, t1);
    
    float tNear = glm::max(glm::max(tmin.x, tmin.y), tmin.z);
    float tFar = glm::min(glm::min(tmax.x, tmax.y), tmax.z);
    
    if (tNear > tFar || tFar < 0.0f) return false;
    
    t = tNear > 0.0f ? tNear : tFar;
    return true;
}

// Mouse button callback for clicking control panel (using screen center)
static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        // Get window size
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        
        // Use screen center instead of mouse position
        float x = 0.0f;  // Center of NDC
        float y = 0.0f;  // Center of NDC
        
        // Create ray from camera through screen center
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 200.0f);
        glm::mat4 view = g_camera.GetViewMatrix();
        
        glm::vec4 rayClip = glm::vec4(x, y, -1.0f, 1.0f);
        glm::vec4 rayEye = glm::inverse(projection) * rayClip;
        rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);
        glm::vec3 rayWorld = glm::vec3(glm::inverse(view) * rayEye);
        rayWorld = glm::normalize(rayWorld);
        
        glm::vec3 rayOrigin = g_camera.GetPosition();
        
        // Check intersection with control panel buttons
        float closestT = FLT_MAX;
        int hitButton = -1;
        
        for (const auto& button : g_controlPanelButtons)
        {
            float t;
            if (rayAABBIntersection(rayOrigin, rayWorld, button.min, button.max, t))
            {
                if (t < closestT)
                {
                    closestT = t;
                    hitButton = button.buttonId;
                }
            }
        }
        
        // Handle button click
        if (hitButton == 1) // Toggle switch
        {
            g_lightsEnabled = !g_lightsEnabled;
            std::cout << "[Crosshair Click] Switch: Lights " << (g_lightsEnabled ? "ON" : "OFF") << std::endl;
        }
        else if (hitButton == 2) // Plus button
        {
            g_lightBrightness = std::min(2.0f, g_lightBrightness + 0.1f);
            std::cout << "[Crosshair Click] + Button: Brightness " << (int)(g_lightBrightness * 100) << "%" << std::endl;
        }
        else if (hitButton == 3) // Minus button
        {
            g_lightBrightness = std::max(0.0f, g_lightBrightness - 0.1f);
            std::cout << "[Crosshair Click] - Button: Brightness " << (int)(g_lightBrightness * 100) << "%" << std::endl;
        }
    }
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
    
    // Toggle lights with L key
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
    {
        if (!g_lightKeyPressed)
        {
            g_lightsEnabled = !g_lightsEnabled;
            g_lightKeyPressed = true;
            std::cout << "Lights " << (g_lightsEnabled ? "ON" : "OFF") << std::endl;
        }
    }
    else
    {
        g_lightKeyPressed = false;
    }
    
    // Increase brightness with + or = key
    if (glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_KP_ADD) == GLFW_PRESS)
    {
        if (!g_brightnessUpPressed)
        {
            g_lightBrightness = std::min(2.0f, g_lightBrightness + 0.1f);
            g_brightnessUpPressed = true;
            std::cout << "Light Brightness: " << (int)(g_lightBrightness * 100) << "%" << std::endl;
        }
    }
    else
    {
        g_brightnessUpPressed = false;
    }
    
    // Decrease brightness with - key
    if (glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS)
    {
        if (!g_brightnessDownPressed)
        {
            g_lightBrightness = std::max(0.0f, g_lightBrightness - 0.1f);
            g_brightnessDownPressed = true;
            std::cout << "Light Brightness: " << (int)(g_lightBrightness * 100) << "%" << std::endl;
        }
    }
    else
    {
        g_brightnessDownPressed = false;
    }
    
    // === CONTROL PANEL SIMULATION (Number Keys) ===
    // Press 1 to toggle lights (like clicking switch)
    static bool key1Pressed = false;
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
    {
        if (!key1Pressed)
        {
            g_lightsEnabled = !g_lightsEnabled;
            key1Pressed = true;
            std::cout << "[Control Panel] Switch: Lights " << (g_lightsEnabled ? "ON" : "OFF") << std::endl;
        }
    }
    else
    {
        key1Pressed = false;
    }
    
    // Press 2 to increase brightness (like clicking + button)
    static bool key2Pressed = false;
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
    {
        if (!key2Pressed)
        {
            g_lightBrightness = std::min(2.0f, g_lightBrightness + 0.1f);
            key2Pressed = true;
            std::cout << "[Control Panel] + Button: Brightness " << (int)(g_lightBrightness * 100) << "%" << std::endl;
        }
    }
    else
    {
        key2Pressed = false;
    }
    
    // Press 3 to decrease brightness (like clicking - button)
    static bool key3Pressed = false;
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
    {
        if (!key3Pressed)
        {
            g_lightBrightness = std::max(0.0f, g_lightBrightness - 0.1f);
            key3Pressed = true;
            std::cout << "[Control Panel] - Button: Brightness " << (int)(g_lightBrightness * 100) << "%" << std::endl;
        }
    }
    else
    {
        key3Pressed = false;
    }
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
    glfwSetMouseButtonCallback(window, mouse_button_callback);  // Register mouse button callback

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
    // Load shaders
    Shader sceneShader("shaders/scene.vs", "shaders/scene_lighting.fs");
    Shader skyboxShader("shaders/scene.vs", "shaders/skybox_blend.fs");
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
    
    // Setup control panel button bounds for ray casting (no rotation - facing outward)
    glm::mat4 pXform = glm::translate(glm::mat4(1.0f), glm::vec3(5.0f, 0.0f, 22.0f));
    g_controlPanelButtons.push_back({glm::vec3(pXform * glm::vec4(-0.15f, 0.85f, 0.0f, 1.0f)), glm::vec3(pXform * glm::vec4(0.15f, 1.15f, 0.2f, 1.0f)), 1});
    g_controlPanelButtons.push_back({glm::vec3(pXform * glm::vec4(-0.125f, 0.475f, 0.0f, 1.0f)), glm::vec3(pXform * glm::vec4(0.125f, 0.725f, 0.18f, 1.0f)), 2});
    g_controlPanelButtons.push_back({glm::vec3(pXform * glm::vec4(-0.125f, 0.125f, 0.0f, 1.0f)), glm::vec3(pXform * glm::vec4(0.125f, 0.375f, 0.18f, 1.0f)), 3});
    
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
        
        // ESC to exit
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        // Forward keyboard movement to camera
        processKeyboardInput(window, deltaTime);

        // Start ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        
        // === CROSSHAIR (Larger and more visible) ===
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
        ImGui::Begin("Crosshair", nullptr, 
            ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | 
            ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | 
            ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoInputs);
        
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        ImVec2 center(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f);
        
        // Black outline for contrast
        draw_list->AddLine(ImVec2(center.x - 16, center.y), ImVec2(center.x + 16, center.y), IM_COL32(0, 0, 0, 255), 5.0f);
        draw_list->AddLine(ImVec2(center.x, center.y - 16), ImVec2(center.x, center.y + 16), IM_COL32(0, 0, 0, 255), 5.0f);
        
        // White crosshair
        draw_list->AddLine(ImVec2(center.x - 15, center.y), ImVec2(center.x + 15, center.y), IM_COL32(255, 255, 255, 255), 3.0f);
        draw_list->AddLine(ImVec2(center.x, center.y - 15), ImVec2(center.x, center.y + 15), IM_COL32(255, 255, 255, 255), 3.0f);
        
        // Center dot with outline
        draw_list->AddCircleFilled(center, 4.0f, IM_COL32(0, 0, 0, 255));
        draw_list->AddCircleFilled(center, 3.0f, IM_COL32(255, 255, 255, 255));
        
        ImGui::End();

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
            ambientColor = glm::vec3(0.1f, 0.1f, 0.15f); // Brighter at night (was 0.02)
        }
        sceneShader.SetVec3("ambientColor", ambientColor);
        
        // === DIRECTIONAL LIGHTS FROM SUN AND MOON ===
        // These lights move with the sun/moon to simulate them emitting light
        
        // Sun directional light (only active during day) - Softer intensity
        glm::vec3 sunLightDir = glm::normalize(-sunPos); // Direction FROM sun TO scene
        glm::vec3 sunLightColor = glm::vec3(1.0f, 0.95f, 0.8f); // Warm yellow-white
        float sunLightIntensity = isDay ? (3.0f * (sunY / orbitRadius)) : 0.0f; // Reduced to 3x for softer light
        
        sceneShader.SetVec3("sunLightDirection", sunLightDir);
        sceneShader.SetVec3("sunLightColor", sunLightColor);
        sceneShader.SetFloat("sunLightIntensity", sunLightIntensity);
        
        // Moon directional light (only active at night) - EXTREMELY BRIGHT
        glm::vec3 moonLightDir = glm::normalize(-moonPos); // Direction FROM moon TO scene
        glm::vec3 moonLightColor = glm::vec3(0.7f, 0.8f, 1.0f); // Cool blue-white
        float moonLightIntensity = !isDay ? (5.0f * (moonY / orbitRadius)) : 0.0f; // 5x intensity!
        
        sceneShader.SetVec3("moonLightDirection", moonLightDir);
        sceneShader.SetVec3("moonLightColor", moonLightColor);
        sceneShader.SetFloat("moonLightIntensity", moonLightIntensity);
        
        // Set up all point lights: 6 central + 6 perimeter + 3 statue + 3 fountain + 4 corners = 22 total
        // Point lights are ALWAYS ON with constant brightness
        int totalLights = 22;
        float lightHeight = 4.0f;
        sceneShader.SetInt("numPointLights", totalLights);
        
        // Light multiplier based on toggle state and brightness
        float lightMultiplier = g_lightsEnabled ? g_lightBrightness : 0.0f;
        
        // Central pathway lights (6 lights in 3 symmetric pairs) - CONSTANT BRIGHTNESS
        int numPairs = 3;
        float spacing = 7.0f;
        for (int i = 0; i < numPairs; ++i)
        {
            float z = 14.0f - i * spacing;
            
            // Left light - ALWAYS 3.5 intensity
            std::string baseNameL = "pointLights[" + std::to_string(i * 2) + "]";
            sceneShader.SetVec3(baseNameL + ".position", glm::vec3(-2.5f, lightHeight, z));
            sceneShader.SetVec3(baseNameL + ".color", glm::vec3(1.0f, 0.9f, 0.7f));
            sceneShader.SetFloat(baseNameL + ".intensity", 3.5f * lightMultiplier); // Toggle-able
            
            // Right light - ALWAYS 3.5 intensity
            std::string baseNameR = "pointLights[" + std::to_string(i * 2 + 1) + "]";
            sceneShader.SetVec3(baseNameR + ".position", glm::vec3(2.5f, lightHeight, z));
            sceneShader.SetVec3(baseNameR + ".color", glm::vec3(1.0f, 0.9f, 0.7f));
            sceneShader.SetFloat(baseNameR + ".intensity", 3.5f * lightMultiplier); // Toggle-able
        }
        
        // Perimeter lights (6 lights around school - outside sports courts) - CONSTANT BRIGHTNESS
        glm::vec3 perimeterLightPositions[] = {
            glm::vec3(-32.0f, lightHeight, 0.0f),    // Left side 1 (outside basketball court)
            glm::vec3(-39.0f, lightHeight, -15.0f),  // Left side 2 (adjusted by user)
            glm::vec3(32.0f, lightHeight, 0.0f),     // Right side 1 (outside football field)
            glm::vec3(39.0f, lightHeight, -15.0f),   // Right side 2 (adjusted by user)
            glm::vec3(-15.0f, lightHeight, -20.0f),  // Back left
            glm::vec3(15.0f, lightHeight, -20.0f)    // Back right
        };
        
        for (int i = 0; i < 6; ++i)
        {
            std::string baseName = "pointLights[" + std::to_string(6 + i) + "]"; // Start from index 6
            sceneShader.SetVec3(baseName + ".position", perimeterLightPositions[i]);
            sceneShader.SetVec3(baseName + ".color", glm::vec3(1.0f, 0.9f, 0.7f));
            sceneShader.SetFloat(baseName + ".intensity", 4.0f * lightMultiplier); // Toggle-able
        }
        
        // Statue spotlights (3 lights around statue) - CONSTANT BRIGHTNESS
        // Statue is at position (-28, 0, 18), scaled 2.0x (in left front corner)
        glm::vec3 statueLightPositions[] = {
            glm::vec3(-28.0f, 5.0f, 21.0f),   // Front light (elevated for 2x scale)
            glm::vec3(-25.0f, 6.0f, 18.0f),   // Right side light (higher)
            glm::vec3(-31.0f, 5.0f, 15.0f)    // Back left light
        };
        
        for (int i = 0; i < 3; ++i)
        {
            std::string baseName = "pointLights[" + std::to_string(12 + i) + "]"; // Start from index 12
            sceneShader.SetVec3(baseName + ".position", statueLightPositions[i]);
            sceneShader.SetVec3(baseName + ".color", glm::vec3(1.0f, 0.95f, 0.8f)); // Warm golden light
            sceneShader.SetFloat(baseName + ".intensity", 5.0f * lightMultiplier); // Toggle-able
        }
        
        // Fountain underwater lights (3 lights around fountain base) - CONSTANT BRIGHTNESS
        // Fountain is at position (28, 0, 18), scaled 1.5x (in right front corner)
        glm::vec3 fountainLightPositions[] = {
            glm::vec3(28.0f, 1.0f, 21.5f),    // Front light (low, underwater effect)
            glm::vec3(31.0f, 1.5f, 18.0f),    // Right side light
            glm::vec3(25.0f, 1.0f, 14.5f)     // Back left light
        };
        
        for (int i = 0; i < 3; ++i)
        {
            std::string baseName = "pointLights[" + std::to_string(15 + i) + "]"; // Start from index 15
            sceneShader.SetVec3(baseName + ".position", fountainLightPositions[i]);
            sceneShader.SetVec3(baseName + ".color", glm::vec3(0.7f, 0.9f, 1.0f)); // Cool blue-white water light
            sceneShader.SetFloat(baseName + ".intensity", 4.5f * lightMultiplier); // Toggle-able
        }
        
        // Corner lights (4 lights at school corners for better coverage)
        glm::vec3 cornerLightPositions[] = {
            glm::vec3(-35.0f, 5.0f, -20.0f),  // Back left corner
            glm::vec3(35.0f, 5.0f, -20.0f),   // Back right corner
            glm::vec3(-35.0f, 5.0f, 25.0f),   // Front left corner
            glm::vec3(35.0f, 5.0f, 25.0f)     // Front right corner
        };
        
        for (int i = 0; i < 4; ++i)
        {
            std::string baseName = "pointLights[" + std::to_string(18 + i) + "]"; // Start from index 18
            sceneShader.SetVec3(baseName + ".position", cornerLightPositions[i]);
            sceneShader.SetVec3(baseName + ".color", glm::vec3(1.0f, 0.9f, 0.7f)); // Warm white
            sceneShader.SetFloat(baseName + ".intensity", 5.0f * lightMultiplier); // Toggle-able, bright
        }

        // Update people animations
        SchoolBuilder::updatePeopleAnimation(root, currentFrame);
        
        // Update clock animation
        SchoolBuilder::updateClockAnimation(root, currentFrame);
        
        // Update cloud animation
        SchoolBuilder::updateCloudAnimation(root, currentFrame);
        
        // Update bird animation
        SchoolBuilder::updateBirdAnimation(root, currentFrame);

        // Update flag animation
        SchoolBuilder::updateFlagAnimation(root, currentFrame);

        // Update fountain animation
        SchoolBuilder::updateFountainAnimation(root, currentFrame);

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