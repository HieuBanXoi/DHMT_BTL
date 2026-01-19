cmake_minimum_required(VERSION 3.20)

# Định nghĩa dự án
project(OpenGLStarter 
    VERSION 1.0.0 
    LANGUAGES CXX C
)

# Thiết lập chuẩn C++ (C++20 cho các tính năng hiện đại)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# --- TÌM KIẾM GÓI (Được cung cấp bởi vcpkg toolchain) ---
find_package(OpenGL REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(Stb REQUIRED) 

# --- CẤU HÌNH TỆP THỰC THI ---
add_executable(${PROJECT_NAME} 
    src/main.cpp
    src/stb_impl.cpp 
    src/Camera.cpp
    src/SceneNode.cpp
    src/Shader.cpp
    src/GLUtils.cpp
    src/SchoolBuilder.cpp
)

# --- LIÊN KẾT THƯ VIỆN ---
target_link_libraries(${PROJECT_NAME} PRIVATE 
    OpenGL::GL
    glad::glad
    glfw      # Lưu ý: target của GLFW là 'glfw', không phải 'glfw3'
    glm::glm
    imgui::imgui
)

# Xử lý đặc biệt cho STB
if(Stb_FOUND)
    target_include_directories(${PROJECT_NAME} PRIVATE ${Stb_INCLUDE_DIR})
endif()

# Copy shaders to output directory (rename 'glsl shaders' to 'shaders')
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/glsl shaders"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders"
    COMMENT "Copying shaders to output directory..."
)