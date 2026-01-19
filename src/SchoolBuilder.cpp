#include "SchoolBuilder.h"

#include <vector>
#include <cmath>

// Helper to create a simple cuboid
static std::shared_ptr<MeshNode> createCuboid(glm::vec3 size, glm::vec3 color, glm::vec3 pos)
{
    auto node = std::make_shared<MeshNode>(MeshType::Cube);
    node->material.albedo = color;
    
    glm::mat4 t(1.0f);
    t = glm::translate(t, pos);
    t = glm::scale(t, size);
    node->SetLocalTransform(t);
    return node;
}

// Helper to create a detailed window with frame
// returns a SceneNode containing frame and glass
static SceneNode::Ptr createWindow(float width, float height)
{
    auto winNode = std::make_shared<SceneNode>();
    
    float frameThickness = 0.1f;
    float glassDepth = 0.05f;
    float frameDepth = 0.15f;
    
    glm::vec3 frameColor(0.2f, 0.2f, 0.2f); // Dark Grey Frame
    glm::vec3 glassColor(0.4f, 0.6f, 0.8f); // Blueish Glass
    
    // 1. Frame Top
    winNode->AddChild(createCuboid(
        glm::vec3(width, frameThickness, frameDepth), 
        frameColor, 
        glm::vec3(0.0f, height/2.0f - frameThickness/2.0f, 0.0f)
    ));
    
    // 2. Frame Bottom
    winNode->AddChild(createCuboid(
        glm::vec3(width, frameThickness, frameDepth), 
        frameColor, 
        glm::vec3(0.0f, -height/2.0f + frameThickness/2.0f, 0.0f)
    ));
    
    // 3. Frame Left
    winNode->AddChild(createCuboid(
        glm::vec3(frameThickness, height, frameDepth), 
        frameColor, 
        glm::vec3(-width/2.0f + frameThickness/2.0f, 0.0f, 0.0f)
    ));
    
    // 4. Frame Right
    winNode->AddChild(createCuboid(
        glm::vec3(frameThickness, height, frameDepth), 
        frameColor, 
        glm::vec3(width/2.0f - frameThickness/2.0f, 0.0f, 0.0f)
    ));
    
    // 5. Glass Pane (slightly recessed)
    winNode->AddChild(createCuboid(
        glm::vec3(width - 2*frameThickness, height - 2*frameThickness, glassDepth),
        glassColor,
        glm::vec3(0.0f, 0.0f, 0.0f)
    ));

    // 6. Horizontal Bar (optional detail)
    winNode->AddChild(createCuboid(
        glm::vec3(width - 2*frameThickness, frameThickness/2.0f, glassDepth + 0.02f),
        frameColor,
        glm::vec3(0.0f, 0.0f, 0.0f)
    ));
    
    return winNode;
}

// Helper to create a door
static SceneNode::Ptr createDoor(float width, float height)
{
    auto doorGroup = std::make_shared<SceneNode>();
    
    glm::vec3 doorColor(0.4f, 0.25f, 0.1f); // Wood color
    glm::vec3 knobColor(0.8f, 0.7f, 0.2f);  // Brass
    
    // Door Leaf
    auto door = createCuboid(glm::vec3(width, height, 0.1f), doorColor, glm::vec3(0.0f, height/2.0f, 0.0f));
    doorGroup->AddChild(door);
    
    // Knob
    auto knob = createCuboid(glm::vec3(0.1f, 0.1f, 0.15f), knobColor, glm::vec3(width * 0.35f, height * 0.5f, 0.0f));
    doorGroup->AddChild(knob);
    
    return doorGroup;
}

// Helper: Create a large, complex tree
static SceneNode::Ptr createTree(float height = 6.5f)
{
    auto tree = std::make_shared<SceneNode>();
    
    // Trunk (very thick, dark brown)
    glm::vec3 trunkColor(0.3f, 0.18f, 0.08f);  // Very dark brown
    float trunkHeight = height * 0.5f;
    float trunkRadius = 0.35f;  // Very thick trunk
    
    auto trunk = createCuboid(
        glm::vec3(trunkRadius * 2, trunkHeight, trunkRadius * 2),
        trunkColor,
        glm::vec3(0.0f, trunkHeight/2.0f, 0.0f)
    );
    tree->AddChild(trunk);
    
    // Main foliage - bottom layer (very large, dark green)
    glm::vec3 foliageColor1(0.12f, 0.4f, 0.12f);  // Very dark green
    float foliageSize1 = height * 0.6f;
    
    auto foliage1 = createCuboid(
        glm::vec3(foliageSize1, foliageSize1 * 0.8f, foliageSize1),
        foliageColor1,
        glm::vec3(0.0f, trunkHeight + foliageSize1 * 0.4f, 0.0f)
    );
    tree->AddChild(foliage1);
    
    // Middle foliage layer (medium green)
    glm::vec3 foliageColor2(0.2f, 0.5f, 0.2f);  // Medium green
    float foliageSize2 = height * 0.45f;
    
    auto foliage2 = createCuboid(
        glm::vec3(foliageSize2, foliageSize2 * 0.8f, foliageSize2),
        foliageColor2,
        glm::vec3(0.0f, trunkHeight + foliageSize1 * 0.6f, 0.0f)
    );
    tree->AddChild(foliage2);
    
    // Upper foliage layer (light green, smaller)
    glm::vec3 foliageColor3(0.3f, 0.6f, 0.3f);  // Light green
    float foliageSize3 = height * 0.3f;
    
    auto foliage3 = createCuboid(
        glm::vec3(foliageSize3, foliageSize3, foliageSize3),
        foliageColor3,
        glm::vec3(0.0f, trunkHeight + foliageSize1 * 0.8f, 0.0f)
    );
    tree->AddChild(foliage3);
    
    // Lower branches (8 branches at lower height)
    glm::vec3 branchColor(0.38f, 0.23f, 0.13f);
    float branchSize = 0.6f;
    float branchY1 = trunkHeight * 0.6f;
    
    // 4 main branches (cardinal directions)
    auto branch1 = createCuboid(
        glm::vec3(branchSize, 0.12f, 0.12f),
        branchColor,
        glm::vec3(-branchSize/2.0f, branchY1, 0.0f)
    );
    tree->AddChild(branch1);
    
    auto branch2 = createCuboid(
        glm::vec3(branchSize, 0.12f, 0.12f),
        branchColor,
        glm::vec3(branchSize/2.0f, branchY1, 0.0f)
    );
    tree->AddChild(branch2);
    
    auto branch3 = createCuboid(
        glm::vec3(0.12f, 0.12f, branchSize),
        branchColor,
        glm::vec3(0.0f, branchY1, branchSize/2.0f)
    );
    tree->AddChild(branch3);
    
    auto branch4 = createCuboid(
        glm::vec3(0.12f, 0.12f, branchSize),
        branchColor,
        glm::vec3(0.0f, branchY1, -branchSize/2.0f)
    );
    tree->AddChild(branch4);
    
    // Upper branches (4 diagonal branches at higher height)
    float branchY2 = trunkHeight * 0.8f;
    float diagBranchSize = 0.5f;
    
    auto branch5 = createCuboid(
        glm::vec3(diagBranchSize, 0.1f, 0.1f),
        branchColor,
        glm::vec3(-diagBranchSize/2.0f, branchY2, diagBranchSize/2.0f)
    );
    tree->AddChild(branch5);
    
    auto branch6 = createCuboid(
        glm::vec3(diagBranchSize, 0.1f, 0.1f),
        branchColor,
        glm::vec3(diagBranchSize/2.0f, branchY2, diagBranchSize/2.0f)
    );
    tree->AddChild(branch6);
    
    auto branch7 = createCuboid(
        glm::vec3(diagBranchSize, 0.1f, 0.1f),
        branchColor,
        glm::vec3(-diagBranchSize/2.0f, branchY2, -diagBranchSize/2.0f)
    );
    tree->AddChild(branch7);
    
    auto branch8 = createCuboid(
        glm::vec3(diagBranchSize, 0.1f, 0.1f),
        branchColor,
        glm::vec3(diagBranchSize/2.0f, branchY2, -diagBranchSize/2.0f)
    );
    tree->AddChild(branch8);
    
    return tree;
}

static SceneNode::Ptr createWing(float w, float h, float d, bool withWindows, bool isCenter,
                                  float balconyExtraLength = 3.7f, 
                                  float balconyWidthRatio = 1.0f,
                                  float balconyOffsetX = 0.0f,
                                  bool includeLeftRailing = true,
                                  bool includeRightRailing = true,
                                  bool useCustomBarRange = false,
                                  float customBarMinX = 0.0f,
                                  float customBarMaxX = 0.0f,
                                  bool useCustomTopRail = false,
                                  float doorStartX = -1000.0f,
                                  float doorEndX = 1000.0f,
                                  int doorMode = 0,  // 0=auto, 1=single left, 2=single right, 3=symmetric pair, 4=no doors
                                  int doorFloor = 3) // 1=floor1 only, 2=floor2 only, 3=both floors
{
    auto wing = std::make_shared<SceneNode>();
    
    // Main Wall Body
    glm::vec3 wallColor(0.9f, 0.85f, 0.8f); // Cream/White wall
    auto walls = createCuboid(glm::vec3(w, h, d), wallColor, glm::vec3(0.0f, h/2.0f, 0.0f));
    wing->AddChild(walls);
    
    // Roof
    glm::vec3 roofColor(0.7f, 0.3f, 0.3f); // Red roof
    float roofH = 0.5f;
    float overhang = 0.4f;
    auto roof = createCuboid(glm::vec3(w + overhang, roofH, d + overhang), roofColor, glm::vec3(0.0f, h + roofH/2.0f, 0.0f));
    wing->AddChild(roof);
    
    // Add Windows
    if (withWindows)
    {
        float winW = 1.2f;
        float winH = 1.5f;
        float spacing = 2.5f;
        int count = static_cast<int>(w / spacing) - 1;
        
        float startX = -((count - 1) * spacing) / 2.0f;
        
        for (int i = 0; i < count; ++i)
        {
            // Floor 1 - Windows
            auto w1 = createWindow(winW, winH);
            w1->SetLocalTransform(glm::translate(glm::mat4(1.0f), glm::vec3(startX + i*spacing, 1.5f, d/2.0f + 0.1f)));
            wing->AddChild(w1);
            
            // Floor 2 - Windows
            if (h > 4.0f) {
                auto w2 = createWindow(winW, winH);
                w2->SetLocalTransform(glm::translate(glm::mat4(1.0f), glm::vec3(startX + i*spacing, 4.5f, d/2.0f + 0.1f)));
                wing->AddChild(w2);
            }
        }
        
        // Add Classroom Doors - theo chế độ tùy chỉnh
        float doorW = 1.0f;
        float doorH = 2.2f;
        
        // Tầng 1
        if ((doorFloor & 1) && doorMode != 4) {  // doorFloor & 1 = có tầng 1, mode 4 = no doors
            if (doorMode == 1) {
                // Mode 1: Một cửa ở ngoài cùng bên trái
                float doorX = -w/2.0f + 1.5f;  // Gần mép trái
                auto door = createDoor(doorW, doorH);
                door->SetLocalTransform(glm::translate(glm::mat4(1.0f), glm::vec3(doorX, 0.0f, d/2.0f + 0.15f)));
                wing->AddChild(door);
            } else if (doorMode == 2) {
                // Mode 2: Một cửa ở ngoài cùng bên phải
                float doorX = w/2.0f - 1.5f;  // Gần mép phải
                auto door = createDoor(doorW, doorH);
                door->SetLocalTransform(glm::translate(glm::mat4(1.0f), glm::vec3(doorX, 0.0f, d/2.0f + 0.15f)));
                wing->AddChild(door);
            } else if (doorMode == 3) {
                // Mode 3: Hai cửa đối xứng ở giữa
                float doorOffset = 1.5f;
                auto door1 = createDoor(doorW, doorH);
                door1->SetLocalTransform(glm::translate(glm::mat4(1.0f), glm::vec3(-doorOffset, 0.0f, d/2.0f + 0.15f)));
                wing->AddChild(door1);
                
                auto door2 = createDoor(doorW, doorH);
                door2->SetLocalTransform(glm::translate(glm::mat4(1.0f), glm::vec3(doorOffset, 0.0f, d/2.0f + 0.15f)));
                wing->AddChild(door2);
            } else if (doorMode == 0) {
                // Mode 0: Auto - cửa cách một khoảng (logic cũ)
                for (int i = 0; i < count - 1; i += 2) {
                    float doorX = startX + i*spacing + spacing/2.0f;
                    if (doorX >= doorStartX && doorX <= doorEndX) {
                        auto classroomDoor = createDoor(doorW, doorH);
                        classroomDoor->SetLocalTransform(glm::translate(glm::mat4(1.0f), glm::vec3(doorX, 0.0f, d/2.0f + 0.15f)));
                        wing->AddChild(classroomDoor);
                    }
                }
            }
        }
        
        // Tầng 2
        if (h > 4.0f && (doorFloor & 2) && doorMode != 4) {  // doorFloor & 2 = có tầng 2
            if (doorMode == 1) {
                // Mode 1: Một cửa ở ngoài cùng bên trái
                float doorX = -w/2.0f + 1.5f;
                auto door = createDoor(doorW, doorH);
                door->SetLocalTransform(glm::translate(glm::mat4(1.0f), glm::vec3(doorX, 3.5f, d/2.0f + 0.15f)));
                wing->AddChild(door);
            } else if (doorMode == 2) {
                // Mode 2: Một cửa ở ngoài cùng bên phải
                float doorX = w/2.0f - 1.5f;
                auto door = createDoor(doorW, doorH);
                door->SetLocalTransform(glm::translate(glm::mat4(1.0f), glm::vec3(doorX, 3.5f, d/2.0f + 0.15f)));
                wing->AddChild(door);
            } else if (doorMode == 3) {
                // Mode 3: Hai cửa đối xứng ở giữa
                float doorOffset = 1.5f;
                auto door1 = createDoor(doorW, doorH);
                door1->SetLocalTransform(glm::translate(glm::mat4(1.0f), glm::vec3(-doorOffset, 3.5f, d/2.0f + 0.15f)));
                wing->AddChild(door1);
                
                auto door2 = createDoor(doorW, doorH);
                door2->SetLocalTransform(glm::translate(glm::mat4(1.0f), glm::vec3(doorOffset, 3.5f, d/2.0f + 0.15f)));
                wing->AddChild(door2);
            } else if (doorMode == 0) {
                // Mode 0: Auto - cửa cách một khoảng (logic cũ)
                for (int i = 0; i < count - 1; i += 2) {
                    float doorX = startX + i*spacing + spacing/2.0f;
                    if (doorX >= doorStartX && doorX <= doorEndX) {
                        auto classroomDoor2 = createDoor(doorW, doorH);
                        classroomDoor2->SetLocalTransform(glm::translate(glm::mat4(1.0f), glm::vec3(doorX, 3.5f, d/2.0f + 0.15f)));
                        wing->AddChild(classroomDoor2);
                    }
                }
            }
        }

    // Windows on the back side too? Let's just do front for now unless requested.
        // Actually, for U shape, the 'front' of side wings faces inward.
    }
    
    // -- 2nd Floor Corridor / Balcony --
    // Extending from the front face (Z+) at 2nd floor level (y=3.5)
    // The reference shows a continuous U-shaped balcony.
    // For Side Wings: "Front" face is inwards (facing courtyard).
    // For Center Wing: "Front" face is outwards (facing courtyard).
    // Width should cover the entire wing length.
    
    float corridorDepth = 1.6f; // Slightly deeper for walking
    float corridorThickness = 0.2f;
    float floor2Y = 3.5f;
    
    // Cấu hình ban công từ tham số hàm
    float corridorExtraLength = balconyExtraLength;
    float corridorWidthRatio = balconyWidthRatio;
    float corridorOffsetX = balconyOffsetX;
    float corridorWidth = (w + corridorExtraLength) * corridorWidthRatio;
    
    auto corridor = createCuboid(
        glm::vec3(corridorWidth, corridorThickness, corridorDepth),
        wallColor, // Match wall or concrete
        glm::vec3(corridorOffsetX, floor2Y - corridorThickness/2.0f, d/2.0f + corridorDepth/2.0f)
    );
    wing->AddChild(corridor);
    
    // Railings for Corridor - Vertical Bars Style
    float railHeight = 1.0f;
    float railThickness = 0.08f;
    float barThickness = 0.05f;
    glm::vec3 railColor(0.25f, 0.25f, 0.25f); // Grey metal
    
    // Top Horizontal Rail
    if (useCustomTopRail && useCustomBarRange) {
        // Thanh chắn ngang tùy chỉnh: chỉ chạy trong phạm vi thanh chắn dọc
        float customRailWidth = customBarMaxX - customBarMinX;
        float customRailCenterX = (customBarMinX + customBarMaxX) / 2.0f;
        auto topRail = createCuboid(
            glm::vec3(customRailWidth, railThickness, railThickness),
            railColor,
            glm::vec3(customRailCenterX, floor2Y + railHeight, d/2.0f + corridorDepth - railThickness/2.0f)
        );
        wing->AddChild(topRail);
    } else {
        // Thanh chắn ngang mặc định: chạy suốt chiều dài ban công
        auto topRail = createCuboid(
            glm::vec3(corridorWidth, railThickness, railThickness),
            railColor,
            glm::vec3(corridorOffsetX, floor2Y + railHeight, d/2.0f + corridorDepth - railThickness/2.0f)
        );
        wing->AddChild(topRail);
    }
    
    // Vertical bars spacing
    float barSpacing = 0.4f; // Closer spacing for vertical bar look    
    float totalWidth = corridorWidth;
    int numBars = static_cast<int>(totalWidth / barSpacing);
    
    // Phạm vi thanh chắn
    float minBarX, maxBarX;
    if (useCustomBarRange) {
        // Sử dụng phạm vi tùy chỉnh (cho ban công giữa)
        minBarX = customBarMinX;
        maxBarX = customBarMaxX;
    } else {
        // Phạm vi mặc định: toàn bộ ban công
        minBarX = corridorOffsetX - corridorWidth/2.0f;
        maxBarX = corridorOffsetX + corridorWidth/2.0f;
    }
    
    for (int i = 0; i <= numBars; ++i) {
        float x = corridorOffsetX - corridorWidth/2.0f + i * (totalWidth / numBars);
        
        // Chỉ tạo thanh chắn nếu vị trí x nằm trong phạm vi cho phép
        if (x >= minBarX && x <= maxBarX) {
            auto bar = createCuboid(
                glm::vec3(barThickness, railHeight, barThickness),
                railColor,
                glm::vec3(x, floor2Y + railHeight/2.0f, d/2.0f + corridorDepth - railThickness/2.0f)
            );
            wing->AddChild(bar);
        }
    }
    
    // Side railings (Left and Right ends of corridor)?
    // Reference image shows continuous flow, but ends might need railings if not connected.
    // For now, let's add them, they might clip into adjacent buildings which is fine.
    
    // Left side railing
    if (includeLeftRailing) {
        auto leftRail = createCuboid(
            glm::vec3(railThickness, railHeight, corridorDepth),
            railColor,
            glm::vec3(corridorOffsetX - corridorWidth/2.0f + railThickness/2.0f, floor2Y + railHeight/2.0f, d/2.0f + corridorDepth/2.0f)
        );
        wing->AddChild(leftRail);
    }

    // Right side railing
    if (includeRightRailing) {
        auto rightRail = createCuboid(
            glm::vec3(railThickness, railHeight, corridorDepth),
            railColor,
            glm::vec3(corridorOffsetX + corridorWidth/2.0f - railThickness/2.0f, floor2Y + railHeight/2.0f, d/2.0f + corridorDepth/2.0f)
        );
        wing->AddChild(rightRail);
    }
    
    return wing;
}

// Create a parabolic arch gate inspired by Bach Khoa (HUST) entrance
static SceneNode::Ptr createParabolicArchGate(float width, float archHeight)
{
    auto gateNode = std::make_shared<SceneNode>();
    
    // White concrete color for the iconic arch
    glm::vec3 archColor(0.95f, 0.95f, 0.97f); // Bright white concrete
    
    // Parabolic arch parameters
    int segments = 40; // Number of segments to approximate the curve
    float archThickness = 0.8f; // Thickness of the arch structure
    float archDepth = 1.2f; // Depth of the arch (front to back)
    
    // Create the parabolic arch using small cuboid segments
    // Parabola equation: y = -a*x^2 + archHeight
    // We want the arch to touch ground at x = ±width/2
    float a = archHeight / ((width/2.0f) * (width/2.0f));
    
    for (int i = 0; i < segments; ++i)
    {
        float t = (float)i / (float)(segments - 1); // 0 to 1
        float x = -width/2.0f + t * width; // -width/2 to +width/2
        
        // Parabolic height
        float y = -a * x * x + archHeight;
        
        // Calculate next point for segment direction
        float x_next = x + (width / (segments - 1));
        float y_next = -a * x_next * x_next + archHeight;
        
        // Segment length and angle
        float dx = x_next - x;
        float dy = y_next - y;
        float segLength = std::sqrt(dx*dx + dy*dy);
        float angle = std::atan2(dy, dx);
        
        // Create segment
        auto segment = std::make_shared<MeshNode>(MeshType::Cube);
        segment->material.albedo = archColor;
        
        glm::mat4 t_mat(1.0f);
        t_mat = glm::translate(t_mat, glm::vec3(x + dx/2.0f, y + dy/2.0f, 0.0f));
        t_mat = glm::rotate(t_mat, angle, glm::vec3(0.0f, 0.0f, 1.0f));
        t_mat = glm::scale(t_mat, glm::vec3(segLength, archThickness, archDepth));
        segment->SetLocalTransform(t_mat);
        
        gateNode->AddChild(segment);
    }
    
    // Add base pillars/foundation at both ends
    float baseWidth = 1.5f;
    float baseHeight = 2.0f;
    
    auto leftBase = createCuboid(
        glm::vec3(baseWidth, baseHeight, archDepth), 
        archColor, 
        glm::vec3(-width/2.0f, baseHeight/2.0f, 0.0f)
    );
    auto rightBase = createCuboid(
        glm::vec3(baseWidth, baseHeight, archDepth), 
        archColor, 
        glm::vec3(width/2.0f, baseHeight/2.0f, 0.0f)
    );
    
    gateNode->AddChild(leftBase);
    gateNode->AddChild(rightBase);
    
    // Add decorative text panel at the top center (optional)
    auto namePanel = createCuboid(
        glm::vec3(width * 0.4f, 0.6f, 0.3f),
        glm::vec3(0.15f, 0.3f, 0.6f), // Darker blue accent
        glm::vec3(0.0f, archHeight * 0.85f, archDepth/2.0f + 0.2f)
    );
    gateNode->AddChild(namePanel);
    
    // Metal fence extending from the gate
    glm::vec3 fenceColor(0.2f, 0.25f, 0.2f); // Dark green metal
    float fenceHeight = 2.5f;
    float fenceLength = 15.0f;
    
    // Left fence section
    for (int i = 0; i < 20; ++i)
    {
        float xPos = -width/2.0f - baseWidth/2.0f - 0.5f - i * 0.8f;
        if (xPos > -fenceLength)
        {
            auto bar = createCuboid(
                glm::vec3(0.08f, fenceHeight, 0.08f),
                fenceColor,
                glm::vec3(xPos, fenceHeight/2.0f, 0.0f)
            );
            gateNode->AddChild(bar);
        }
    }
    
    // Right fence section
    for (int i = 0; i < 20; ++i)
    {
        float xPos = width/2.0f + baseWidth/2.0f + 0.5f + i * 0.8f;
        if (xPos < fenceLength)
        {
            auto bar = createCuboid(
                glm::vec3(0.08f, fenceHeight, 0.08f),
                fenceColor,
                glm::vec3(xPos, fenceHeight/2.0f, 0.0f)
            );
            gateNode->AddChild(bar);
        }
    }
    
    // Horizontal fence rails
    auto leftRail = createCuboid(
        glm::vec3(fenceLength - width/2.0f, 0.08f, 0.08f),
        fenceColor,
        glm::vec3(-width/2.0f - (fenceLength - width/2.0f)/2.0f, fenceHeight * 0.3f, 0.0f)
    );
    auto rightRail = createCuboid(
        glm::vec3(fenceLength - width/2.0f, 0.08f, 0.08f),
        fenceColor,
        glm::vec3(width/2.0f + (fenceLength - width/2.0f)/2.0f, fenceHeight * 0.3f, 0.0f)
    );
    
    gateNode->AddChild(leftRail);
    gateNode->AddChild(rightRail);
    
    return gateNode;
}

// Create a complete perimeter wall/fence system enclosing the school grounds
static SceneNode::Ptr createPerimeterWall(float width, float depth)
{
    auto wallNode = std::make_shared<SceneNode>();
    
    // Wall parameters
    glm::vec3 brickColor(0.7f, 0.5f, 0.4f); // Reddish brick
    glm::vec3 railColor(0.15f, 0.18f, 0.15f); // Dark metal green
    
    float brickHeight = 1.2f;
    float brickThickness = 0.3f;
    float railHeight = 1.5f;
    float railThickness = 0.06f;
    float railSpacing = 0.8f;
    float totalHeight = brickHeight + railHeight;
    
    // Calculate wall positions (rectangular perimeter)
    float halfW = width / 2.0f;
    float halfD = depth / 2.0f;
    
    // --- FRONT WALL (with gap for gate) ---
    float gateWidth = 14.0f; // Width of the gate opening
    float frontWallSegmentLength = (width - gateWidth) / 2.0f;
    
    // Left segment of front wall
    {
        // Brick base
        auto brickBase = createCuboid(
            glm::vec3(frontWallSegmentLength, brickHeight, brickThickness),
            brickColor,
            glm::vec3(-halfW + frontWallSegmentLength/2.0f, brickHeight/2.0f, halfD)
        );
        wallNode->AddChild(brickBase);
        
        // Metal railings on top
        int numRails = static_cast<int>(frontWallSegmentLength / railSpacing);
        for (int i = 0; i < numRails; ++i)
        {
            float xPos = -halfW + i * railSpacing;
            auto rail = createCuboid(
                glm::vec3(railThickness, railHeight, railThickness),
                railColor,
                glm::vec3(xPos, brickHeight + railHeight/2.0f, halfD)
            );
            wallNode->AddChild(rail);
        }
        
        // Top horizontal rail
        auto topRail = createCuboid(
            glm::vec3(frontWallSegmentLength, railThickness, railThickness),
            railColor,
            glm::vec3(-halfW + frontWallSegmentLength/2.0f, brickHeight + railHeight - railThickness/2.0f, halfD)
        );
        wallNode->AddChild(topRail);
    }
    
    // Right segment of front wall
    {
        // Brick base
        auto brickBase = createCuboid(
            glm::vec3(frontWallSegmentLength, brickHeight, brickThickness),
            brickColor,
            glm::vec3(halfW - frontWallSegmentLength/2.0f, brickHeight/2.0f, halfD)
        );
        wallNode->AddChild(brickBase);
        
        // Metal railings on top
        int numRails = static_cast<int>(frontWallSegmentLength / railSpacing);
        for (int i = 0; i < numRails; ++i)
        {
            float xPos = halfW - frontWallSegmentLength + i * railSpacing;
            auto rail = createCuboid(
                glm::vec3(railThickness, railHeight, railThickness),
                railColor,
                glm::vec3(xPos, brickHeight + railHeight/2.0f, halfD)
            );
            wallNode->AddChild(rail);
        }
        
        // Top horizontal rail
        auto topRail = createCuboid(
            glm::vec3(frontWallSegmentLength, railThickness, railThickness),
            railColor,
            glm::vec3(halfW - frontWallSegmentLength/2.0f, brickHeight + railHeight - railThickness/2.0f, halfD)
        );
        wallNode->AddChild(topRail);
    }
    
    // --- BACK WALL (continuous) ---
    {
        // Brick base
        auto brickBase = createCuboid(
            glm::vec3(width, brickHeight, brickThickness),
            brickColor,
            glm::vec3(0.0f, brickHeight/2.0f, -halfD)
        );
        wallNode->AddChild(brickBase);
        
        // Metal railings on top
        int numRails = static_cast<int>(width / railSpacing);
        for (int i = 0; i < numRails; ++i)
        {
            float xPos = -halfW + i * railSpacing;
            auto rail = createCuboid(
                glm::vec3(railThickness, railHeight, railThickness),
                railColor,
                glm::vec3(xPos, brickHeight + railHeight/2.0f, -halfD)
            );
            wallNode->AddChild(rail);
        }
        
        // Top horizontal rail
        auto topRail = createCuboid(
            glm::vec3(width, railThickness, railThickness),
            railColor,
            glm::vec3(0.0f, brickHeight + railHeight - railThickness/2.0f, -halfD)
        );
        wallNode->AddChild(topRail);
    }
    
    // --- LEFT WALL (continuous) ---
    {
        // Brick base
        auto brickBase = createCuboid(
            glm::vec3(brickThickness, brickHeight, depth),
            brickColor,
            glm::vec3(-halfW, brickHeight/2.0f, 0.0f)
        );
        wallNode->AddChild(brickBase);
        
        // Metal railings on top
        int numRails = static_cast<int>(depth / railSpacing);
        for (int i = 0; i < numRails; ++i)
        {
            float zPos = -halfD + i * railSpacing;
            auto rail = createCuboid(
                glm::vec3(railThickness, railHeight, railThickness),
                railColor,
                glm::vec3(-halfW, brickHeight + railHeight/2.0f, zPos)
            );
            wallNode->AddChild(rail);
        }
        
        // Top horizontal rail
        auto topRail = createCuboid(
            glm::vec3(railThickness, railThickness, depth),
            railColor,
            glm::vec3(-halfW, brickHeight + railHeight - railThickness/2.0f, 0.0f)
        );
        wallNode->AddChild(topRail);
    }
    
    // --- RIGHT WALL (continuous) ---
    {
        // Brick base
        auto brickBase = createCuboid(
            glm::vec3(brickThickness, brickHeight, depth),
            brickColor,
            glm::vec3(halfW, brickHeight/2.0f, 0.0f)
        );
        wallNode->AddChild(brickBase);
        
        // Metal railings on top
        int numRails = static_cast<int>(depth / railSpacing);
        for (int i = 0; i < numRails; ++i)
        {
            float zPos = -halfD + i * railSpacing;
            auto rail = createCuboid(
                glm::vec3(railThickness, railHeight, railThickness),
                railColor,
                glm::vec3(halfW, brickHeight + railHeight/2.0f, zPos)
            );
            wallNode->AddChild(rail);
        }
        
        // Top horizontal rail
        auto topRail = createCuboid(
            glm::vec3(railThickness, railThickness, depth),
            railColor,
            glm::vec3(halfW, brickHeight + railHeight - railThickness/2.0f, 0.0f)
        );
        wallNode->AddChild(topRail);
    }
    
    return wallNode;
}

// Create a modern streetlight
static SceneNode::Ptr createStreetlight(float height)
{
    auto light = std::make_shared<SceneNode>();
    
    // Pole (dark metal)
    float poleHeight = height;
    float poleRadius = 0.08f;
    glm::vec3 poleColor(0.2f, 0.2f, 0.25f); // Dark grey metal
    
    auto pole = createCuboid(
        glm::vec3(poleRadius * 2, poleHeight, poleRadius * 2),
        poleColor,
        glm::vec3(0.0f, poleHeight/2.0f, 0.0f)
    );
    light->AddChild(pole);
    
    // Lamp housing (at top)
    float housingSize = 0.4f;
    glm::vec3 housingColor(0.3f, 0.3f, 0.35f); // Lighter grey
    
    auto housing = createCuboid(
        glm::vec3(housingSize, housingSize * 0.6f, housingSize),
        housingColor,
        glm::vec3(0.0f, poleHeight + housingSize * 0.3f, 0.0f)
    );
    light->AddChild(housing);
    
    // Light bulb (bright yellow/white - emissive)
    glm::vec3 bulbColor(1.0f, 0.95f, 0.7f); // Warm LED color
    
    auto bulb = createCuboid(
        glm::vec3(housingSize * 0.6f, housingSize * 0.4f, housingSize * 0.6f),
        bulbColor,
        glm::vec3(0.0f, poleHeight, 0.0f)
    );
    light->AddChild(bulb);
    
    return light;
}



// Helper to create a straight staircase
// height: total height to climb
// width: width of steps
// depth: total horizontal run
// numSteps: number of risers
SceneNode::Ptr createStaircase(float height, float width, float depth, int numSteps)
{
    auto stairsNode = std::make_shared<SceneNode>();
    
    float stepHeight = height / numSteps;
    float stepDepth = depth / numSteps;
    
    // Materials
    glm::vec3 woodColor(0.55f, 0.35f, 0.15f); // Brown wood for steps
    glm::vec3 metalColor(0.2f, 0.2f, 0.2f);   // Dark grey metal for frame/rails
    
    // 1. Steps
    for (int i = 0; i < numSteps; ++i)
    {
        // Each step is a box
        // Position: X centered, Y goes up, Z goes forward
        // We want the stairs to start at Z=0 and go to Z=-depth (or Z=depth, let's say Z decreases as we go up to match wing depth typically)
        // Let's standard: Starts at Base (0,0,0), goes Up (+Y) and Forward (+Z)
        
        float yPos = i * stepHeight + stepHeight/2.0f;
        float zPos = i * stepDepth + stepDepth/2.0f;
        
        auto step = createCuboid(
            glm::vec3(width, stepHeight, stepDepth),
            woodColor,
            glm::vec3(0.0f, yPos, zPos)
        );
        stairsNode->AddChild(step);
    }
    
    // 2. Stringers (Side supports)
    // Slanted beams on both sides
    float stringerWidth = 0.1f;
    float stringerHeight = std::sqrt(height*height + depth*depth); // Approx length
    float angle = std::atan2(height, depth);
    
    // We'll approximate stringers with rotated cuboids
    // Left Stringer
    auto leftStringer = std::make_shared<MeshNode>(MeshType::Cube);
    leftStringer->material.albedo = metalColor;
    {
        glm::mat4 t(1.0f);
        t = glm::translate(t, glm::vec3(-width/2.0f - stringerWidth/2.0f, height/2.0f, depth/2.0f));
        t = glm::rotate(t, -angle, glm::vec3(1.0f, 0.0f, 0.0f)); // Tilt up
        t = glm::scale(t, glm::vec3(stringerWidth, 0.2f, stringerHeight + 0.5f));
        leftStringer->SetLocalTransform(t);
    }
    stairsNode->AddChild(leftStringer);
    
    // Right Stringer
    auto rightStringer = std::make_shared<MeshNode>(MeshType::Cube);
    rightStringer->material.albedo = metalColor;
    {
        glm::mat4 t(1.0f);
        t = glm::translate(t, glm::vec3(width/2.0f + stringerWidth/2.0f, height/2.0f, depth/2.0f));
        t = glm::rotate(t, -angle, glm::vec3(1.0f, 0.0f, 0.0f));
        t = glm::scale(t, glm::vec3(stringerWidth, 0.2f, stringerHeight + 0.5f));
        rightStringer->SetLocalTransform(t);
    }
    stairsNode->AddChild(rightStringer);
    
    // 3. Railings
    float railHeight = 0.9f;
    
    // Handrails (slanted parallel to stringers)
    // Left Handrail
    auto leftHandrail = std::make_shared<MeshNode>(MeshType::Cube);
    leftHandrail->material.albedo = woodColor; // Wooden handrail
    {
        glm::mat4 t(1.0f);
        t = glm::translate(t, glm::vec3(-width/2.0f - stringerWidth/2.0f, height/2.0f + railHeight, depth/2.0f));
        t = glm::rotate(t, -angle, glm::vec3(1.0f, 0.0f, 0.0f));
        t = glm::scale(t, glm::vec3(0.08f, 0.08f, stringerHeight + 0.5f));
        leftHandrail->SetLocalTransform(t);
    }
    stairsNode->AddChild(leftHandrail);
    
    // Right Handrail
    auto rightHandrail = std::make_shared<MeshNode>(MeshType::Cube);
    rightHandrail->material.albedo = woodColor;
    {
        glm::mat4 t(1.0f);
        t = glm::translate(t, glm::vec3(width/2.0f + stringerWidth/2.0f, height/2.0f + railHeight, depth/2.0f));
        t = glm::rotate(t, -angle, glm::vec3(1.0f, 0.0f, 0.0f));
        t = glm::scale(t, glm::vec3(0.08f, 0.08f, stringerHeight + 0.5f));
        rightHandrail->SetLocalTransform(t);
    }
    stairsNode->AddChild(rightHandrail);
    
    // Vertical Balusters
    int numBalusters = numSteps / 2; // Every other step
    for (int i = 0; i <= numBalusters; ++i)
    {
        float t = (float)i / numBalusters;
        float y = t * height; // Base y
        float z = t * depth;  // Z pos
        
        // Left Post
        auto lPost = createCuboid(
            glm::vec3(0.05f, railHeight, 0.05f),
            metalColor,
            glm::vec3(-width/2.0f - stringerWidth/2.0f, y + railHeight/2.0f, z)
        );
        stairsNode->AddChild(lPost);
        
        // Right Post
        auto rPost = createCuboid(
            glm::vec3(0.05f, railHeight, 0.05f),
            metalColor,
            glm::vec3(width/2.0f + stringerWidth/2.0f, y + railHeight/2.0f, z)
        );
        stairsNode->AddChild(rPost);
    }
    
    return stairsNode;
}

SceneNode::Ptr SchoolBuilder::generateSchool(float size)
{
    auto root = std::make_shared<SceneNode>();
    
    // -- Ground / Courtyard --
    // Sân lát gạch đá với lối đi nổi bật và các khoảng cỏ xung quanh
    {
        float groundSize = 100.0f;
        
        // 1. Nền gạch đá chính (màu xám nhạt)
        glm::vec3 pavingColor(0.7f, 0.7f, 0.75f);
        auto pavedGround = std::make_shared<MeshNode>(MeshType::Cube);
        pavedGround->material.albedo = pavingColor;
        glm::mat4 groundT = glm::mat4(1.0f);
        groundT = glm::translate(groundT, glm::vec3(0.0f, -0.05f, 0.0f));
        groundT = glm::scale(groundT, glm::vec3(groundSize, 0.1f, groundSize));
        pavedGround->SetLocalTransform(groundT);
        root->AddChild(pavedGround);
        
        // 2. Lối đi chính từ cổng đến cửa (màu gạch đỏ nâu nổi bật)
        glm::vec3 pathwayColor(0.75f, 0.45f, 0.35f);  // Màu gạch đỏ nâu
        auto pathway = std::make_shared<MeshNode>(MeshType::Cube);
        pathway->material.albedo = pathwayColor;
        glm::mat4 pathT = glm::mat4(1.0f);
        pathT = glm::translate(pathT, glm::vec3(0.0f, -0.03f, 10.0f));  // Từ cổng (Z=30) đến cửa (Z=-10)
        pathT = glm::scale(pathT, glm::vec3(4.0f, 0.12f, 40.0f));  // Rộng 4m, dài 40m
        pathway->SetLocalTransform(pathT);
        root->AddChild(pathway);
        
        // 3. Các khoảng cỏ xanh nhỏ xung quanh (trong khuôn viên, tránh tòa nhà)
        glm::vec3 grassColor(0.3f, 0.6f, 0.3f);
        
        // Khoảng cỏ phía sau bên trái (nhỏ hơn, gần tường)
        auto grass1 = std::make_shared<MeshNode>(MeshType::Cube);
        grass1->material.albedo = grassColor;
        glm::mat4 g1 = glm::mat4(1.0f);
        g1 = glm::translate(g1, glm::vec3(-18.0f, -0.04f, -8.0f));
        g1 = glm::scale(g1, glm::vec3(5.0f, 0.11f, 4.0f));
        grass1->SetLocalTransform(g1);
        root->AddChild(grass1);
        
        // Khoảng cỏ phía sau bên phải (nhỏ hơn, gần tường)
        auto grass2 = std::make_shared<MeshNode>(MeshType::Cube);
        grass2->material.albedo = grassColor;
        glm::mat4 g2 = glm::mat4(1.0f);
        g2 = glm::translate(g2, glm::vec3(18.0f, -0.04f, -8.0f));
        g2 = glm::scale(g2, glm::vec3(5.0f, 0.11f, 4.0f));
        grass2->SetLocalTransform(g2);
        root->AddChild(grass2);
        
        // Khoảng cỏ bên trái giữa (nhỏ, trong khuôn viên)
        auto grass3 = std::make_shared<MeshNode>(MeshType::Cube);
        grass3->material.albedo = grassColor;
        glm::mat4 g3 = glm::mat4(1.0f);
        g3 = glm::translate(g3, glm::vec3(-20.0f, -0.04f, 3.0f));
        g3 = glm::scale(g3, glm::vec3(4.0f, 0.11f, 5.0f));
        grass3->SetLocalTransform(g3);
        root->AddChild(grass3);
        
        // Khoảng cỏ bên phải giữa (nhỏ, trong khuôn viên)
        auto grass4 = std::make_shared<MeshNode>(MeshType::Cube);
        grass4->material.albedo = grassColor;
        glm::mat4 g4 = glm::mat4(1.0f);
        g4 = glm::translate(g4, glm::vec3(20.0f, -0.04f, 3.0f));
        g4 = glm::scale(g4, glm::vec3(4.0f, 0.11f, 5.0f));
        grass4->SetLocalTransform(g4);
        root->AddChild(grass4);
        
        // Khoảng cỏ phía trước bên trái (nhỏ, gần cổng nhưng trong khuôn viên)
        auto grass5 = std::make_shared<MeshNode>(MeshType::Cube);
        grass5->material.albedo = grassColor;
        glm::mat4 g5 = glm::mat4(1.0f);
        g5 = glm::translate(g5, glm::vec3(-12.0f, -0.04f, 18.0f));
        g5 = glm::scale(g5, glm::vec3(5.0f, 0.11f, 6.0f));
        grass5->SetLocalTransform(g5);
        root->AddChild(grass5);
        
        // Khoảng cỏ phía trước bên phải (nhỏ, gần cổng nhưng trong khuôn viên)
        auto grass6 = std::make_shared<MeshNode>(MeshType::Cube);
        grass6->material.albedo = grassColor;
        glm::mat4 g6 = glm::mat4(1.0f);
        g6 = glm::translate(g6, glm::vec3(12.0f, -0.04f, 18.0f));
        g6 = glm::scale(g6, glm::vec3(5.0f, 0.11f, 6.0f));
        grass6->SetLocalTransform(g6);
        root->AddChild(grass6);
        
        // 4. Cây trong các khoảng cỏ (1 cây mỗi vùng, rất to và phức tạp)
        // Cây trong khoảng cỏ phía sau trái
        auto tree1 = createTree(6.2f);
        tree1->SetLocalTransform(glm::translate(glm::mat4(1.0f), glm::vec3(-18.0f, 0.0f, -8.0f)));
        root->AddChild(tree1);
        
        // Cây trong khoảng cỏ phía sau phải
        auto tree2 = createTree(6.5f);
        tree2->SetLocalTransform(glm::translate(glm::mat4(1.0f), glm::vec3(18.0f, 0.0f, -8.0f)));
        root->AddChild(tree2);
        
        // Cây trong khoảng cỏ bên trái giữa
        auto tree3 = createTree(7.0f);
        tree3->SetLocalTransform(glm::translate(glm::mat4(1.0f), glm::vec3(-20.0f, 0.0f, 3.0f)));
        root->AddChild(tree3);
        
        // Cây trong khoảng cỏ bên phải giữa
        auto tree4 = createTree(6.8f);
        tree4->SetLocalTransform(glm::translate(glm::mat4(1.0f), glm::vec3(20.0f, 0.0f, 3.0f)));
        root->AddChild(tree4);
        
        // Cây trong khoảng cỏ phía trước trái
        auto tree5 = createTree(6.0f);
        tree5->SetLocalTransform(glm::translate(glm::mat4(1.0f), glm::vec3(-12.0f, 0.0f, 18.0f)));
        root->AddChild(tree5);
        
        // Cây trong khoảng cỏ phía trước phải
        auto tree6 = createTree(6.3f);
        tree6->SetLocalTransform(glm::translate(glm::mat4(1.0f), glm::vec3(12.0f, 0.0f, 18.0f)));
        root->AddChild(tree6);
    }

    // School Complex Helper Node
    auto schoolParams = std::make_shared<SceneNode>();
    schoolParams->SetLocalTransform(glm::scale(glm::mat4(1.0f), glm::vec3(size)));
    root->AddChild(schoolParams);

    // Dimensions
    float wingW = 15.0f;
    float wingH = 7.0f; // 2 floors
    float wingD = 6.0f;
    
    // -- Center Wing (Facing Z) --
    // Ban công toàn bộ chiều dài, không có phần mở rộng
    // Không có thanh chắn hai bên, thanh chắn dọc chỉ đến điểm nối với ban công bên
    // Chỉ có 2 cửa đối xứng ở giữa tầng 2, tầng 1 không có cửa
    {
        // Giới hạn thanh chắn: chỉ ở phần giữa, không đến hai đầu
        float centerBarMinX = -5.0f;
        float centerBarMaxX = 5.0f;
        
        auto centerWing = createWing(wingW, wingH, wingD, true, true,
                                     0.0f,   // balconyExtraLength: không mở rộng
                                     1.0f,   // balconyWidthRatio: toàn bộ chiều dài
                                     0.0f,   // balconyOffsetX: ở giữa
                                     false,  // includeLeftRailing: KHÔNG có thanh chắn trái
                                     false,  // includeRightRailing: KHÔNG có thanh chắn phải
                                     true,   // useCustomBarRange: sử dụng phạm vi tùy chỉnh
                                     centerBarMinX,  // customBarMinX
                                     centerBarMaxX,  // customBarMaxX
                                     true,   // useCustomTopRail
                                     -1000.0f,  // doorStartX (không dùng)
                                     1000.0f,   // doorEndX (không dùng)
                                     3,      // doorMode: 3 = hai cửa đối xứng
                                     2);     // doorFloor: 2 = chỉ tầng 2
        centerWing->SetLocalTransform(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -10.0f)));
        schoolParams->AddChild(centerWing);
        
        // Main Entrance (in front of center wing)
        // Cao bằng lan can tầng 2
        auto entrance = std::make_shared<SceneNode>();
        float entSize = 4.0f;
        float entranceHeight = 3.45f;  // Cao bằng lan can tầng 2 (floor2Y + railHeight)
        auto entBlock = createCuboid(glm::vec3(entSize, entranceHeight, 2.0f), glm::vec3(0.8f, 0.8f, 0.9f), glm::vec3(0.0f, entranceHeight/2.0f, 0.0f));
        entrance->AddChild(entBlock);
        
        // Door - phù hợp với entrance
        auto mainDoor = createDoor(2.2f, 2.8f);
        mainDoor->SetLocalTransform(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.1f, 1.1f))); // stick out slightly
        entrance->AddChild(mainDoor);
        
        entrance->SetLocalTransform(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -10.0f + wingD/2.0f + 1.0f)));
        schoolParams->AddChild(entrance);
    }
    
    // -- Left Wing --
    // Ban công chỉ ở nửa bên trái
    // Chỉ có 1 cửa ở ngoài cùng bên trái
    {
        float leftBalconyOffset = -(wingW + 3.7f) / 4.0f;
        
        auto leftWing = createWing(wingW, wingH, wingD, true, false,
                                    3.7f,   // balconyExtraLength
                                    0.5f,   // balconyWidthRatio
                                    leftBalconyOffset,
                                    true,   // includeLeftRailing
                                    false,  // includeRightRailing
                                    false,  // useCustomBarRange
                                    0.0f, 0.0f,
                                    false,  // useCustomTopRail
                                    -1000.0f, 1000.0f,  // door range (không dùng)
                                    1,      // doorMode: 1 = một cửa bên trái
                                    3);     // doorFloor: 3 = cả 2 tầng
        glm::mat4 t = glm::mat4(1.0f);
        t = glm::translate(t, glm::vec3(-wingW/2.0f - wingD/2.0f + 1.0f, 0.0f, -10.0f + wingW/2.0f - wingD/2.0f)); 
        t = glm::rotate(t, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        leftWing->SetLocalTransform(t);
        schoolParams->AddChild(leftWing);
    }
    
    // -- Right Wing --
    // Ban công chỉ ở nửa bên phải
    // Chỉ có 1 cửa ở ngoài cùng bên phải
    {
        float rightBalconyOffset = (wingW + 3.7f) / 4.0f;
        
        auto rightWing = createWing(wingW, wingH, wingD, true, false,
                                     3.7f,   // balconyExtraLength
                                     0.5f,   // balconyWidthRatio
                                     rightBalconyOffset,
                                     false,  // includeLeftRailing
                                     true,   // includeRightRailing
                                     false,  // useCustomBarRange
                                     0.0f, 0.0f,
                                     false,  // useCustomTopRail
                                     -1000.0f, 1000.0f,  // door range (không dùng)
                                     2,      // doorMode: 2 = một cửa bên phải
                                     3);     // doorFloor: 3 = cả 2 tầng
        glm::mat4 t = glm::mat4(1.0f);
        t = glm::translate(t, glm::vec3(wingW/2.0f + wingD/2.0f - 1.0f, 0.0f, -10.0f + wingW/2.0f - wingD/2.0f)); 
        t = glm::rotate(t, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        rightWing->SetLocalTransform(t);
        schoolParams->AddChild(rightWing);
    }
    
    // -- Parabolic Arch Gate (Bach Khoa style) --
    {
        auto gate = createParabolicArchGate(12.0f, 8.0f); // Wider and taller arch
        // Position gate at the front wall opening
        // Perimeter wall is at Z=-5, with depth 50, so front is at Z = -5 + 50/2 = 20
        gate->SetLocalTransform(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 20.0f)));
        schoolParams->AddChild(gate);
    }
    
    // -- Pathways / Courtyard Pavement --
    {
        // Path from gate to entrance
        auto path = std::make_shared<MeshNode>(MeshType::Plane);
        path->material.albedo = glm::vec3(0.7f, 0.7f, 0.65f); // Concrete path
        glm::mat4 t = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.01f, 0.0f)); // Just above grass
        t = glm::scale(t, glm::vec3(4.0f, 1.0f, 20.0f)); // Wide path, long Z
        path->SetLocalTransform(t); // goes from Z=-10 to Z=10 roughly
        schoolParams->AddChild(path);
    }
    
    // -- Perimeter Wall/Fence System --
    {
        // Define the rectangular boundary of the school grounds
        // The school extends roughly from -20 to +20 in X, and -25 to +15 in Z
        float perimeterWidth = 50.0f;  // Total width (X direction)
        float perimeterDepth = 50.0f;  // Total depth (Z direction)
        
        auto perimeter = createPerimeterWall(perimeterWidth, perimeterDepth);
        // Center the perimeter around the school (shift back slightly)
        perimeter->SetLocalTransform(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -5.0f)));
        schoolParams->AddChild(perimeter);
    }
    
    // -- Streetlights Along Pathway (Symmetric) --
    {
        // Place streetlights symmetrically on both sides of the path
        int numPairs = 3; // 3 pairs = 6 lights total
        float lightHeight = 4.0f;
        float spacing = 7.0f; // Distance between pairs
        
        for (int i = 0; i < numPairs; ++i)
        {
            float z = 14.0f - i * spacing; // Z positions: 14, 7, 0
            
            // Left side
            auto streetlightL = createStreetlight(lightHeight);
            streetlightL->SetLocalTransform(glm::translate(glm::mat4(1.0f), glm::vec3(-2.5f, 0.0f, z)));
            schoolParams->AddChild(streetlightL);
            
            // Right side (symmetric)
            auto streetlightR = createStreetlight(lightHeight);
            streetlightR->SetLocalTransform(glm::translate(glm::mat4(1.0f), glm::vec3(2.5f, 0.0f, z)));
            schoolParams->AddChild(streetlightR);
        }
    }


    root->updateGlobalTransform();
    // -- Staircases --
    // We want two staircases, one for each wing (Left and Right), connecting ground to 2nd floor balcony/corridor.
    // Dimensions: Height = 1 floor (3.5m), connect to Z=0 relative to wing?
    // Let's place them at the "back" end of the wings (furthest from center).
    
    // -- Staircases --
    // Repositioned to side walls (outer walls of left and right wings)
    // Rotated 90 degrees to run horizontally along the walls
    // Connect ground level to 2nd floor balcony (Y = 3.5)
    
    // Building layout:
    // - Left Wing: Center at X ≈ -11, extends along Z axis
    // - Right Wing: Center at X ≈ +11, extends along Z axis
    // - Wing dimensions: W=15, H=7, D=6
    // - Outer wall of left wing: X ≈ -11 - 6/2 = -14
    // - Outer wall of right wing: X ≈ +11 + 6/2 = +14
    
    float stairHeight = 3.5f;  // Height to 2nd floor
    float stairWidth = 1.5f;   // Width of stairs
    float stairDepth = 6.0f;   // Horizontal run length
    
    // Left Wing Stair (on outer/west wall)
    {
        auto leftStair = createStaircase(stairHeight, stairWidth, stairDepth, 16);
        
        glm::mat4 t(1.0f);
        // Position at outer wall of left wing
        // X = -14 (outer wall), Z positioned to align with wing
        // Rotate -90 degrees to run along X axis (towards center/+X direction)
        t = glm::translate(t, glm::vec3(-wingW/2.0f - wingD/2.0f + 1.0f - wingD/2.0f - 0.1f, 0.0f, -10.0f + wingW/2.0f - 2.0f+7.45f)); 
        t = glm::rotate(t, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        
        leftStair->SetLocalTransform(t);
        schoolParams->AddChild(leftStair);
    }

    // Right Wing Stair (on outer/east wall)
    {
        auto rightStair = createStaircase(stairHeight, stairWidth, stairDepth, 16);
        
        glm::mat4 t(1.0f);
        // Position at outer wall of right wing (mirrored from left)
        // X = +14 (outer wall), Z positioned to align with wing
        // Rotate +90 degrees to run along X axis (towards center/-X direction)
        t = glm::translate(t, glm::vec3(wingW/2.0f + wingD/2.0f - 1.0f + wingD/2.0f + 0.1f, 0.0f, -10.0f + wingW/2.0f - 2.0f+7.45f)); 
        t = glm::rotate(t, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        
        rightStair->SetLocalTransform(t);
        schoolParams->AddChild(rightStair);
    }

    return root;
}