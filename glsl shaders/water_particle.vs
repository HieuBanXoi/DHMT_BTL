#version 330 core

// === ATTRIBUTES (Đầu vào từ CPU) ===
layout(location = 0) in vec3 a_initialPosition;  // Vị trí bắt đầu của hạt (vòi phun)
layout(location = 1) in vec3 a_initialVelocity;  // Vận tốc ban đầu (hướng + tốc độ)
layout(location = 2) in float a_startTime;       // Thời điểm hạt bắt đầu phun

// === UNIFORMS (Tham số toàn cục) ===
uniform float u_time;                // Thời gian hiện tại (giây)
uniform mat4 u_model;                // Ma trận model (vị trí đài phun nước)
uniform mat4 u_view;                 // Ma trận view (camera)
uniform mat4 u_projection;           // Ma trận projection

// === OUTPUTS (Đầu ra cho Fragment Shader) ===
out float v_age;                     // Tuổi của hạt (0.0 -> 1.0)
out vec3 v_velocity;                 // Vận tốc hiện tại (để tính màu)

// === CONSTANTS (Hằng số vật lý) ===
const vec3 GRAVITY = vec3(0.0, -9.8, 0.0);  // Gia tốc trọng trường (m/s²)
const float PARTICLE_LIFETIME = 2.5;         // Tuổi thọ tối đa của hạt (giây)

void main()
{
    // === 1. TÍNH TUỔI CỦA HẠT ===
    // Thời gian từ khi hạt bắt đầu phun
    float timeSinceStart = u_time - a_startTime;
    
    // Tạo vòng lặp: khi hạt chết, nó được "tái sinh"
    float particleTime = mod(timeSinceStart, PARTICLE_LIFETIME);
    
    // Tuổi chuẩn hóa (0.0 = mới sinh, 1.0 = sắp chết)
    v_age = particleTime / PARTICLE_LIFETIME;
    
    // === 2. MÔ PHỎNG VẬT LÝ - CHUYỂN ĐỘNG NÉM LÊN ===
    // Công thức: s = s₀ + v₀*t + 0.5*a*t²
    
    // Vị trí = Vị trí đầu + (Vận tốc đầu * thời gian)
    vec3 position = a_initialPosition + (a_initialVelocity * particleTime);
    
    // Thêm ảnh hưởng của trọng lực: + 0.5 * g * t²
    position += 0.5 * GRAVITY * particleTime * particleTime;
    
    // === 3. RESET HẠT KHI RƠI QUÁ THẤP ===
    // Nếu hạt rơi xuống dưới vị trí bắt đầu quá nhiều, reset về vòi phun
    if (position.y < a_initialPosition.y - 2.0) {
        position = a_initialPosition;
        v_age = 0.0;
    }
    
    // === 4. TÍNH VẬN TỐC HIỆN TẠI ===
    // v = v₀ + a*t
    v_velocity = a_initialVelocity + GRAVITY * particleTime;
    
    // === 5. BIẾN ĐỔI TỌA ĐỘ ===
    // Chuyển từ world space -> clip space
    gl_Position = u_projection * u_view * u_model * vec4(position, 1.0);
    
    // === 6. KÍCH THƯỚC HẠT ===
    // Hạt nhỏ dần khi già đi
    float size = mix(8.0, 2.0, v_age);
    gl_PointSize = size;
}
