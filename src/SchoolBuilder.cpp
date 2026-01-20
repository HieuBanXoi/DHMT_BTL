#include "SchoolBuilder.h"

#include <vector>
#include <cmath>
#include <string>

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

// Helper to create a flagpole with waving Vietnamese flag
static SceneNode::Ptr createFlagpole(float height = 10.0f)
{
    auto flagpoleNode = std::make_shared<SceneNode>();
    
    // Pole (tall metal pole)
    glm::vec3 poleColor(0.7f, 0.7f, 0.75f); // Silver/grey metal
    float poleRadius = 0.08f;
    
    auto pole = createCuboid(
        glm::vec3(poleRadius * 2, height, poleRadius * 2),
        poleColor,
        glm::vec3(0.0f, height/2.0f, 0.0f)
    );
    flagpoleNode->AddChild(pole);
    
    // Pole base (wider base for stability)
    auto base = createCuboid(
        glm::vec3(0.4f, 0.3f, 0.4f),
        glm::vec3(0.5f, 0.5f, 0.5f), // Dark grey
        glm::vec3(0.0f, 0.15f, 0.0f)
    );
    flagpoleNode->AddChild(base);
    
    // Pole top (decorative finial - gold sphere)
    auto finial = createCuboid(
        glm::vec3(0.2f, 0.2f, 0.2f),
        glm::vec3(0.9f, 0.8f, 0.3f), // Gold color
        glm::vec3(0.0f, height + 0.1f, 0.0f)
    );
    flagpoleNode->AddChild(finial);
    
    // Vietnamese flag (red background with yellow 5-point star)
    // Flag positioned at TOP of pole, extending HORIZONTALLY along X axis
    // FLAT FLAG - no wave animation
    float flagWidth = 3.0f;  // Horizontal length (along X axis)
    float flagHeight = 2.0f; // Vertical height (along Y axis)
    float flagY = height - flagHeight/2.0f - 0.3f; // At top of pole
    
    // Create flat flag - single piece
    int flagSegments = 20;
    float segmentWidth = flagWidth / flagSegments;
    
    glm::vec3 flagRed(0.85f, 0.15f, 0.15f); // Vietnamese flag red
    
    for (int i = 0; i < flagSegments; ++i)
    {
        float xOffset = i * segmentWidth + segmentWidth/2.0f; // Start from pole (x=0)
        
        // Red background segment - completely flat, no waves
        auto flagSegment = createCuboid(
            glm::vec3(segmentWidth * 1.15f, flagHeight, 0.02f),
            flagRed,
            glm::vec3(xOffset, flagY, 0.0f)  // Z = 0, completely flat
        );
        flagpoleNode->AddChild(flagSegment);
    }
    
    // Yellow 5-point star - pixel art approach for accuracy
    glm::vec3 starColor(1.0f, 0.85f, 0.0f); // Yellow/gold
    
    // Star center position (middle of flag in X-Y plane)
    glm::vec3 starCenter(flagWidth/2.0f, flagY, 0.0f);
    
    // Create star using pixel grid - ULTRA HIGH RESOLUTION
    // Grid size: 31x31 for an extremely detailed 5-point star
    // 1 = star pixel, 0 = empty
    // Ngôi sao kích thước 31x31
int starGrid[31][31] = {
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
    {0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0},
    {0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0},
    {0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0},
    {0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0},
    {0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0,0},
    {0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0},
    {0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0},
    {0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0},
    {0,0,0,0,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,0,0,0,0},
    {0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0},
    {0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0},
    {0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0},
    {0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0}
};
    float pixelSize = 0.035f; // Very small pixels for ultra-high resolution
    int gridSize = 31;
    
    // Draw the star pixel by pixel
    // Flag is in X-Y plane (facing Z), so:
    // Grid row -> Y axis (vertical on flag)
    // Grid col -> X axis (horizontal on flag)
    for (int row = 0; row < gridSize; ++row)
    {
        for (int col = 0; col < gridSize; ++col)
        {
            if (starGrid[row][col] == 1)
            {
                // Calculate position
                // Center the grid around starCenter
                float x = starCenter.x + (col - gridSize/2.0f) * pixelSize;
                float y = starCenter.y + (gridSize/2.0f - row) * pixelSize;
                
                auto pixel = createCuboid(
                    glm::vec3(pixelSize, pixelSize, 0.04f),
                    starColor,
                    glm::vec3(x, y, starCenter.z)
                );
                flagpoleNode->AddChild(pixel);
            }
        }
    }

    
    return flagpoleNode;
}

// Helper to create a stone bench
static SceneNode::Ptr createStoneBench()
{
    auto benchNode = std::make_shared<SceneNode>();
    
    glm::vec3 stoneGray(0.6f, 0.6f, 0.65f);
    glm::vec3 stoneDark(0.4f, 0.4f, 0.45f);
    
    float benchLength = 1.5f;
    float benchWidth = 0.4f;
    float benchHeight = 0.45f;
    
    // Seat
    auto seat = createCuboid(glm::vec3(benchLength, 0.08f, benchWidth), stoneGray, glm::vec3(0.0f, benchHeight, 0.0f));
    benchNode->AddChild(seat);
    
    // Backrest
    auto backrest = createCuboid(glm::vec3(benchLength, 0.6f, 0.06f), stoneGray, glm::vec3(0.0f, benchHeight + 0.3f, -benchWidth/2.0f + 0.03f));
    benchNode->AddChild(backrest);
    
    // Legs
    float legW = 0.12f;
    auto leg1 = createCuboid(glm::vec3(legW, benchHeight, legW), stoneDark, glm::vec3(-benchLength/2.0f + 0.16f, benchHeight/2.0f, benchWidth/2.0f - 0.11f));
    auto leg2 = createCuboid(glm::vec3(legW, benchHeight, legW), stoneDark, glm::vec3(benchLength/2.0f - 0.16f, benchHeight/2.0f, benchWidth/2.0f - 0.11f));
    auto leg3 = createCuboid(glm::vec3(legW, benchHeight, legW), stoneDark, glm::vec3(-benchLength/2.0f + 0.16f, benchHeight/2.0f, -benchWidth/2.0f + 0.11f));
    auto leg4 = createCuboid(glm::vec3(legW, benchHeight, legW), stoneDark, glm::vec3(benchLength/2.0f - 0.16f, benchHeight/2.0f, -benchWidth/2.0f + 0.11f));
    benchNode->AddChild(leg1);
    benchNode->AddChild(leg2);
    benchNode->AddChild(leg3);
    benchNode->AddChild(leg4);
    
    // Base
    auto base = createCuboid(glm::vec3(benchLength + 0.2f, 0.05f, benchWidth + 0.1f), stoneDark, glm::vec3(0.0f, 0.025f, 0.0f));
    benchNode->AddChild(base);
    
    return benchNode;
}

// Helper to create picnic table
static SceneNode::Ptr createPicnicTable()
{
    auto tableNode = std::make_shared<SceneNode>();
    
    glm::vec3 woodBrown(0.55f, 0.35f, 0.2f);
    glm::vec3 woodDark(0.35f, 0.25f, 0.15f);
    
    float tableLength = 2.0f;
    float tableWidth = 0.8f;
    float tableHeight = 0.75f;
    
    // Table top planks
    for (int i = 0; i < 5; ++i)
    {
        float z = -tableWidth/2.0f + i * (tableWidth/5.0f) + (tableWidth/10.0f);
        auto plank = createCuboid(glm::vec3(tableLength, 0.05f, tableWidth/5.0f * 0.9f), woodBrown, glm::vec3(0.0f, tableHeight, z));
        tableNode->AddChild(plank);
    }
    
    // Table legs
    float legS = 0.08f;
    auto tleg1 = createCuboid(glm::vec3(legS, tableHeight, legS), woodDark, glm::vec3(-tableLength/2.0f + 0.15f, tableHeight/2.0f, -tableWidth/2.0f + 0.1f));
    auto tleg2 = createCuboid(glm::vec3(legS, tableHeight, legS), woodDark, glm::vec3(tableLength/2.0f - 0.15f, tableHeight/2.0f, -tableWidth/2.0f + 0.1f));
    auto tleg3 = createCuboid(glm::vec3(legS, tableHeight, legS), woodDark, glm::vec3(-tableLength/2.0f + 0.15f, tableHeight/2.0f, tableWidth/2.0f - 0.1f));
    auto tleg4 = createCuboid(glm::vec3(legS, tableHeight, legS), woodDark, glm::vec3(tableLength/2.0f - 0.15f, tableHeight/2.0f, tableWidth/2.0f - 0.1f));
    tableNode->AddChild(tleg1);
    tableNode->AddChild(tleg2);
    tableNode->AddChild(tleg3);
    tableNode->AddChild(tleg4);
    
    // Benches
    float benchH = 0.45f;
    float benchDist = tableWidth/2.0f + 0.4f;
    for (int side = 0; side < 2; ++side)
    {
        float zPos = (side == 0) ? -benchDist : benchDist;
        auto bench1 = createCuboid(glm::vec3(tableLength - 0.2f, 0.04f, 0.135f), woodBrown, glm::vec3(0.0f, benchH, zPos - 0.075f));
        auto bench2 = createCuboid(glm::vec3(tableLength - 0.2f, 0.04f, 0.135f), woodBrown, glm::vec3(0.0f, benchH, zPos + 0.075f));
        auto bleg1 = createCuboid(glm::vec3(legS, benchH, legS), woodDark, glm::vec3(-tableLength/2.0f + 0.3f, benchH/2.0f, zPos));
        auto bleg2 = createCuboid(glm::vec3(legS, benchH, legS), woodDark, glm::vec3(tableLength/2.0f - 0.3f, benchH/2.0f, zPos));
        tableNode->AddChild(bench1);
        tableNode->AddChild(bench2);
        tableNode->AddChild(bleg1);
        tableNode->AddChild(bleg2);
    }
    
    return tableNode;
}

// Helper to create a simple person with articulated limbs for walking animation
static SceneNode::Ptr createPerson(glm::vec3 shirtColor = glm::vec3(0.3f, 0.5f, 0.8f))
{
    auto personNode = std::make_shared<SceneNode>();
    
    glm::vec3 skinColor(0.9f, 0.7f, 0.6f);
    glm::vec3 pantsColor(0.2f, 0.2f, 0.3f);
    glm::vec3 hairColor(0.15f, 0.1f, 0.05f);
    
    float personHeight = 1.7f;
    float headRadius = 0.12f;
    float bodyWidth = 0.25f;
    float bodyHeight = 0.6f;
    float upperLegHeight = 0.25f;
    float lowerLegHeight = 0.25f;
    float upperArmLength = 0.2f;
    float lowerArmLength = 0.2f;
    
    // Head
    auto head = createCuboid(
        glm::vec3(headRadius * 2, headRadius * 2, headRadius * 2),
        skinColor,
        glm::vec3(0.0f, personHeight - headRadius, 0.0f)
    );
    personNode->AddChild(head);
    
    // Hair
    auto hair = createCuboid(
        glm::vec3(headRadius * 2.2f, headRadius * 0.8f, headRadius * 2.2f),
        hairColor,
        glm::vec3(0.0f, personHeight - headRadius/2.0f, 0.0f)
    );
    personNode->AddChild(hair);
    
    // Body (torso)
    auto torso = createCuboid(
        glm::vec3(bodyWidth, bodyHeight, 0.15f),
        shirtColor,
        glm::vec3(0.0f, personHeight - headRadius * 2 - bodyHeight/2.0f, 0.0f)
    );
    personNode->AddChild(torso);
    
    // Arms - Create as separate nodes for animation
    float shoulderY = personHeight - headRadius * 2 - 0.05f;
    
    // Left arm (upper + lower)
    auto leftArmUpper = std::make_shared<SceneNode>();
    auto leftArmUpperMesh = createCuboid(
        glm::vec3(0.06f, upperArmLength, 0.06f),
        shirtColor,
        glm::vec3(0.0f, -upperArmLength/2.0f, 0.0f)
    );
    leftArmUpper->AddChild(leftArmUpperMesh);
    leftArmUpper->SetLocalTransform(glm::translate(glm::mat4(1.0f), 
        glm::vec3(-bodyWidth/2.0f - 0.03f, shoulderY, 0.0f)));
    
    auto leftArmLower = createCuboid(
        glm::vec3(0.05f, lowerArmLength, 0.05f),
        skinColor,
        glm::vec3(0.0f, -upperArmLength - lowerArmLength/2.0f, 0.0f)
    );
    leftArmUpper->AddChild(leftArmLower);
    personNode->AddChild(leftArmUpper);
    
    // Right arm (upper + lower)
    auto rightArmUpper = std::make_shared<SceneNode>();
    auto rightArmUpperMesh = createCuboid(
        glm::vec3(0.06f, upperArmLength, 0.06f),
        shirtColor,
        glm::vec3(0.0f, -upperArmLength/2.0f, 0.0f)
    );
    rightArmUpper->AddChild(rightArmUpperMesh);
    rightArmUpper->SetLocalTransform(glm::translate(glm::mat4(1.0f), 
        glm::vec3(bodyWidth/2.0f + 0.03f, shoulderY, 0.0f)));
    
    auto rightArmLower = createCuboid(
        glm::vec3(0.05f, lowerArmLength, 0.05f),
        skinColor,
        glm::vec3(0.0f, -upperArmLength - lowerArmLength/2.0f, 0.0f)
    );
    rightArmUpper->AddChild(rightArmLower);
    personNode->AddChild(rightArmUpper);
    
    // Legs - Create as separate nodes for animation
    float hipY = personHeight - headRadius * 2 - bodyHeight;
    
    // Left leg (upper + lower)
    auto leftLegUpper = std::make_shared<SceneNode>();
    auto leftLegUpperMesh = createCuboid(
        glm::vec3(0.08f, upperLegHeight, 0.08f),
        pantsColor,
        glm::vec3(0.0f, -upperLegHeight/2.0f, 0.0f)
    );
    leftLegUpper->AddChild(leftLegUpperMesh);
    leftLegUpper->SetLocalTransform(glm::translate(glm::mat4(1.0f), 
        glm::vec3(-bodyWidth/4.0f, hipY, 0.0f)));
    
    auto leftLegLower = createCuboid(
        glm::vec3(0.07f, lowerLegHeight, 0.07f),
        pantsColor,
        glm::vec3(0.0f, -upperLegHeight - lowerLegHeight/2.0f, 0.0f)
    );
    leftLegUpper->AddChild(leftLegLower);
    
    auto leftFoot = createCuboid(
        glm::vec3(0.1f, 0.04f, 0.15f),
        glm::vec3(0.1f, 0.1f, 0.1f),
        glm::vec3(0.0f, -upperLegHeight - lowerLegHeight - 0.02f, 0.05f)
    );
    leftLegUpper->AddChild(leftFoot);
    personNode->AddChild(leftLegUpper);
    
    // Right leg (upper + lower)
    auto rightLegUpper = std::make_shared<SceneNode>();
    auto rightLegUpperMesh = createCuboid(
        glm::vec3(0.08f, upperLegHeight, 0.08f),
        pantsColor,
        glm::vec3(0.0f, -upperLegHeight/2.0f, 0.0f)
    );
    rightLegUpper->AddChild(rightLegUpperMesh);
    rightLegUpper->SetLocalTransform(glm::translate(glm::mat4(1.0f), 
        glm::vec3(bodyWidth/4.0f, hipY, 0.0f)));
    
    auto rightLegLower = createCuboid(
        glm::vec3(0.07f, lowerLegHeight, 0.07f),
        pantsColor,
        glm::vec3(0.0f, -upperLegHeight - lowerLegHeight/2.0f, 0.0f)
    );
    rightLegUpper->AddChild(rightLegLower);
    
    auto rightFoot = createCuboid(
        glm::vec3(0.1f, 0.04f, 0.15f),
        glm::vec3(0.1f, 0.1f, 0.1f),
        glm::vec3(0.0f, -upperLegHeight - lowerLegHeight - 0.02f, 0.05f)
    );
    rightLegUpper->AddChild(rightFoot);
    personNode->AddChild(rightLegUpper);
    
    // Store limb references in children for animation access
    // Order: 0=head, 1=hair, 2=torso, 3=leftArmUpper, 4=rightArmUpper, 5=leftLegUpper, 6=rightLegUpper
    
    return personNode;
}

// Helper to create a wall-mounted clock (no tower, no second hand)
static SceneNode::Ptr createClock()
{
    auto clockNode = std::make_shared<SceneNode>();
    
    glm::vec3 clockFaceColor(0.95f, 0.95f, 0.95f); // White
    glm::vec3 handColor(0.1f, 0.1f, 0.1f); // Black
    glm::vec3 centerColor(0.8f, 0.7f, 0.3f); // Gold
    
    float clockRadius = 0.6f; // Smaller clock
    
    // Clock face (circular - approximated with thin box)
    auto clockFace = createCuboid(
        glm::vec3(clockRadius * 2, clockRadius * 2, 0.05f),
        clockFaceColor,
        glm::vec3(0.0f, 0.0f, 0.0f)
    );
    clockNode->AddChild(clockFace);
    
    // Clock border (frame)
    auto clockBorder = createCuboid(
        glm::vec3(clockRadius * 2.1f, clockRadius * 2.1f, 0.03f),
        glm::vec3(0.2f, 0.2f, 0.2f), // Dark frame
        glm::vec3(0.0f, 0.0f, -0.02f)
    );
    clockNode->AddChild(clockBorder);
    
    // Hour markers (12 markers)
    for (int i = 0; i < 12; ++i)
    {
        float angle = i * 30.0f * 3.14159f / 180.0f; // 30 degrees each
        float markerDist = clockRadius * 0.85f;
        float x = std::sin(angle) * markerDist;
        float y = std::cos(angle) * markerDist;
        
        bool isMainHour = (i % 3 == 0); // 12, 3, 6, 9 are bigger
        float markerSize = isMainHour ? 0.06f : 0.04f;
        
        auto marker = createCuboid(
            glm::vec3(markerSize, markerSize, 0.04f),
            handColor,
            glm::vec3(x, y, 0.03f)
        );
        clockNode->AddChild(marker);
    }
    
    // Hour hand (short, thick) - as separate node for animation
    auto hourHand = std::make_shared<SceneNode>();
    auto hourHandMesh = createCuboid(
        glm::vec3(0.04f, clockRadius * 0.45f, 0.04f),
        handColor,
        glm::vec3(0.0f, clockRadius * 0.225f, 0.0f) // Pivot at bottom
    );
    hourHand->AddChild(hourHandMesh);
    hourHand->SetLocalTransform(glm::translate(glm::mat4(1.0f), 
        glm::vec3(0.0f, 0.0f, 0.05f)));
    clockNode->AddChild(hourHand);
    
    // Minute hand (longer, thinner) - as separate node for animation
    auto minuteHand = std::make_shared<SceneNode>();
    auto minuteHandMesh = createCuboid(
        glm::vec3(0.03f, clockRadius * 0.7f, 0.04f),
        handColor,
        glm::vec3(0.0f, clockRadius * 0.35f, 0.0f) // Pivot at bottom
    );
    minuteHand->AddChild(minuteHandMesh);
    minuteHand->SetLocalTransform(glm::translate(glm::mat4(1.0f), 
        glm::vec3(0.0f, 0.0f, 0.06f)));
    clockNode->AddChild(minuteHand);
    
    // Store hand references in children for animation
    // Order: ..., hourHand, minuteHand are last 2 children (no second hand)
    
    return clockNode;
}

// Helper to create a simple cloud
static SceneNode::Ptr createCloud(float size = 1.0f)
{
    auto cloudNode = std::make_shared<SceneNode>();
    
    glm::vec3 cloudColor(0.95f, 0.95f, 0.98f); // Light white/blue
    
    // Cloud made of multiple "puffs" (cuboids approximating spheres)
    // Center puff (largest)
    auto puff1 = createCuboid(
        glm::vec3(size * 1.2f, size * 0.8f, size * 0.8f),
        cloudColor,
        glm::vec3(0.0f, 0.0f, 0.0f)
    );
    cloudNode->AddChild(puff1);
    
    // Left puff
    auto puff2 = createCuboid(
        glm::vec3(size * 0.9f, size * 0.7f, size * 0.7f),
        cloudColor,
        glm::vec3(-size * 0.8f, -size * 0.1f, 0.0f)
    );
    cloudNode->AddChild(puff2);
    
    // Right puff
    auto puff3 = createCuboid(
        glm::vec3(size * 0.9f, size * 0.7f, size * 0.7f),
        cloudColor,
        glm::vec3(size * 0.8f, -size * 0.1f, 0.0f)
    );
    cloudNode->AddChild(puff3);
    
    // Top puff
    auto puff4 = createCuboid(
        glm::vec3(size * 0.7f, size * 0.6f, size * 0.6f),
        cloudColor,
        glm::vec3(-size * 0.3f, size * 0.4f, 0.0f)
    );
    cloudNode->AddChild(puff4);
    
    // Top right puff
    auto puff5 = createCuboid(
        glm::vec3(size * 0.7f, size * 0.6f, size * 0.6f),
        cloudColor,
        glm::vec3(size * 0.4f, size * 0.3f, 0.0f)
    );
    cloudNode->AddChild(puff5);
    
    return cloudNode;
}

// Helper to create a simple bird (V-shape)
static SceneNode::Ptr createBird(float size = 0.5f, const glm::vec3& color = glm::vec3(0.2f, 0.2f, 0.2f))
{
    auto birdNode = std::make_shared<SceneNode>();
    
    // Left wing
    auto leftWing = createCuboid(
        glm::vec3(size, size/5.0f, size/2.0f),
        color,
        glm::vec3(-size/2.0f, 0.0f, size/4.0f)
    );
    // Rotate wing slightly
    glm::mat4 tL = leftWing->GetLocalTransform();
    tL = glm::rotate(tL, glm::radians(15.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    tL = glm::rotate(tL, glm::radians(-30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    leftWing->SetLocalTransform(tL);
    birdNode->AddChild(leftWing);
    
    // Right wing
    auto rightWing = createCuboid(
        glm::vec3(size, size/5.0f, size/2.0f),
        color,
        glm::vec3(size/2.0f, 0.0f, size/4.0f)
    );
     // Rotate wing slightly
    glm::mat4 tR = rightWing->GetLocalTransform();
    tR = glm::rotate(tR, glm::radians(-15.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    tR = glm::rotate(tR, glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    rightWing->SetLocalTransform(tR);
    birdNode->AddChild(rightWing);
    
    // Body (small center)
    auto body = createCuboid(
        glm::vec3(size/3.0f, size/4.0f, size/1.5f),
        color,
        glm::vec3(0.0f, -size/8.0f, 0.0f)
    );
    birdNode->AddChild(body);

    return birdNode;
}


// Helper to create school name sign
static SceneNode::Ptr createSchoolSign(const std::string & schoolName = "TRUONG HOC")
{
    auto signNode = std::make_shared<SceneNode>();
    
    // Sign board (large rectangular board)
    float signWidth = 8.0f;
    float signHeight = 2.0f;
    float signDepth = 0.2f;
    
    glm::vec3 boardColor(0.15f, 0.25f, 0.45f); // Dark blue
    auto board = createCuboid(
        glm::vec3(signWidth, signHeight, signDepth),
        boardColor,
        glm::vec3(0.0f, 0.0f, 0.0f)
    );
    signNode->AddChild(board);
    
    // Decorative border (gold/yellow frame)
    glm::vec3 frameColor(0.9f, 0.75f, 0.2f); // Gold
    float frameThickness = 0.15f;
    
    // Top frame
    auto frameTop = createCuboid(
        glm::vec3(signWidth, frameThickness, signDepth + 0.05f),
        frameColor,
        glm::vec3(0.0f, signHeight/2.0f - frameThickness/2.0f, 0.0f)
    );
    signNode->AddChild(frameTop);
    
    // Bottom frame
    auto frameBottom = createCuboid(
        glm::vec3(signWidth, frameThickness, signDepth + 0.05f),
        frameColor,
        glm::vec3(0.0f, -signHeight/2.0f + frameThickness/2.0f, 0.0f)
    );
    signNode->AddChild(frameBottom);
    
    // Left frame
    auto frameLeft = createCuboid(
        glm::vec3(frameThickness, signHeight, signDepth + 0.05f),
        frameColor,
        glm::vec3(-signWidth/2.0f + frameThickness/2.0f, 0.0f, 0.0f)
    );
    signNode->AddChild(frameLeft);
    
    // Right frame
    auto frameRight = createCuboid(
        glm::vec3(frameThickness, signHeight, signDepth + 0.05f),
        frameColor,
        glm::vec3(signWidth/2.0f - frameThickness/2.0f, 0.0f, 0.0f)
    );
    signNode->AddChild(frameRight);
    
    // Text representation (simplified as colored rectangles)
    // We'll create simple letter-like shapes
    glm::vec3 textColor(1.0f, 1.0f, 1.0f); // White text
    
    // Create simplified text blocks (representing school name)
    int numLetters = 8;
    float letterSpacing = (signWidth - 1.0f) / numLetters;
    float letterWidth = 0.4f;
    float letterHeight = 1.0f;
    
    for (int i = 0; i < numLetters; ++i)
    {
        float xPos = -signWidth/2.0f + 0.8f + i * letterSpacing;
        
        auto letter = createCuboid(
            glm::vec3(letterWidth, letterHeight, 0.05f),
            textColor,
            glm::vec3(xPos, 0.0f, signDepth/2.0f + 0.05f)
        );
        signNode->AddChild(letter);
    }
    
    // Support posts (two posts holding the sign)
    glm::vec3 postColor(0.3f, 0.25f, 0.2f); // Dark brown wood
    float postHeight = 3.0f;
    float postRadius = 0.15f;
    
    // Left post
    auto leftPost = createCuboid(
        glm::vec3(postRadius * 2, postHeight, postRadius * 2),
        postColor,
        glm::vec3(-signWidth/2.0f + 0.5f, -signHeight/2.0f - postHeight/2.0f, 0.0f)
    );
    signNode->AddChild(leftPost);
    
    // Right post
    auto rightPost = createCuboid(
        glm::vec3(postRadius * 2, postHeight, postRadius * 2),
        postColor,
        glm::vec3(signWidth/2.0f - 0.5f, -signHeight/2.0f - postHeight/2.0f, 0.0f)
    );
    signNode->AddChild(rightPost);
    
    return signNode;
}


// Helper to create a basketball court
static SceneNode::Ptr createBasketballCourt(float length = 28.0f, float width = 15.0f)
{
    auto court = std::make_shared<SceneNode>();
    
    // Court surface (orange/brown color typical of outdoor courts)
    glm::vec3 courtColor(0.85f, 0.5f, 0.3f); // Orange-brown court
    auto surface = createCuboid(
        glm::vec3(length, 0.15f, width),
        courtColor,
        glm::vec3(0.0f, 0.075f, 0.0f)
    );
    court->AddChild(surface);
    
    // Line markings (white)
    glm::vec3 lineColor(0.95f, 0.95f, 0.95f);
    float lineThickness = 0.1f;
    float lineHeight = 0.17f; // Slightly above surface
    
    // Outer boundary lines
    // Top boundary
    auto topLine = createCuboid(
        glm::vec3(length, lineHeight, lineThickness),
        lineColor,
        glm::vec3(0.0f, lineHeight/2.0f, width/2.0f)
    );
    court->AddChild(topLine);
    
    // Bottom boundary
    auto bottomLine = createCuboid(
        glm::vec3(length, lineHeight, lineThickness),
        lineColor,
        glm::vec3(0.0f, lineHeight/2.0f, -width/2.0f)
    );
    court->AddChild(bottomLine);
    
    // Left boundary
    auto leftLine = createCuboid(
        glm::vec3(lineThickness, lineHeight, width),
        lineColor,
        glm::vec3(-length/2.0f, lineHeight/2.0f, 0.0f)
    );
    court->AddChild(leftLine);
    
    // Right boundary
    auto rightLine = createCuboid(
        glm::vec3(lineThickness, lineHeight, width),
        lineColor,
        glm::vec3(length/2.0f, lineHeight/2.0f, 0.0f)
    );
    court->AddChild(rightLine);
    
    // Center circle
    float centerRadius = 1.8f;
    int circleSegments = 60; // Increased for smoother circle
    for (int i = 0; i < circleSegments; ++i)
    {
        float angle1 = (float)i / circleSegments * 2.0f * 3.14159f;
        float angle2 = (float)(i + 1) / circleSegments * 2.0f * 3.14159f;
        
        float x1 = centerRadius * std::cos(angle1);
        float z1 = centerRadius * std::sin(angle1);
        float x2 = centerRadius * std::cos(angle2);
        float z2 = centerRadius * std::sin(angle2);
        
        float segLength = std::sqrt((x2-x1)*(x2-x1) + (z2-z1)*(z2-z1));
        float segAngle = std::atan2(z2-z1, x2-x1);
        
        auto segment = std::make_shared<MeshNode>(MeshType::Cube);
        segment->material.albedo = lineColor;
        
        glm::mat4 t(1.0f);
        t = glm::translate(t, glm::vec3((x1+x2)/2.0f, lineHeight/2.0f, (z1+z2)/2.0f));
        t = glm::rotate(t, segAngle, glm::vec3(0.0f, 1.0f, 0.0f));
        t = glm::scale(t, glm::vec3(segLength, lineHeight, lineThickness));
        segment->SetLocalTransform(t);
        
        court->AddChild(segment);
    }
    
    // Free throw lanes and key areas (paint) for both ends
    float keyWidth = 3.6f;  // Width of the key/lane
    float keyLength = 5.8f; // Length from baseline to free throw line
    
    for (int side = 0; side < 2; ++side)
    {
        float xCenter = (side == 0) ? -length/2.0f : length/2.0f;
        float xOffset = (side == 0) ? keyLength/2.0f : -keyLength/2.0f;
        
        // Free throw lane - front line
        auto ftFront = createCuboid(
            glm::vec3(lineThickness, lineHeight, keyWidth),
            lineColor,
            glm::vec3(xCenter + xOffset, lineHeight/2.0f, 0.0f)
        );
        court->AddChild(ftFront);
        
        // Free throw lane - left side
        auto ftLeft = createCuboid(
            glm::vec3(keyLength, lineHeight, lineThickness),
            lineColor,
            glm::vec3(xCenter + xOffset/2.0f, lineHeight/2.0f, keyWidth/2.0f)
        );
        court->AddChild(ftLeft);
        
        // Free throw lane - right side
        auto ftRight = createCuboid(
            glm::vec3(keyLength, lineHeight, lineThickness),
            lineColor,
            glm::vec3(xCenter + xOffset/2.0f, lineHeight/2.0f, -keyWidth/2.0f)
        );
        court->AddChild(ftRight);
        
        // Free throw circle (top of key) - semicircle only
        float ftCircleRadius = 1.8f;
        for (int i = 0; i < circleSegments/2; ++i)
        {
            // Draw semicircle from -90 to +90 degrees
            float angle1 = -3.14159f/2.0f + (float)i / (circleSegments/2) * 3.14159f;
            float angle2 = -3.14159f/2.0f + (float)(i + 1) / (circleSegments/2) * 3.14159f;
            
            float z1 = ftCircleRadius * std::sin(angle1);
            float z2 = ftCircleRadius * std::sin(angle2);
            float x1 = ftCircleRadius * std::cos(angle1) * (side == 0 ? 1.0f : -1.0f);
            float x2 = ftCircleRadius * std::cos(angle2) * (side == 0 ? 1.0f : -1.0f);
            
            float segLength = std::sqrt((x2-x1)*(x2-x1) + (z2-z1)*(z2-z1));
            float segAngle = std::atan2(z2-z1, x2-x1);
            
            auto segment = std::make_shared<MeshNode>(MeshType::Cube);
            segment->material.albedo = lineColor;
            
            glm::mat4 t(1.0f);
            t = glm::translate(t, glm::vec3(xCenter + xOffset + (x1+x2)/2.0f, lineHeight/2.0f, (z1+z2)/2.0f));
            t = glm::rotate(t, segAngle, glm::vec3(0.0f, 1.0f, 0.0f));
            t = glm::scale(t, glm::vec3(segLength, lineHeight, lineThickness));
            segment->SetLocalTransform(t);
            
            court->AddChild(segment);
        }
    }
    
    // Three-point arcs (more detailed)
    float threePointRadius = 6.75f;
    int arcSegments = 80; // Increased for smoother, continuous arc
    
    for (int side = 0; side < 2; ++side)
    {
        float xCenter = (side == 0) ? -length/2.0f : length/2.0f;
        
        for (int i = 0; i < arcSegments; ++i)
        {
            // Arc from -90 to +90 degrees (semicircle facing the basket)
            float angle1 = -3.14159f/2.0f + (float)i / arcSegments * 3.14159f;
            float angle2 = -3.14159f/2.0f + (float)(i + 1) / arcSegments * 3.14159f;
            
            float z1 = threePointRadius * std::sin(angle1);
            float z2 = threePointRadius * std::sin(angle2);
            float x1 = threePointRadius * std::cos(angle1) * (side == 0 ? 1.0f : -1.0f);
            float x2 = threePointRadius * std::cos(angle2) * (side == 0 ? 1.0f : -1.0f);
            
            if (std::abs(z1) <= width/2.0f && std::abs(z2) <= width/2.0f)
            {
                float segLength = std::sqrt((x2-x1)*(x2-x1) + (z2-z1)*(z2-z1));
                float segAngle = std::atan2(z2-z1, x2-x1);
                
                auto segment = std::make_shared<MeshNode>(MeshType::Cube);
                segment->material.albedo = lineColor;
                
                glm::mat4 t(1.0f);
                t = glm::translate(t, glm::vec3(xCenter + (x1+x2)/2.0f, lineHeight/2.0f, (z1+z2)/2.0f));
                t = glm::rotate(t, segAngle, glm::vec3(0.0f, 1.0f, 0.0f));
                t = glm::scale(t, glm::vec3(segLength, lineHeight, lineThickness));
                segment->SetLocalTransform(t);
                
                court->AddChild(segment);
            }
        }
    }
    
    // Basketball hoops (2 hoops at each end)
    glm::vec3 poleColor(0.3f, 0.3f, 0.35f); // Dark grey metal
    glm::vec3 backboardColor(0.95f, 0.95f, 0.97f); // White/transparent
    glm::vec3 rimColor(0.9f, 0.4f, 0.1f); // Orange rim
    glm::vec3 netColor(0.9f, 0.9f, 0.95f); // White net
    
    float hoopHeight = 3.05f; // Standard 10 feet
    float poleRadius = 0.1f;
    float backboardWidth = 1.8f;
    float backboardHeight = 1.05f;
    float backboardThickness = 0.05f;
    
    for (int side = 0; side < 2; ++side)
    {
        float xPos = (side == 0) ? -length/2.0f - 1.0f : length/2.0f + 1.0f;
        
        // Pole
        auto pole = createCuboid(
            glm::vec3(poleRadius * 2, hoopHeight + 0.5f, poleRadius * 2),
            poleColor,
            glm::vec3(xPos, (hoopHeight + 0.5f)/2.0f, 0.0f)
        );
        court->AddChild(pole);
        
        // Backboard
        auto backboard = createCuboid(
            glm::vec3(backboardThickness, backboardHeight, backboardWidth),
            backboardColor,
            glm::vec3(xPos + (side == 0 ? 0.5f : -0.5f), hoopHeight, 0.0f)
        );
        court->AddChild(backboard);
        
        // Backboard frame (red/orange outline)
        glm::vec3 frameColor(0.8f, 0.2f, 0.1f);
        float frameThick = 0.06f;
        
        // Top frame
        auto frameTop = createCuboid(
            glm::vec3(frameThick, frameThick, backboardWidth),
            frameColor,
            glm::vec3(xPos + (side == 0 ? 0.5f : -0.5f), hoopHeight + backboardHeight/2.0f, 0.0f)
        );
        court->AddChild(frameTop);
        
        // Bottom frame
        auto frameBottom = createCuboid(
            glm::vec3(frameThick, frameThick, backboardWidth),
            frameColor,
            glm::vec3(xPos + (side == 0 ? 0.5f : -0.5f), hoopHeight - backboardHeight/2.0f, 0.0f)
        );
        court->AddChild(frameBottom);
        
        // Rim (horizontal circle in X-Z plane)
        float rimRadius = 0.45f;
        int rimSegments = 40; // Increased for smoother rim
        for (int i = 0; i < rimSegments; ++i)
        {
            float angle1 = (float)i / rimSegments * 2.0f * 3.14159f;
            float angle2 = (float)(i + 1) / rimSegments * 2.0f * 3.14159f;
            
            // Create circle in X-Z plane (horizontal)
            float x1 = rimRadius * std::cos(angle1);
            float z1 = rimRadius * std::sin(angle1);
            float x2 = rimRadius * std::cos(angle2);
            float z2 = rimRadius * std::sin(angle2);
            
            float segLength = std::sqrt((x2-x1)*(x2-x1) + (z2-z1)*(z2-z1));
            float segAngle = std::atan2(z2-z1, x2-x1);
            
            auto segment = std::make_shared<MeshNode>(MeshType::Cube);
            segment->material.albedo = rimColor;
            
            glm::mat4 t(1.0f);
            t = glm::translate(t, glm::vec3(xPos + (side == 0 ? 0.9f : -0.9f) + (x1+x2)/2.0f, hoopHeight - 0.5f, (z1+z2)/2.0f));
            t = glm::rotate(t, segAngle, glm::vec3(0.0f, 1.0f, 0.0f));
            t = glm::scale(t, glm::vec3(segLength, 0.05f, 0.05f));
            segment->SetLocalTransform(t);
            
            court->AddChild(segment);
        }
        
        // Rim support bracket
        auto rimSupport = createCuboid(
            glm::vec3(0.4f, 0.05f, 0.05f),
            poleColor,
            glm::vec3(xPos + (side == 0 ? 0.7f : -0.7f), hoopHeight - 0.5f, 0.0f)
        );
        court->AddChild(rimSupport);
        
        // Net (simplified as vertical strips)
        float netHeight = 0.4f;
        int netStrips = 12;
        for (int i = 0; i < netStrips; ++i)
        {
            float angle = (float)i / netStrips * 2.0f * 3.14159f;
            float netX = rimRadius * 0.8f * std::cos(angle);
            float netZ = rimRadius * 0.8f * std::sin(angle);
            
            auto netStrip = createCuboid(
                glm::vec3(0.02f, netHeight, 0.02f),
                netColor,
                glm::vec3(xPos + (side == 0 ? 0.9f : -0.9f) + netX * 0.3f, hoopHeight - 0.5f - netHeight/2.0f, netZ)
            );
            court->AddChild(netStrip);
        }
    }
    
    return court;
}


// Helper to create a football (soccer) field
static SceneNode::Ptr createFootballField(float length = 40.0f, float width = 25.0f)
{
    auto field = std::make_shared<SceneNode>();
    
    // Field surface (grass green)
    glm::vec3 grassColor(0.25f, 0.55f, 0.25f); // Vibrant grass green
    auto surface = createCuboid(
        glm::vec3(length, 0.15f, width),
        grassColor,
        glm::vec3(0.0f, 0.075f, 0.0f)
    );
    field->AddChild(surface);
    
    // Line markings (white)
    glm::vec3 lineColor(0.95f, 0.95f, 0.95f);
    float lineThickness = 0.12f;
    float lineHeight = 0.22f; // Increased height to prevent z-fighting
    
    // Outer boundary lines
    // Top sideline
    auto topLine = createCuboid(
        glm::vec3(length, lineHeight, lineThickness),
        lineColor,
        glm::vec3(0.0f, lineHeight/2.0f, width/2.0f)
    );
    field->AddChild(topLine);
    
    // Bottom sideline
    auto bottomLine = createCuboid(
        glm::vec3(length, lineHeight, lineThickness),
        lineColor,
        glm::vec3(0.0f, lineHeight/2.0f, -width/2.0f)
    );
    field->AddChild(bottomLine);
    
    // Left goal line
    auto leftLine = createCuboid(
        glm::vec3(lineThickness, lineHeight, width),
        lineColor,
        glm::vec3(-length/2.0f, lineHeight/2.0f, 0.0f)
    );
    field->AddChild(leftLine);
    
    // Right goal line
    auto rightLine = createCuboid(
        glm::vec3(lineThickness, lineHeight, width),
        lineColor,
        glm::vec3(length/2.0f, lineHeight/2.0f, 0.0f)
    );
    field->AddChild(rightLine);
    
    // Center line
    auto centerLine = createCuboid(
        glm::vec3(lineThickness, lineHeight, width),
        lineColor,
        glm::vec3(0.0f, lineHeight/2.0f, 0.0f)
    );
    field->AddChild(centerLine);
    
    // Center circle
    float centerRadius = 4.5f;
    int circleSegments = 80; // High segment count for smooth circle
    for (int i = 0; i < circleSegments; ++i)
    {
        float angle1 = (float)i / circleSegments * 2.0f * 3.14159f;
        float angle2 = (float)(i + 1) / circleSegments * 2.0f * 3.14159f;
        
        float x1 = centerRadius * std::cos(angle1);
        float z1 = centerRadius * std::sin(angle1);
        float x2 = centerRadius * std::cos(angle2);
        float z2 = centerRadius * std::sin(angle2);
        
        float segLength = std::sqrt((x2-x1)*(x2-x1) + (z2-z1)*(z2-z1));
        float segAngle = std::atan2(z2-z1, x2-x1);
        
        auto segment = std::make_shared<MeshNode>(MeshType::Cube);
        segment->material.albedo = lineColor;
        
        glm::mat4 t(1.0f);
        t = glm::translate(t, glm::vec3((x1+x2)/2.0f, lineHeight/2.0f, (z1+z2)/2.0f));
        t = glm::rotate(t, segAngle, glm::vec3(0.0f, 1.0f, 0.0f));
        t = glm::scale(t, glm::vec3(segLength * 1.08f, lineHeight, lineThickness)); // Add overlap to close gaps
        segment->SetLocalTransform(t);
        
        field->AddChild(segment);
    }
    
    // Penalty areas (simplified - just the arc at each end)
    float penaltyArcRadius = 5.5f;
    int penaltySegments = 50; // Smoother arc
    
    for (int side = 0; side < 2; ++side)
    {
        float xCenter = (side == 0) ? -length/2.0f : length/2.0f;
        
        // Draw penalty arc (semicircle facing away from goal)
        for (int i = 0; i < penaltySegments/2; ++i)
        {
            float angle1 = -3.14159f/2.0f + (float)i / (penaltySegments/2) * 3.14159f;
            float angle2 = -3.14159f/2.0f + (float)(i + 1) / (penaltySegments/2) * 3.14159f;
            
            float z1 = penaltyArcRadius * std::sin(angle1);
            float z2 = penaltyArcRadius * std::sin(angle2);
            float x1 = penaltyArcRadius * std::cos(angle1) * (side == 0 ? 1.0f : -1.0f);
            float x2 = penaltyArcRadius * std::cos(angle2) * (side == 0 ? 1.0f : -1.0f);
            
            if (std::abs(z1) <= width/2.0f && std::abs(z2) <= width/2.0f)
            {
                float segLength = std::sqrt((x2-x1)*(x2-x1) + (z2-z1)*(z2-z1));
                float segAngle = std::atan2(z2-z1, x2-x1);
                
                auto segment = std::make_shared<MeshNode>(MeshType::Cube);
                segment->material.albedo = lineColor;
                
                glm::mat4 t(1.0f);
                t = glm::translate(t, glm::vec3(xCenter + (x1+x2)/2.0f, lineHeight/2.0f, (z1+z2)/2.0f));
                t = glm::rotate(t, segAngle, glm::vec3(0.0f, 1.0f, 0.0f));
                t = glm::scale(t, glm::vec3(segLength * 1.08f, lineHeight, lineThickness)); // Add overlap
                segment->SetLocalTransform(t);
                
                field->AddChild(segment);
            }
        }
    }
    
    // Goal posts (2 goals at each end)
    glm::vec3 goalColor(0.95f, 0.95f, 0.95f); // White
    glm::vec3 netColor(0.8f, 0.8f, 0.85f); // Light grey net
    
    float goalWidth = 7.32f;
    float goalHeight = 2.44f;
    float goalDepth = 2.0f;
    float postRadius = 0.12f;
    
    for (int side = 0; side < 2; ++side)
    {
        float xPos = (side == 0) ? -length/2.0f - 0.1f : length/2.0f + 0.1f;
        
        // Left post
        auto leftPost = createCuboid(
            glm::vec3(postRadius, goalHeight, postRadius),
            goalColor,
            glm::vec3(xPos, goalHeight/2.0f, -goalWidth/2.0f)
        );
        field->AddChild(leftPost);
        
        // Right post
        auto rightPost = createCuboid(
            glm::vec3(postRadius, goalHeight, postRadius),
            goalColor,
            glm::vec3(xPos, goalHeight/2.0f, goalWidth/2.0f)
        );
        field->AddChild(rightPost);
        
        // Crossbar
        auto crossbar = createCuboid(
            glm::vec3(postRadius, postRadius, goalWidth),
            goalColor,
            glm::vec3(xPos, goalHeight, 0.0f)
        );
        field->AddChild(crossbar);
        
        // Net (simplified as semi-transparent panels)
        // Back net
        auto backNet = createCuboid(
            glm::vec3(0.05f, goalHeight, goalWidth),
            netColor,
            glm::vec3(xPos + (side == 0 ? -goalDepth : goalDepth), goalHeight/2.0f, 0.0f)
        );
        field->AddChild(backNet);
        
        // Side nets
        auto leftNet = createCuboid(
            glm::vec3(goalDepth, goalHeight, 0.05f),
            netColor,
            glm::vec3(xPos + (side == 0 ? -goalDepth/2.0f : goalDepth/2.0f), goalHeight/2.0f, -goalWidth/2.0f)
        );
        field->AddChild(leftNet);
        
        auto rightNet = createCuboid(
            glm::vec3(goalDepth, goalHeight, 0.05f),
            netColor,
            glm::vec3(xPos + (side == 0 ? -goalDepth/2.0f : goalDepth/2.0f), goalHeight/2.0f, goalWidth/2.0f)
        );
        field->AddChild(rightNet);
        
        // Top net
        auto topNet = createCuboid(
            glm::vec3(goalDepth, 0.05f, goalWidth),
            netColor,
            glm::vec3(xPos + (side == 0 ? -goalDepth/2.0f : goalDepth/2.0f), goalHeight, 0.0f)
        );
        field->AddChild(topNet);
    }
    
    return field;
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
        // Perimeter wall is at Z=-5, with depth 70, so front is at Z = -5 + 70/2 = 30
        gate->SetLocalTransform(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 30.0f)));
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
        // Expanded to accommodate sports courts
        float perimeterWidth = 80.0f;  // Total width (X direction) - expanded for sports courts
        float perimeterDepth = 70.0f;  // Total depth (Z direction) - expanded for sports courts
        
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
    
    // -- Sports Courts --
    {
        // Basketball Court (positioned on the left side, oriented vertically along Z axis)
        auto basketballCourt = createBasketballCourt(20.0f, 12.0f); // Smaller court for school
        glm::mat4 bballTransform = glm::mat4(1.0f);
        bballTransform = glm::translate(bballTransform, glm::vec3(-28.0f, 0.0f, -18.0f));
        bballTransform = glm::rotate(bballTransform, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Rotate to be vertical
        basketballCourt->SetLocalTransform(bballTransform);
        schoolParams->AddChild(basketballCourt);
        
        // Football Field (positioned on the right side, oriented vertically along Z axis)
        auto footballField = createFootballField(30.0f, 20.0f); // Mini football field
        glm::mat4 footballTransform = glm::mat4(1.0f);
        footballTransform = glm::translate(footballTransform, glm::vec3(28.0f, 0.0f, -20.0f));
        footballTransform = glm::rotate(footballTransform, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Rotate to be vertical
        footballField->SetLocalTransform(footballTransform);
        schoolParams->AddChild(footballField);
    }
    
    // -- Flagpole (in front courtyard) --
    {
        auto flagpole = createFlagpole(10.0f); // 10m tall flagpole
        // Position in the courtyard, slightly to the left of the pathway
        flagpole->SetLocalTransform(glm::translate(glm::mat4(1.0f), glm::vec3(-6.0f, 0.0f, 12.0f)));
        schoolParams->AddChild(flagpole);
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
    
    // -- Stone Benches Along Pathways --
    {
        // Bench 1: Left pathway
        auto bench1 = createStoneBench();
        glm::mat4 t1 = glm::translate(glm::mat4(1.0f), glm::vec3(-8.0f, 0.0f, 8.0f));
        t1 = glm::rotate(t1, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Face pathway
        bench1->SetLocalTransform(t1);
        schoolParams->AddChild(bench1);
        
        // Bench 2: Right pathway
        auto bench2 = createStoneBench();
        glm::mat4 t2 = glm::translate(glm::mat4(1.0f), glm::vec3(8.0f, 0.0f, 8.0f));
        t2 = glm::rotate(t2, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Face pathway
        bench2->SetLocalTransform(t2);
        schoolParams->AddChild(bench2);
        
        // Bench 3: Near gate left
        auto bench3 = createStoneBench();
        glm::mat4 t3 = glm::translate(glm::mat4(1.0f), glm::vec3(-4.0f, 0.0f, 18.0f));
        t3 = glm::rotate(t3, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Face inward
        bench3->SetLocalTransform(t3);
        schoolParams->AddChild(bench3);
        
        // Bench 4: Near gate right
        auto bench4 = createStoneBench();
        glm::mat4 t4 = glm::translate(glm::mat4(1.0f), glm::vec3(4.0f, 0.0f, 18.0f));
        t4 = glm::rotate(t4, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Face inward
        bench4->SetLocalTransform(t4);
        schoolParams->AddChild(bench4);
    }
    
    // -- Picnic Tables Under Trees --
    {
        // Table 1: Left side (near basketball court)
        auto table1 = createPicnicTable();
        table1->SetLocalTransform(glm::translate(glm::mat4(1.0f), glm::vec3(-15.0f, 0.0f, 5.0f)));
        schoolParams->AddChild(table1);
        
        // Table 2: Right side (near football field)
        auto table2 = createPicnicTable();
        table2->SetLocalTransform(glm::translate(glm::mat4(1.0f), glm::vec3(15.0f, 0.0f, 5.0f)));
        schoolParams->AddChild(table2);
    }
    
    // -- Streetlights Around School --
    {
        float lightHeight = 4.0f;
        
        // Left side lights (outside basketball court)
        auto light3 = createStreetlight(lightHeight);
        light3->SetLocalTransform(glm::translate(glm::mat4(1.0f), glm::vec3(-32.0f, 0.0f, 0.0f)));
        schoolParams->AddChild(light3);
        
        auto light4 = createStreetlight(lightHeight);
        light4->SetLocalTransform(glm::translate(glm::mat4(1.0f), glm::vec3(-39.0f, 0.0f, -15.0f)));
        schoolParams->AddChild(light4);
        
        // Right side lights (outside football field)
        auto light5 = createStreetlight(lightHeight);
        light5->SetLocalTransform(glm::translate(glm::mat4(1.0f), glm::vec3(32.0f, 0.0f, 0.0f)));
        schoolParams->AddChild(light5);
        
        auto light6 = createStreetlight(lightHeight);
        light6->SetLocalTransform(glm::translate(glm::mat4(1.0f), glm::vec3(39.0f, 0.0f, -15.0f)));
        schoolParams->AddChild(light6);
        
        // Back area lights (behind building)
        auto light7 = createStreetlight(lightHeight);
        light7->SetLocalTransform(glm::translate(glm::mat4(1.0f), glm::vec3(-15.0f, 0.0f, -20.0f)));
        schoolParams->AddChild(light7);
        
        auto light8 = createStreetlight(lightHeight);
        light8->SetLocalTransform(glm::translate(glm::mat4(1.0f), glm::vec3(15.0f, 0.0f, -20.0f)));
        schoolParams->AddChild(light8);
    }
    
    // -- Wall Clock (mounted on center building) --
    {
        auto clock = createClock();
        // Mount on front wall of center building
        // Center building is at Z around -10, front face at Z=-5
        // Mount at 3.5m height on the wall
        glm::mat4 clockTransform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 6.0f, -6.95f));
        // No rotation needed - clock faces forward naturally
        clock->SetLocalTransform(clockTransform);
        schoolParams->AddChild(clock);
        // Store clock reference for animation
        s_clock = clock;
    }
    
    // -- People Walking Around --
    {
        // Clear previous people (if any)
        s_people.clear();
        
        // Person 1: Walking on left pathway (blue shirt)
        auto person1 = createPerson(glm::vec3(0.2f, 0.4f, 0.8f)); // Blue
        glm::mat4 t1 = glm::translate(glm::mat4(1.0f), glm::vec3(-5.0f, 0.0f, 10.0f));
        t1 = glm::rotate(t1, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        person1->SetLocalTransform(t1);
        schoolParams->AddChild(person1);
        s_people.push_back(person1);
        
        // Person 2: Walking on right pathway (red shirt)
        auto person2 = createPerson(glm::vec3(0.8f, 0.2f, 0.2f)); // Red
        glm::mat4 t2 = glm::translate(glm::mat4(1.0f), glm::vec3(6.0f, 0.0f, 12.0f));
        t2 = glm::rotate(t2, glm::radians(-120.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        person2->SetLocalTransform(t2);
        schoolParams->AddChild(person2);
        s_people.push_back(person2);
        
        // Person 3: Near basketball court (green shirt)
        auto person3 = createPerson(glm::vec3(0.2f, 0.7f, 0.3f)); // Green
        glm::mat4 t3 = glm::translate(glm::mat4(1.0f), glm::vec3(-20.0f, 0.0f, -10.0f));
        t3 = glm::rotate(t3, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        person3->SetLocalTransform(t3);
        schoolParams->AddChild(person3);
        s_people.push_back(person3);
        
        // Person 4: Near football field (yellow shirt)
        auto person4 = createPerson(glm::vec3(0.9f, 0.8f, 0.2f)); // Yellow
        glm::mat4 t4 = glm::translate(glm::mat4(1.0f), glm::vec3(22.0f, 0.0f, -8.0f));
        t4 = glm::rotate(t4, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        person4->SetLocalTransform(t4);
        schoolParams->AddChild(person4);
        s_people.push_back(person4);
        
        // Person 5: Sitting at picnic table (purple shirt)
        auto person5 = createPerson(glm::vec3(0.6f, 0.3f, 0.7f)); // Purple
        glm::mat4 t5 = glm::translate(glm::mat4(1.0f), glm::vec3(-15.0f, 0.0f, 5.8f));
        t5 = glm::rotate(t5, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        person5->SetLocalTransform(t5);
        schoolParams->AddChild(person5);
        s_people.push_back(person5);
        
        // Person 6: Near entrance (orange shirt)
        auto person6 = createPerson(glm::vec3(0.9f, 0.5f, 0.1f)); // Orange
        glm::mat4 t6 = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, 0.0f, 20.0f));
        t6 = glm::rotate(t6, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        person6->SetLocalTransform(t6);
        schoolParams->AddChild(person6);
        s_people.push_back(person6);
    }
    
    // -- Clouds in the Sky --
    {
        // Cloud 1 (very high, very large) - Directly overhead
        auto cloud1 = createCloud(12.0f);
        cloud1->SetLocalTransform(glm::translate(glm::mat4(1.0f), glm::vec3(-50.0f, 45.0f, 10.0f)));
        schoolParams->AddChild(cloud1);
        s_clouds.push_back(cloud1);
        
        // Cloud 2 (high, large) - Overhead right
        auto cloud2 = createCloud(9.6f);
        cloud2->SetLocalTransform(glm::translate(glm::mat4(1.0f), glm::vec3(40.0f, 38.0f, 5.0f)));
        schoolParams->AddChild(cloud2);
        s_clouds.push_back(cloud2);
        
        // Cloud 3 (very high, medium) - Overhead left
        auto cloud3 = createCloud(6.0f);
        cloud3->SetLocalTransform(glm::translate(glm::mat4(1.0f), glm::vec3(-60.0f, 50.0f, -5.0f)));
        schoolParams->AddChild(cloud3);
        s_clouds.push_back(cloud3);
        
        // Cloud 4 (high, very large) - Far right, back
        auto cloud4 = createCloud(11.4f);
        cloud4->SetLocalTransform(glm::translate(glm::mat4(1.0f), glm::vec3(55.0f, 42.0f, -40.0f)));
        schoolParams->AddChild(cloud4);
        s_clouds.push_back(cloud4);
        
        // Cloud 5 (medium high, large) - Center overhead
        auto cloud5 = createCloud(8.4f);
        cloud5->SetLocalTransform(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 35.0f, 15.0f)));
        schoolParams->AddChild(cloud5);
        s_clouds.push_back(cloud5);
        
        // Cloud 6 (high, large) - Left center, slightly back
        auto cloud6 = createCloud(9.0f);
        cloud6->SetLocalTransform(glm::translate(glm::mat4(1.0f), glm::vec3(-30.0f, 40.0f, -20.0f)));
        schoolParams->AddChild(cloud6);
        s_clouds.push_back(cloud6);
        
        // Cloud 7 (very high, medium) - Far right, very far
        auto cloud7 = createCloud(5.4f);
        cloud7->SetLocalTransform(glm::translate(glm::mat4(1.0f), glm::vec3(65.0f, 48.0f, -80.0f)));
        schoolParams->AddChild(cloud7);
        s_clouds.push_back(cloud7);
        
        // Cloud 8 (medium high, medium) - Far left, overhead
        auto cloud8 = createCloud(6.6f);
        cloud8->SetLocalTransform(glm::translate(glm::mat4(1.0f), glm::vec3(-70.0f, 33.0f, 0.0f)));
        schoolParams->AddChild(cloud8);
        s_clouds.push_back(cloud8);
        
        // Cloud 9 (high, very large) - Right center, back
        auto cloud9 = createCloud(10.5f);
        cloud9->SetLocalTransform(glm::translate(glm::mat4(1.0f), glm::vec3(25.0f, 44.0f, -30.0f)));
        schoolParams->AddChild(cloud9);
        s_clouds.push_back(cloud9);
        
        // Cloud 10 (very high, small) - Center left, far
        auto cloud10 = createCloud(4.5f);
        cloud10->SetLocalTransform(glm::translate(glm::mat4(1.0f), glm::vec3(-15.0f, 52.0f, -60.0f)));
        schoolParams->AddChild(cloud10);
        s_clouds.push_back(cloud10);
        
        // Cloud 11 (medium high, very large) - Right, overhead
        auto cloud11 = createCloud(10.8f);
        cloud11->SetLocalTransform(glm::translate(glm::mat4(1.0f), glm::vec3(45.0f, 36.0f, 8.0f)));
        schoolParams->AddChild(cloud11);
        s_clouds.push_back(cloud11);
        
        // Cloud 12 (high, large) - Left, back
        auto cloud12 = createCloud(7.5f);
        cloud12->SetLocalTransform(glm::translate(glm::mat4(1.0f), glm::vec3(-45.0f, 46.0f, -50.0f)));
        schoolParams->AddChild(cloud12);
        s_clouds.push_back(cloud12);
    }
    
    // -- Birds in the Sky --
    {
        // Flock 1 (Left side)
        for(int i=0; i<3; ++i) {
             auto bird = createBird(0.8f);
             float offsetX = i * 2.0f;
             float offsetZ = i * 1.5f;
             bird->SetLocalTransform(glm::translate(glm::mat4(1.0f), glm::vec3(-20.0f + offsetX, 30.0f, -10.0f - offsetZ)));
             schoolParams->AddChild(bird);
             s_birds.push_back(bird);
        }
        
        // Flock 2 (Right side)
        for(int i=0; i<4; ++i) {
             auto bird = createBird(0.7f);
             float offsetX = i * 2.5f;
             float offsetY = (i%2) * 1.0f;
             bird->SetLocalTransform(glm::translate(glm::mat4(1.0f), glm::vec3(15.0f + offsetX, 35.0f + offsetY, -20.0f)));
             schoolParams->AddChild(bird);
             s_birds.push_back(bird);
        }
    }

    return root;
}

// Static vector to store people for animation
std::vector<SceneNode::Ptr> SchoolBuilder::s_people;

// Static clock for animation
SceneNode::Ptr SchoolBuilder::s_clock;

// Static clouds for animation
std::vector<SceneNode::Ptr> SchoolBuilder::s_clouds;

// Update people animation
void SchoolBuilder::updatePeopleAnimation(SceneNode::Ptr root, float time)
{
    if (s_people.empty()) return;
    
    // Helper to animate limbs (arms and legs swing)
    auto animateLimbs = [](SceneNode::Ptr person, float walkCycle) {
        if (person->children.size() < 7) return; // Need all limbs
        
        // Walking cycle animation
        float armSwing = std::sin(walkCycle * 2.0f) * 30.0f; // Arms swing opposite to legs
        float legSwing = std::sin(walkCycle * 2.0f) * 25.0f; // Legs swing
        
        // Get limb nodes: 3=leftArm, 4=rightArm, 5=leftLeg, 6=rightLeg
        auto leftArm = person->children[3];
        auto rightArm = person->children[4];
        auto leftLeg = person->children[5];
        auto rightLeg = person->children[6];
        
        // Animate arms (swing forward/back around X axis)
        if (leftArm) {
            glm::mat4 t = leftArm->GetLocalTransform();
            // Extract translation
            glm::vec3 pos = glm::vec3(t[3]);
            // Apply rotation around shoulder
            t = glm::translate(glm::mat4(1.0f), pos);
            t = glm::rotate(t, glm::radians(armSwing), glm::vec3(1.0f, 0.0f, 0.0f));
            leftArm->SetLocalTransform(t);
        }
        
        if (rightArm) {
            glm::mat4 t = rightArm->GetLocalTransform();
            glm::vec3 pos = glm::vec3(t[3]);
            t = glm::translate(glm::mat4(1.0f), pos);
            t = glm::rotate(t, glm::radians(-armSwing), glm::vec3(1.0f, 0.0f, 0.0f));
            rightArm->SetLocalTransform(t);
        }
        
        // Animate legs (swing forward/back around X axis)
        if (leftLeg) {
            glm::mat4 t = leftLeg->GetLocalTransform();
            glm::vec3 pos = glm::vec3(t[3]);
            t = glm::translate(glm::mat4(1.0f), pos);
            t = glm::rotate(t, glm::radians(-legSwing), glm::vec3(1.0f, 0.0f, 0.0f));
            leftLeg->SetLocalTransform(t);
        }
        
        if (rightLeg) {
            glm::mat4 t = rightLeg->GetLocalTransform();
            glm::vec3 pos = glm::vec3(t[3]);
            t = glm::translate(glm::mat4(1.0f), pos);
            t = glm::rotate(t, glm::radians(legSwing), glm::vec3(1.0f, 0.0f, 0.0f));
            rightLeg->SetLocalTransform(t);
        }
    };
    
    // Person 1: Walk back and forth on left pathway (X direction)
    if (s_people.size() > 0)
    {
        float walkSpeed = 0.5f;
        float x = -5.0f + std::sin(time * walkSpeed) * 3.0f;
        float angle = (std::sin(time * walkSpeed) > 0) ? 45.0f : -135.0f;
        glm::mat4 t = glm::translate(glm::mat4(1.0f), glm::vec3(x, 0.0f, 10.0f));
        t = glm::rotate(t, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
        s_people[0]->SetLocalTransform(t);
        animateLimbs(s_people[0], time * walkSpeed);
    }
    
    // Person 2: Walk back and forth on right pathway (Z direction)
    if (s_people.size() > 1)
    {
        float walkSpeed = 0.6f;
        float z = 12.0f + std::sin(time * walkSpeed) * 4.0f;
        float angle = (std::sin(time * walkSpeed) > 0) ? 0.0f : 180.0f;
        glm::mat4 t = glm::translate(glm::mat4(1.0f), glm::vec3(6.0f, 0.0f, z));
        t = glm::rotate(t, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
        s_people[1]->SetLocalTransform(t);
        animateLimbs(s_people[1], time * walkSpeed);
    }
    
    // Person 3: Walk around basketball court (circular path)
    if (s_people.size() > 2)
    {
        float walkSpeed = 0.4f;
        float radius = 3.0f;
        float x = -20.0f + std::cos(time * walkSpeed) * radius;
        float z = -10.0f + std::sin(time * walkSpeed) * radius;
        float angle = time * walkSpeed * 180.0f / 3.14159f + 90.0f;
        glm::mat4 t = glm::translate(glm::mat4(1.0f), glm::vec3(x, 0.0f, z));
        t = glm::rotate(t, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
        s_people[2]->SetLocalTransform(t);
        animateLimbs(s_people[2], time * walkSpeed);
    }
    
    // Person 4: Walk back and forth near football field
    if (s_people.size() > 3)
    {
        float walkSpeed = 0.55f;
        float z = -8.0f + std::sin(time * walkSpeed) * 5.0f;
        float angle = (std::sin(time * walkSpeed) > 0) ? 0.0f : 180.0f;
        glm::mat4 t = glm::translate(glm::mat4(1.0f), glm::vec3(22.0f, 0.0f, z));
        t = glm::rotate(t, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
        s_people[3]->SetLocalTransform(t);
        animateLimbs(s_people[3], time * walkSpeed);
    }
    
    // Person 5: Stay at picnic table (no movement, no animation)
    
    // Person 6: Walk near entrance (X direction)
    if (s_people.size() > 5)
    {
        float walkSpeed = 0.45f;
        float x = 2.0f + std::sin(time * walkSpeed) * 4.0f;
        float angle = (std::sin(time * walkSpeed) > 0) ? 90.0f : -90.0f;
        glm::mat4 t = glm::translate(glm::mat4(1.0f), glm::vec3(x, 0.0f, 20.0f));
        t = glm::rotate(t, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
        s_people[5]->SetLocalTransform(t);
        animateLimbs(s_people[5], time * walkSpeed);
    }
}

// Update clock animation
void SchoolBuilder::updateClockAnimation(SceneNode::Ptr root, float time)
{
    if (!s_clock) return;
    
    // Clock hands are the last 2 children: hourHand, minuteHand (no second hand)
    size_t numChildren = s_clock->children.size();
    if (numChildren < 2) return;
    
    auto hourHand = s_clock->children[numChildren - 2];
    auto minuteHand = s_clock->children[numChildren - 1];
    
    // Calculate angles based on time
    // Speed up time for demonstration (1 real second = 60 game seconds)
    float gameTime = time * 60.0f;
    
    float minutesAngle = (gameTime / 60.0f) * 6.0f; // 360° / 60 minutes = 6° per minute
    float hoursAngle = (gameTime / 3600.0f) * 30.0f; // 360° / 12 hours = 30° per hour
    
    // Update hour hand
    if (hourHand) {
        glm::mat4 t = hourHand->GetLocalTransform();
        glm::vec3 pos = glm::vec3(t[3]);
        t = glm::translate(glm::mat4(1.0f), pos);
        t = glm::rotate(t, glm::radians(-hoursAngle), glm::vec3(0.0f, 0.0f, 1.0f)); // Rotate around Z axis
        hourHand->SetLocalTransform(t);
    }
    
    // Update minute hand
    if (minuteHand) {
        glm::mat4 t = minuteHand->GetLocalTransform();
        glm::vec3 pos = glm::vec3(t[3]);
        t = glm::translate(glm::mat4(1.0f), pos);
        t = glm::rotate(t, glm::radians(-minutesAngle), glm::vec3(0.0f, 0.0f, 1.0f));
        minuteHand->SetLocalTransform(t);
    }
}

// Update cloud animation
void SchoolBuilder::updateCloudAnimation(SceneNode::Ptr root, float time)
{
    if (s_clouds.empty()) return;
    
    // Each cloud drifts slowly across the sky at different speeds
    float cloudSpeeds[] = {0.5f, 0.3f, 0.4f, 0.6f, 0.35f, 0.45f, 0.55f, 0.38f, 0.42f, 0.32f, 0.48f, 0.52f};
    float cloudRange = 80.0f; // How far clouds travel (increased for larger clouds)
    
    // Base X positions for each cloud (evenly distributed)
    float basePositions[] = {-50.0f, 40.0f, -60.0f, 55.0f, 0.0f, -30.0f, 65.0f, -70.0f, 25.0f, -15.0f, 45.0f, -45.0f};
    
    for (size_t i = 0; i < s_clouds.size() && i < 12; ++i)
    {
        // Get current transform
        glm::mat4 t = s_clouds[i]->GetLocalTransform();
        glm::vec3 pos = glm::vec3(t[3]);
        
        // Calculate new X position (drift from left to right)
        float offset = std::sin(time * cloudSpeeds[i] * 0.1f) * cloudRange;
        
        // Update position (keep original Y and Z, only change X)
        pos.x = basePositions[i] + offset;
        
        // Set new transform
        t = glm::translate(glm::mat4(1.0f), pos);
        s_clouds[i]->SetLocalTransform(t);
    }
}

// Static birds for animation
std::vector<SceneNode::Ptr> SchoolBuilder::s_birds;

// Update bird animation
void SchoolBuilder::updateBirdAnimation(SceneNode::Ptr root, float time)
{
    if (s_birds.empty()) return;
    
    float birdSpeed = 2.0f;
    float circleRadius = 5.0f;
    
    for (size_t i = 0; i < s_birds.size(); ++i)
    {
        glm::mat4 t = s_birds[i]->GetLocalTransform();
        glm::vec3 pos = glm::vec3(t[3]);
        
        // Simple circling or bobbing movement
        float offsetX = std::cos(time * 0.5f + i) * 0.05f; 
        float offsetY = std::sin(time * 1.0f + i) * 0.05f;
        float offsetZ = std::sin(time * 0.3f + i) * 0.05f; // Slight forward/back
        
        pos.x += offsetX;
        pos.y += offsetY;
        pos.z += offsetZ;
        
        // Also rotate them slightly to face movement direction if we wanted complex physics, 
        // but for ambient birds, small bobbing is enough to look alive.
        
        // Apply transform
        t = glm::translate(glm::mat4(1.0f), pos);
        
        // Add a little flap rotation (wobble)
        float flap = std::sin(time * 10.0f + i) * 5.0f; // Rapid flap
        t = glm::rotate(t, glm::radians(flap), glm::vec3(0.0f, 0.0f, 1.0f)); // Banking
        
        // Keep original orientation somewhat? We just rebuilt T from identity, lost original rotation.
        // It's better to update the existing matrix if we want to separate position update.
        // But here we just set position. Re-applying basic rotation:
        // Birds face generally forward (-Z or +X depending on flock)
        // Let's just make them face consistent direction for now or reuse existing rotation from previous frame?
        // Re-using existing rotation is hard if we rebuild matrix.
        // Let's just drift them. The 'createBird' adds wing rotation relative to body.
        
        // Simplified: just update position in existing matrix?
        // SceneNode transform is local.
        // Let's just create new matrix with current pos + slight offset.
        
         // Reset rotational components for banking effect 
         // Assume birds face generally -Z (default) or we need to know their heading.
         // Let's just translate the EXISTING matrix instead of rebuilding.
         
         // Actually, simple way: modify the position columns of the matrix directly.
         // t[3][0] += offsetX; ...
         // But SceneNode encapsulation prefers SetLocalTransform.
         
         // Let's stick to the subtle drift added to current position.
         // But we need to avoid "walking" away infinitely.
         // We calculated offset from TIME, so we need Base Position.
         // Like clouds, we need original positions.
         // For simplicity: just bob in place relative to where they current are? 
         // No, time-based offset from *initial* position is best.
         // But we don't store initial positions easily here without a struct.
         
         // Alternative: small delta movement each frame?
         // pos += delta. 
         // Yes, pos.x += offsetX is delta if offsetX is small? No, offsetX is sine wave result.
         // We are adding sine result to current pos? That would accelerate back and forth!
         
         // Correct way without storing state:
         // Just use sine wave for visual fluff, don't accumulate.
         // But we need base position.
         // Let's assume the previous position was close to base.
         // Or just define trajectories like clouds.
         
         // Let's do a simple circular path for all birds around a center point?
         // Or linear flight.
         
         // Let's implement simple LINEAR flight with wrap-around.
         
         float speed = 2.5f;
         pos.z -= speed * 0.016f; // Fly forward (-Z)
         pos.y += std::sin(time * 3.0f + i) * 0.02f; // Bob up down
         
         // Reset if too far
         if (pos.z < -60.0f) pos.z = 20.0f;
         
         // Rebuild matrix to maintain orientation
         t = glm::translate(glm::mat4(1.0f), pos);
         // Rotate 180 to face camera if moving -Z matches face? 
         // Actually createBird makes them face +Z? No, usually +Z is "back".
         // Let's assume they face -Z (standard).
         
         // Add banking/flapping
         float bank = std::sin(time * 5.0f + i) * 10.0f;
         t = glm::rotate(t, glm::radians(bank), glm::vec3(0.0f, 0.0f, 1.0f));
         
         s_birds[i]->SetLocalTransform(t);
    }
}
