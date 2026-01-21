#version 330 core

// === INPUTS (Từ Vertex Shader) ===
in float v_age;          // Tuổi của hạt (0.0 -> 1.0)
in vec3 v_velocity;      // Vận tốc hiện tại

// === OUTPUT ===
out vec4 FragColor;

void main()
{
    // === 1. TẠO HÌNH TRÒN MỀM MẠI ===
    // gl_PointCoord: (0,0) ở góc trên trái, (1,1) ở góc dưới phải
    vec2 coord = gl_PointCoord - vec2(0.5);  // Chuyển về tâm (0,0)
    float dist = length(coord);               // Khoảng cách từ tâm
    
    // Loại bỏ pixel ngoài hình tròn
    if (dist > 0.5) {
        discard;
    }
    
    // === 2. TẠO ĐỘ MỜ DẦN Ở RÌA ===
    // Hạt mờ dần từ tâm ra rìa (soft edge)
    float alpha = 1.0 - smoothstep(0.3, 0.5, dist);
    
    // === 3. MÀU SẮC DỰA TRÊN TUỔI ===
    // Hạt mới: trắng sáng (nước vừa phun)
    // Hạt già: xanh dương nhạt (nước đang rơi)
    vec3 youngColor = vec3(0.9, 0.95, 1.0);   // Trắng xanh
    vec3 oldColor = vec3(0.4, 0.7, 0.95);     // Xanh dương nhạt
    
    vec3 color = mix(youngColor, oldColor, v_age);
    
    // === 4. ĐỘ TRONG SUỐT DỰA TRÊN TUỔI ===
    // Hạt mới: rõ nét
    // Hạt già: mờ dần (chuẩn bị biến mất)
    float ageAlpha = 1.0 - (v_age * v_age);  // Mờ nhanh hơn khi già
    
    // === 5. HIỆU ỨNG PHẢN CHIẾU ÁNH SÁNG ===
    // Tạo điểm sáng ở tâm hạt (giống giọt nước phản chiếu)
    float highlight = 1.0 - smoothstep(0.0, 0.2, dist);
    color += vec3(highlight * 0.3);
    
    // === 6. KẾT HỢP TẤT CẢ ===
    alpha *= ageAlpha;
    
    // Đảm bảo alpha không quá mờ
    alpha = max(alpha, 0.1);
    
    FragColor = vec4(color, alpha);
}
