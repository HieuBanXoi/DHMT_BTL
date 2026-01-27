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

// Helper to create a simple car
static SceneNode::Ptr createCar(glm::vec3 color) {
    auto carNode = std::make_shared<SceneNode>();
    
    // Chassis (Body)
    auto chassis = createCuboid(glm::vec3(4.5f, 1.0f, 2.0f), color, glm::vec3(0.0f, 0.7f, 0.0f));
    carNode->AddChild(chassis);
    
    // Cabin (Top)
    auto cabin = createCuboid(glm::vec3(2.5f, 0.8f, 1.8f), color * 1.2f, glm::vec3(-0.5f, 1.6f, 0.0f));
    carNode->AddChild(cabin);
    
    // Windows (Black)
    auto windowL = createCuboid(glm::vec3(1.5f, 0.5f, 1.85f), glm::vec3(0.1f), glm::vec3(-0.5f, 1.65f, 0.0f));
    carNode->AddChild(windowL);
    
    // Wheels (Indices 3, 4, 5, 6)
    glm::vec3 wheelColor(0.1f, 0.1f, 0.1f);
    
    // Note: Cylinder is Y-axis aligned, Radius 0.5, Height 1.0.
    // We want Radius 0.4 (Scale 0.8), Width 0.4 (Scale 0.4).
    // And oriented so the circular face is in the YZ plane? No, XZ plane?
    // Wheels roll along X axis. The axle is along Z axis.
    // So Cylinder Y-axis should map to World Z-axis.
    // Rotate 90 deg around X axis.
    
    auto createWheel = [&](float x, float z) {
        auto wheel = std::make_shared<MeshNode>(MeshType::Cylinder); // Cylinder Mesh
        wheel->material.albedo = wheelColor;
        
        // Initial Transform (No rotation yet, just placement and orientation)
        glm::mat4 t(1.0f);
        t = glm::translate(t, glm::vec3(x, 0.4f, z)); // Center of wheel
        // Rotate cylinder to align with Z axis (Axle)
        t = glm::rotate(t, glm::radians(90.0f), glm::vec3(1, 0, 0)); 
        // Scale: X=Radius*2, Y=Width, Z=Radius*2 (Relative to rotated frame?)
        // Cylinder local: Radius in XZ, Height in Y.
        // After RotX(90): Local Y is World Z. Local X is World X. Local Z is World -Y.
        // We want World X/Y to be Radius dimensions. World Z to be Width.
        // So Scale Local X (Radius), Local Y (Height/Width), Local Z (Radius).
        // Radius 0.4 -> Scale 0.8 (since base is 0.5).
        // Width 0.4 -> Scale 0.4 (since height is 1.0).
        t = glm::scale(t, glm::vec3(0.8f, 0.4f, 0.8f)); 
        
        wheel->SetLocalTransform(t);
        carNode->AddChild(wheel);
    };
    
    createWheel(1.5f, 1.0f);  // Front Left (Index 3)
    createWheel(1.5f, -1.0f); // Front Right (Index 4)
    createWheel(-1.5f, 1.0f); // Back Left (Index 5)
    createWheel(-1.5f, -1.0f);// Back Right (Index 6)
    
    // Headlights (Yellow)
    auto headlightL = createCuboid(glm::vec3(0.1f, 0.3f, 0.5f), glm::vec3(1.0f, 1.0f, 0.5f), glm::vec3(2.25f, 0.8f, 0.6f));
    carNode->AddChild(headlightL);
    auto headlightR = createCuboid(glm::vec3(0.1f, 0.3f, 0.5f), glm::vec3(1.0f, 1.0f, 0.5f), glm::vec3(2.25f, 0.8f, -0.6f));
    carNode->AddChild(headlightR);

    // Taillights (Red)
    auto taillightL = createCuboid(glm::vec3(0.1f, 0.3f, 0.5f), glm::vec3(0.8f, 0.0f, 0.0f), glm::vec3(-2.25f, 0.8f, 0.6f));
    carNode->AddChild(taillightL);
    auto taillightR = createCuboid(glm::vec3(0.1f, 0.3f, 0.5f), glm::vec3(0.8f, 0.0f, 0.0f), glm::vec3(-2.25f, 0.8f, -0.6f));
    carNode->AddChild(taillightR);
    
    return carNode;
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
static SceneNode::Ptr createDoor(float width, float height, float openAngle = 90.0f)
{
    // Root Node (Fixed position in wall)
    auto doorRoot = std::make_shared<SceneNode>();
    
    // Hinge Node (Rotates)
    auto doorHinge = std::make_shared<SceneNode>();
    doorRoot->AddChild(doorHinge);
    
    glm::vec3 doorColor(0.4f, 0.25f, 0.1f); // Wood color
    glm::vec3 knobColor(0.8f, 0.7f, 0.2f);  // Brass
    
    // Door Leaf (Offset so Hinge is at edge)
    // Center of leaf is at (width/2, height/2, 0) relative to hinge
    auto door = createCuboid(glm::vec3(width, height, 0.1f), doorColor, glm::vec3(width/2.0f, height/2.0f, 0.0f));
    doorHinge->AddChild(door);
    
    // Knob (Place knob on opposite side of hinge)
    // If openAngle is positive (Standard, hinge Left), knob is on Right (width * 0.85)
    // If openAngle is negative (Hinge Right/Rotated?), logic depends on usage.
    // Standard usage: Hinge is always at (0,0,0) of local space.
    // So knob is always at width * 0.85.
    auto knob = createCuboid(glm::vec3(0.1f, 0.1f, 0.15f), knobColor, glm::vec3(width * 0.85f, height * 0.5f, 0.0f));
    doorHinge->AddChild(knob);
    
    // Register the HINGE node
    SchoolBuilder::Door newDoor;
    newDoor.node = doorHinge;
    newDoor.currentAngle = 0.0f;
    newDoor.targetAngle = 0.0f;
    newDoor.openAngle = openAngle; // Store custom angle
    newDoor.isOpen = false;
    newDoor.isMoving = false;
    SchoolBuilder::s_doors.push_back(newDoor);
    
    return doorRoot;
}

// Implement updateDoorAnimation
void SchoolBuilder::updateDoorAnimation(float dt)
{
    float speed = 120.0f; 
    for (auto& door : s_doors) {
        if (std::abs(door.currentAngle - door.targetAngle) > 0.1f) {
            if (door.currentAngle < door.targetAngle) {
                door.currentAngle += speed * dt;
                if (door.currentAngle > door.targetAngle) door.currentAngle = door.targetAngle;
            } else {
                door.currentAngle -= speed * dt;
                if (door.currentAngle < door.targetAngle) door.currentAngle = door.targetAngle;
            }
            door.node->SetLocalTransform(glm::rotate(glm::mat4(1.0f), glm::radians(door.currentAngle), glm::vec3(0.0f, 1.0f, 0.0f)));
        }
    }
}

void SchoolBuilder::toggleDoor(const glm::vec3& playerPos, float maxDistance, const glm::vec3& forward)
{
    // Find nearest door
    float minDist = maxDistance;
    Door* nearest = nullptr;
    
    for (auto& door : s_doors) {
        // Need global position. 
        glm::vec3 doorPos = glm::vec3(door.node->GetGlobalTransform()[3]);
        float dist = glm::distance(playerPos, doorPos);
        if (dist < minDist) {
            minDist = dist;
            nearest = &door;
        }
    }
    
    if (nearest) {
        nearest->isOpen = !nearest->isOpen;
        nearest->targetAngle = nearest->isOpen ? 90.0f : 0.0f;
    }
}

// --- FURNITURE HELPERS ---

static SceneNode::Ptr createTable(float width, float depth, float height)
{
    auto table = std::make_shared<SceneNode>();
    glm::vec3 woodColor(0.6f, 0.4f, 0.2f);
    
    // Tabletop
    auto top = createCuboid(glm::vec3(width, 0.05f, depth), woodColor, glm::vec3(0.0f, height - 0.025f, 0.0f));
    table->AddChild(top);
    
    // Legs
    float legW = 0.05f;
    glm::vec3 legPos(width/2.0f - legW/2.0f, height/2.0f, depth/2.0f - legW/2.0f);
    
    table->AddChild(createCuboid(glm::vec3(legW, height, legW), woodColor, glm::vec3(legPos.x, height/2.0f, legPos.z)));
    table->AddChild(createCuboid(glm::vec3(legW, height, legW), woodColor, glm::vec3(-legPos.x, height/2.0f, legPos.z)));
    table->AddChild(createCuboid(glm::vec3(legW, height, legW), woodColor, glm::vec3(legPos.x, height/2.0f, -legPos.z)));
    table->AddChild(createCuboid(glm::vec3(legW, height, legW), woodColor, glm::vec3(-legPos.x, height/2.0f, -legPos.z)));
    
    return table;
}

static SceneNode::Ptr createChair(float size)
{
    auto chair = std::make_shared<SceneNode>();
    glm::vec3 woodColor(0.5f, 0.35f, 0.15f);
    float seatH = 0.45f;
    
    // Seat
    chair->AddChild(createCuboid(glm::vec3(size, 0.05f, size), woodColor, glm::vec3(0.0f, seatH, 0.0f)));
    
    // Backrest
    chair->AddChild(createCuboid(glm::vec3(size, size, 0.05f), woodColor, glm::vec3(0.0f, seatH + size/2.0f, -size/2.0f + 0.025f)));
    
    // Legs
    float legW = 0.04f;
    float legH = seatH;
    glm::vec3 legPos(size/2.0f - legW/2.0f, 0.0f, size/2.0f - legW/2.0f);
    
    chair->AddChild(createCuboid(glm::vec3(legW, legH, legW), woodColor, glm::vec3(legPos.x, legH/2.0f, legPos.z)));
    chair->AddChild(createCuboid(glm::vec3(legW, legH, legW), woodColor, glm::vec3(-legPos.x, legH/2.0f, legPos.z)));
    chair->AddChild(createCuboid(glm::vec3(legW, legH, legW), woodColor, glm::vec3(legPos.x, legH/2.0f, -legPos.z)));
    chair->AddChild(createCuboid(glm::vec3(legW, legH, legW), woodColor, glm::vec3(-legPos.x, legH/2.0f, -legPos.z)));
    
    return chair;
}

static SceneNode::Ptr createBlackboard(float width, float height)
{
    auto boardGroup = std::make_shared<SceneNode>();
    
    // Board Node (Frame + Surface)
    auto board = std::make_shared<SceneNode>();
    
    // Frame
    glm::vec3 frameColor(0.3f, 0.2f, 0.1f);
    float frameT = 0.05f;
    board->AddChild(createCuboid(glm::vec3(width, height, 0.05f), frameColor, glm::vec3(0.0f, 0.0f, 0.0f)));
    
    // Surface (Green)
    board->AddChild(createCuboid(glm::vec3(width - 2*frameT, height - 2*frameT, 0.06f), glm::vec3(0.1f, 0.4f, 0.2f), glm::vec3(0.0f, 0.0f, 0.01f)));
    
    // Raise board to standing height
    float standHeight = 1.0f; // Height from floor to bottom of board
    float totalH = standHeight + height/2.0f;
    board->SetLocalTransform(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, totalH, 0.0f)));
    boardGroup->AddChild(board);
    
    // --- STAND / LEGS ---
    glm::vec3 metalColor(0.2f, 0.2f, 0.2f);
    float legW = 0.05f;
    
    // Left Leg
    boardGroup->AddChild(createCuboid(
        glm::vec3(legW, totalH + height/2.0f, 0.1f), 
        metalColor, 
        glm::vec3(-width/2.0f, (totalH + height/2.0f)/2.0f, 0.0f)
    ));
    
    // Right Leg
    boardGroup->AddChild(createCuboid(
        glm::vec3(legW, totalH + height/2.0f, 0.1f), 
        metalColor, 
        glm::vec3(width/2.0f, (totalH + height/2.0f)/2.0f, 0.0f)
    ));
    
    // Feet
    float footLen = 0.6f;
    boardGroup->AddChild(createCuboid(glm::vec3(legW, 0.05f, footLen), metalColor, glm::vec3(-width/2.0f, 0.025f, 0.0f)));
    boardGroup->AddChild(createCuboid(glm::vec3(legW, 0.05f, footLen), metalColor, glm::vec3(width/2.0f, 0.025f, 0.0f)));
    
    // Crossbar
    boardGroup->AddChild(createCuboid(glm::vec3(width, 0.05f, 0.05f), metalColor, glm::vec3(0.0f, standHeight/2.0f, 0.0f)));
    
    return boardGroup;
}

static SceneNode::Ptr createPodium()
{
    auto podium = std::make_shared<SceneNode>();
    glm::vec3 woodColor(0.55f, 0.35f, 0.2f);
    
    // Base Box
    podium->AddChild(createCuboid(glm::vec3(1.2f, 1.1f, 0.6f), woodColor, glm::vec3(0.0f, 0.55f, 0.0f)));
    
    return podium;
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

// Forward declare
static SceneNode::Ptr createStaircase(float height, float width, float depth, int numSteps);

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
                                  int doorFloor = 3, // 1=floor1 only, 2=floor2 only, 3=both floors
                                  int maskStart = 1, // Number of slots to mask (no window) from start
                                  int maskEnd = 1)   // Number of slots to mask (no window) from end
{
    auto wing = std::make_shared<SceneNode>();
    
    // Materials
    glm::vec3 wallColor(0.9f, 0.85f, 0.8f); // Cream/White wall
    glm::vec3 floorColor(0.6f, 0.6f, 0.65f); // Concrete floor
    glm::vec3 roofColor(0.7f, 0.3f, 0.3f); // Red roof
    glm::vec3 pillarColor(0.85f, 0.8f, 0.75f); // Slightly darker for pillars
    
    // Dimensions
    float wallThick = 0.2f;
    float floorThick = 0.2f;
    
    // --- 1. BUILD THE SHELL (Walls, Floors, Ceiling) ---
    
    // Back Wall (Solid)
    auto backWall = createCuboid(
        glm::vec3(w, h, wallThick),
        wallColor,
        glm::vec3(0.0f, h/2.0f, -d/2.0f + wallThick/2.0f)
    );
    wing->AddChild(backWall);
    
    // Left Wall (Solid side)
    auto leftWall = createCuboid(
        glm::vec3(wallThick, h, d),
        wallColor,
        glm::vec3(-w/2.0f + wallThick/2.0f, h/2.0f, 0.0f)
    );
    wing->AddChild(leftWall);
    
    // Right Wall (Solid side)
    auto rightWall = createCuboid(
        glm::vec3(wallThick, h, d),
        wallColor,
        glm::vec3(w/2.0f - wallThick/2.0f, h/2.0f, 0.0f)
    );
    wing->AddChild(rightWall);
    
    // Floor 1 (Ground)
    auto floor1 = createCuboid(
        glm::vec3(w, floorThick, d),
        floorColor,
        glm::vec3(0.0f, floorThick/2.0f, 0.0f)
    );
    wing->AddChild(floor1);
    
    // Ceiling / Roof Base
    auto ceiling = createCuboid(
        glm::vec3(w, floorThick, d),
        wallColor,
        glm::vec3(0.0f, h - floorThick/2.0f, 0.0f)
    );
    wing->AddChild(ceiling);
    
    // Slanted Roof Top
    float roofH = 0.5f;
    float overhang = 0.4f;
    auto roof = createCuboid(
        glm::vec3(w + overhang, roofH, d + overhang), 
        roofColor, 
        glm::vec3(0.0f, h + roofH/2.0f, 0.0f)
    );
    wing->AddChild(roof);

    // Floor 2 (Intermediate) - If 2-story
    bool isTwoStory = (h > 4.0f);
    float floor2Y = 3.5f; // Height of 2nd floor
    
    if (isTwoStory) {
        auto floor2 = createCuboid(
            glm::vec3(w - 2*wallThick, floorThick, d - 2*wallThick), // Slightly smaller to fit inside walls
            floorColor,
            glm::vec3(0.0f, floor2Y - floorThick/2.0f, 0.0f)
        );
        wing->AddChild(floor2);
        
        // Internal Staircase REMOVED as requested
    }
    
    // --- 2. FACADE SYSTEM (Front Wall with Windows/Doors) ---
    // We construct the front wall segment by segment
    
    float spacing = 2.5f;
    int count = static_cast<int>(w / spacing); // Number of slots
    if (count % 2 == 0) count--; // Ensure odd number
    
    float slotWidth = w / count;
    float startX = -w/2.0f + slotWidth/2.0f;
    
    float frontZ = d/2.0f - wallThick/2.0f;
    
    // Window params - RAISED HIGHER
    float winW = 1.2f;
    float winH = 1.4f;
    float winY1 = 2.2f; // Raised Floor 1 Window
    float winY2 = 5.6f; // Raised Floor 2 Window (Significantly higher)
    
    // Door params
    float doorW = 1.0f;
    float doorH = 2.4f; 
    
    for (int i = 0; i < count; ++i)
    {
        float currentX = startX + i * slotWidth;
        
        // --- PILLARS (Between slots) ---
        // Right side of this slot
        if (i < count - 1) {
            auto pillar = createCuboid(
                glm::vec3(0.3f, h, wallThick),
                pillarColor,
                glm::vec3(currentX + slotWidth/2.0f, h/2.0f, frontZ)
            );
            wing->AddChild(pillar);
        }
        
        // SPECIAL LOGIC for Center Wing
        // Floor 1: Center is Double Door. Adjacent are Windows. Rest are Walls.
        // Floor 2: Center-1 (Left) is Door. Center+1 (Right) is Window.
        
        bool isMid = (i == count/2);
        bool isMidLeft = (i == count/2 - 1);
        bool isMidRight = (i == count/2 + 1);
        
        bool hasDoor = false;
        
        if (isCenter) {
            // Floor 1 Logic
            // Only Main Center Door (Double)
            hasDoor = (isMid);
        } else {
             // Standard Logic for other wings
             // ... existing checks ...
             if (isCenter && i > count/2) hasDoor = false; // (This block handles Floor 2 if we are in loop? Wait, loop handles Floor 1 only here)
             // Ah, this loop is strictly FACADE (which implies Floor 1 if not 2-story? No, this loop builds wall columns which span H)
             // NO, looking at structure:
             // Pillars are height H.
             // Slot Content Floor 1 is lines 406-460.
             // Slot Content Floor 2 is lines 502-580.
             
             // So here is Floor 1 Logic ONLY.
        }
        
        // Re-eval hasDoor for Floor 1
         if (isCenter) {
             hasDoor = isMid; // Only center has door on Floor 1
         } else {
             // Existing logic for Left/Right wings
             // (Retain existing logic block)
             if (doorMode == 0) {
                  if (i % 2 == 0) hasDoor = true; 
             } else if ((doorFloor & 1) && doorMode != 4) {
                  if (doorMode == 1 && i == 0) hasDoor = true; // Left
                  else if (doorMode == 2 && i == count-1) hasDoor = true; // Right
                  else if (doorMode == 3 && (i == count/2 - 1 || i == count/2 + 1)) hasDoor = true; // Middle pair
             }
         }
        
        if (hasDoor) {
            bool isDoubleDoor = (isCenter && isMid);
            float actualDoorW = isDoubleDoor ? 1.6f : doorW; 
            
            // Door Slot
            // 1. Door Frame / Lintel
            float lintelH = floor2Y - doorH;
            if (lintelH > 0) {
                auto lintel = createCuboid(
                    glm::vec3(slotWidth, lintelH, wallThick),
                    wallColor,
                    glm::vec3(currentX, doorH + lintelH/2.0f, frontZ)
                );
                wing->AddChild(lintel);
            }
            
            // 2. Side Walls (Jambs)
            float sideGap = (slotWidth - actualDoorW) / 2.0f;
            if (sideGap > 0.05f) {
                // Left Jamb
                wing->AddChild(createCuboid(
                    glm::vec3(sideGap, doorH, wallThick),
                    wallColor,
                    glm::vec3(currentX - actualDoorW/2.0f - sideGap/2.0f, doorH/2.0f, frontZ)
                ));
                // Right Jamb
                wing->AddChild(createCuboid(
                    glm::vec3(sideGap, doorH, wallThick),
                    wallColor,
                    glm::vec3(currentX + actualDoorW/2.0f + sideGap/2.0f, doorH/2.0f, frontZ)
                ));
            }
            
                // 3. The Door Object(s)
            if (isDoubleDoor) {
                float leafW = 0.8f;
                // Left Leaf (-90 open)
                auto doorLeft = createDoor(leafW, doorH, -90.0f);
                doorLeft->SetLocalTransform(glm::translate(glm::mat4(1.0f), glm::vec3(currentX - leafW, 0.0f, frontZ + 0.1f)));
                wing->AddChild(doorLeft);
                
                // Right Leaf (90 open, Rotated body)
                auto doorRight = createDoor(leafW, doorH, 90.0f);
                glm::mat4 rt = glm::translate(glm::mat4(1.0f), glm::vec3(currentX + leafW, 0.0f, frontZ + 0.1f));
                rt = glm::rotate(rt, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                doorRight->SetLocalTransform(rt);
                wing->AddChild(doorRight);
                
            } else {
                auto doorObj = createDoor(doorW, doorH);
                // Offset by -doorW/2
                doorObj->SetLocalTransform(glm::translate(glm::mat4(1.0f), glm::vec3(currentX - doorW/2.0f, 0.0f, frontZ + 0.1f)));
                wing->AddChild(doorObj);
            }
            
            // --- FURNITURE PLACEMENT (CLASSROOM SETUP) ---
            // Layout: Determined by room position
            // Left Half of Wing (i < count/2): Room extends +X. Board on Left (-X). Desks face Left.
            // Right Half of Wing (i >= count/2): Room extends -X. Board on Right (+X). Desks face Right.
            
            float direction = (i < count/2) ? 1.0f : -1.0f; 
            // If Double Door at exact center, assume direction 1 (or symmetric?) -> Let's use 1.
            if (isCenter && isMid) direction = 1.0f;
            
            float roomDepth = d - 2*wallThick; 
            
            // 1. Blackboard
            auto board = createBlackboard(2.4f, 1.2f);
            // Position: 0.9m from door center in 'direction' reverse (i.e. towards the wall we lean on)
            // Actually: The "Side Wall" we face is at local 0 relative to room block?
            // If dir=1 (Extend Right), Board is on Left Wall (near door). X offset = -0.9f.
            // If dir=-1 (Extend Left), Board is on Right Wall (near door). X offset = +0.9f.
            float boardX = -0.9f * direction;
            
            glm::mat4 bt = glm::translate(glm::mat4(1.0f), glm::vec3(currentX + boardX, 0.0f, 0.0f));
            // Rotate: If dir=1, Face +X (90). If dir=-1, Face -X (-90).
            bt = glm::rotate(bt, glm::radians(90.0f * direction), glm::vec3(0.0f, 1.0f, 0.0f));
            board->SetLocalTransform(bt);
            wing->AddChild(board);
            
            // 2. Student Desks (Split into 2 groups for Aisle)
            int rows = 5; // More rows at back
            int groupCols = 2; // 2 cols per side
            // Total width used: 2*1.0 + Gap + 2*1.0 = ~5m
            
            float deskSpacingX = 1.1f;
            float deskSpacingZ = 1.1f; 
            float aisleWidth = 1.2f; // Gap in middle
            
            // Iterate rows (distance from board)
            for (int r = 0; r < rows; ++r) {
                // Determine X distance from door/board
                // Start gap: 0.8m.
                float dx = (0.8f + r * deskSpacingX) * direction;
                
                // Group 1 (Lower Z / Left side of aisle)
                for (int c = 0; c < groupCols; ++c) {
                    // Z pos relative to center: -HalfGap - Spacing
                    float zOffset = -(aisleWidth/2.0f + 0.5f + c * deskSpacingZ);
                     
                    glm::vec3 deskPos(currentX + dx, 0.0f, zOffset);
                    
                    // Table
                    auto table = createTable(1.1f, 0.5f, 0.7f);
                    glm::mat4 tt = glm::translate(glm::mat4(1.0f), deskPos);
                    tt = glm::rotate(tt, glm::radians(90.0f * direction), glm::vec3(0.0f, 1.0f, 0.0f));
                    table->SetLocalTransform(tt);
                    wing->AddChild(table);
                    
                    // Chair (Behind table)
                    auto chair = createChair(0.35f);
                    // Offset: +0.5 * direction (Since chair is "behind" along X)
                    glm::mat4 ct = glm::translate(glm::mat4(1.0f), deskPos + glm::vec3(0.5f * direction, 0.0f, 0.0f));
                    // Chair Back faces Board. Board is at -direction. Chair Front faces Board. 
                    // createChair faces +Z? No, wait. 
                    // Previous logic: rotate 90 -> Face -X. (When dir=1).
                    // If dir=1, Rotate 90. Chair Back at +X. Front at -X. Correct.
                    // If dir=-1, Rotate -90. Chair Back at -X. Front at +X. Correct.
                    ct = glm::rotate(ct, glm::radians(-90.0f * direction), glm::vec3(0.0f, 1.0f, 0.0f));
                    chair->SetLocalTransform(ct);
                    wing->AddChild(chair);
                }
                
                // Group 2 (Upper Z / Right side of aisle)
                for (int c = 0; c < groupCols; ++c) {
                    float zOffset = (aisleWidth/2.0f + 0.5f + c * deskSpacingZ);
                    
                    glm::vec3 deskPos(currentX + dx, 0.0f, zOffset);
                    
                    // Table
                    auto table = createTable(1.1f, 0.5f, 0.7f);
                    glm::mat4 tt = glm::translate(glm::mat4(1.0f), deskPos);
                    tt = glm::rotate(tt, glm::radians(90.0f * direction), glm::vec3(0.0f, 1.0f, 0.0f));
                    table->SetLocalTransform(tt);
                    wing->AddChild(table);
                    
                    // Chair
                    auto chair = createChair(0.35f);
                    glm::mat4 ct = glm::translate(glm::mat4(1.0f), deskPos + glm::vec3(0.5f * direction, 0.0f, 0.0f));
                    ct = glm::rotate(ct, glm::radians(-90.0f * direction), glm::vec3(0.0f, 1.0f, 0.0f));
                    chair->SetLocalTransform(ct);
                    wing->AddChild(chair);
                }
            }
            
        } else {
            // Window Slot (or Wall)
            bool isIntersection = (i < maskStart || i >= count - maskEnd);
            bool forceWindow = (isCenter && (isMidLeft || isMidRight)); // Force 2 side windows
            
            if ((withWindows && !isIntersection) || forceWindow) {
                // Wall Below Window
                float belowH = winY1 - winH/2.0f;
                wing->AddChild(createCuboid(
                    glm::vec3(slotWidth, belowH, wallThick),
                    wallColor,
                    glm::vec3(currentX, belowH/2.0f, frontZ)
                ));
                
                // The Window
                auto winObj = createWindow(winW, winH);
                winObj->SetLocalTransform(glm::translate(glm::mat4(1.0f), glm::vec3(currentX, winY1, frontZ)));
                wing->AddChild(winObj);
                
                // Wall Above Window (up to floor 2)
                float topY = floor2Y; 
                float baseWinTop = winY1 + winH/2.0f;
                float heightAbove = topY - baseWinTop;
                
                if (heightAbove > 0) {
                    wing->AddChild(createCuboid(
                        glm::vec3(slotWidth, heightAbove, wallThick),
                        wallColor,
                        glm::vec3(currentX, baseWinTop + heightAbove/2.0f, frontZ)
                    ));
                }
            } else {
                // Solid Wall Slot (No Window)
                wing->AddChild(createCuboid(
                     glm::vec3(slotWidth, floor2Y, wallThick),
                     wallColor,
                     glm::vec3(currentX, floor2Y/2.0f, frontZ)
                ));
            }
        }
        
        // --- SLOT CONTENT (Floor 2) ---
        if (isTwoStory) {
             // Logic for Floor 2 Doors and Windows
             bool hasDoor2 = false; // Default
             
             // Enable doors for floor 2 similar to floor 1 if configured
             if (doorFloor & 2) { // 2 = has 2nd floor doors
                 if (doorMode == 0) {
                     // Auto mode: same pattern as floor 1
                     if (i % 2 == 0) hasDoor2 = true;
                 } else if (doorMode != 4) {
                     // Mimic floor 1 custom modes
                     if (doorMode == 1 && i == 0) hasDoor2 = true;
                     else if (doorMode == 2 && i == count-1) hasDoor2 = true;
                     else if (doorMode == 3 && (i == count/2 - 1 || i == count/2 + 1)) hasDoor2 = true;
                 }
                 
                 // SPECIAL LOGIC: Center Wing (isCenter == true)
                 // User request: "1 bên thay cửa ra vào bằng cửa sổ" -> Replace Right Door with Window
                 // The right door is commonly at index > count/2
                 if (isCenter && i > count/2) {
                     hasDoor2 = false; // Disable right side door
                 }
             }

             if (hasDoor2) {
                // Door at Floor 2
                // 1. Lintel above door
                float lintelH = (h - floor2Y) - doorH; 
                if (lintelH > 0) {
                    wing->AddChild(createCuboid(
                        glm::vec3(slotWidth, lintelH, wallThick),
                        wallColor,
                        glm::vec3(currentX, floor2Y + doorH + lintelH/2.0f, frontZ)
                    ));
                }
                
                // 2. Side Walls (Jambs) to fill the gap (Door=1.0, Slot=2.5)
                float sideGap = (slotWidth - doorW) / 2.0f;
                if (sideGap > 0.05f) {
                    // Left Jamb
                    wing->AddChild(createCuboid(
                        glm::vec3(sideGap, doorH, wallThick),
                        wallColor,
                        glm::vec3(currentX - doorW/2.0f - sideGap/2.0f, floor2Y + doorH/2.0f, frontZ)
                    ));
                    // Right Jamb
                    wing->AddChild(createCuboid(
                        glm::vec3(sideGap, doorH, wallThick),
                        wallColor,
                        glm::vec3(currentX + doorW/2.0f + sideGap/2.0f, floor2Y + doorH/2.0f, frontZ)
                    ));
                }
                
                // 3. The Door Object
                auto doorObj2 = createDoor(doorW, doorH);
                // Offset by -doorW/2 to center
                doorObj2->SetLocalTransform(glm::translate(glm::mat4(1.0f), glm::vec3(currentX - doorW/2.0f, floor2Y, frontZ + 0.1f)));
                wing->AddChild(doorObj2);
                
                // --- FURNITURE PLACEMENT (FLOOR 2) ---
                
                float direction = (i < count/2) ? 1.0f : -1.0f; 
                if (isCenter && isMid) direction = 1.0f;
                // Center wing floor 2 has door at mid-1 (Left). 
                // If i=mid-1 (Left), dir=1. Extends Right. Correct.
                // If i=mid+1 (Right), Window. No door. (We still build furniture? Code implies furniture built per-slot if door exists?)
                // Wait, Floor 2 furniture block is inside `if (hasDoor2)`.
                // So furniture implies door.
                // If Center Wing Force Window logic creates furniture? No, that's in `else`.
                // So Floor 2 rooms only appear where doors are.
                
                // 1. Blackboard
                auto board = createBlackboard(2.4f, 1.2f);
                float boardX = -0.9f * direction;
                
                glm::mat4 bt = glm::translate(glm::mat4(1.0f), glm::vec3(currentX + boardX, floor2Y, 0.0f));
                bt = glm::rotate(bt, glm::radians(90.0f * direction), glm::vec3(0.0f, 1.0f, 0.0f));
                board->SetLocalTransform(bt);
                wing->AddChild(board);
                
                // 2. Student Desks
                int rows = 5;
                int groupCols = 2;
                float deskSpacingX = 1.1f;
                float deskSpacingZ = 1.1f;
                float aisleWidth = 1.2f;
                
                for (int r = 0; r < rows; ++r) {
                    float dx = (0.8f + r * deskSpacingX) * direction;
                    
                    // Group 1
                    for (int c = 0; c < groupCols; ++c) {
                        float zOffset = -(aisleWidth/2.0f + 0.5f + c * deskSpacingZ);
                        glm::vec3 deskPos(currentX + dx, floor2Y, zOffset);
                        
                        auto table = createTable(1.1f, 0.5f, 0.7f);
                        glm::mat4 tt = glm::translate(glm::mat4(1.0f), deskPos);
                        tt = glm::rotate(tt, glm::radians(90.0f * direction), glm::vec3(0.0f, 1.0f, 0.0f));
                        table->SetLocalTransform(tt);
                        wing->AddChild(table);
                        
                        auto chair = createChair(0.35f);
                        glm::mat4 ct = glm::translate(glm::mat4(1.0f), deskPos + glm::vec3(0.5f * direction, 0.0f, 0.0f));
                        ct = glm::rotate(ct, glm::radians(-90.0f * direction), glm::vec3(0.0f, 1.0f, 0.0f));
                        chair->SetLocalTransform(ct);
                        wing->AddChild(chair);
                    }
                    
                    // Group 2
                    for (int c = 0; c < groupCols; ++c) {
                        float zOffset = (aisleWidth/2.0f + 0.5f + c * deskSpacingZ);
                        glm::vec3 deskPos(currentX + dx, floor2Y, zOffset);
                        
                        auto table = createTable(1.1f, 0.5f, 0.7f);
                        glm::mat4 tt = glm::translate(glm::mat4(1.0f), deskPos);
                        tt = glm::rotate(tt, glm::radians(90.0f * direction), glm::vec3(0.0f, 1.0f, 0.0f));
                        table->SetLocalTransform(tt);
                        wing->AddChild(table);
                        
                        auto chair = createChair(0.35f);
                        glm::mat4 ct = glm::translate(glm::mat4(1.0f), deskPos + glm::vec3(0.5f * direction, 0.0f, 0.0f));
                        ct = glm::rotate(ct, glm::radians(-90.0f * direction), glm::vec3(0.0f, 1.0f, 0.0f));
                        chair->SetLocalTransform(ct);
                        wing->AddChild(chair);
                    }
                }
                
             } else {
                 bool isIntersection = (i < maskStart || i >= count - maskEnd);
                 bool forceWindow = (isCenter && isMidRight); // Force window on right side (symmetric to Door)
                 
                 if ((withWindows && !isIntersection) || forceWindow) {
                    // Wall Below Window (from floor2Y to Window Bottom)
                    float baseH = floor2Y;
                    float winBottom = winY2 - winH/2.0f;
                    float belowH = winBottom - baseH;
                    
                    if (belowH > 0) {
                        wing->AddChild(createCuboid(
                            glm::vec3(slotWidth, belowH, wallThick),
                            wallColor,
                            glm::vec3(currentX, baseH + belowH/2.0f, frontZ)
                        ));
                    }
                    
                    // Window
                    auto winObj2 = createWindow(winW, winH);
                    winObj2->SetLocalTransform(glm::translate(glm::mat4(1.0f), glm::vec3(currentX, winY2, frontZ)));
                    wing->AddChild(winObj2);
                    
                    // Wall Above Window (up to roof)
                    float winTop = winY2 + winH/2.0f;
                    float heightAbove = h - winTop;
                    
                    if (heightAbove > 0) {
                        wing->AddChild(createCuboid(
                            glm::vec3(slotWidth, heightAbove, wallThick),
                            wallColor,
                            glm::vec3(currentX, winTop + heightAbove/2.0f, frontZ)
                        ));
                    }
                 } else {
                     // Solid Wall
                     float height = h - floor2Y;
                     wing->AddChild(createCuboid(
                         glm::vec3(slotWidth, height, wallThick),
                         wallColor,
                         glm::vec3(currentX, floor2Y + height/2.0f, frontZ)
                    ));
                 }
             }
        }
    }

    
    // -- 2nd Floor Corridor / Balcony --
    // Extending from the front face (Z+) at 2nd floor level (y=3.5)
    
    float corridorDepth = 1.6f; 
    float corridorThickness = 0.2f;
    
    // Cấu hình ban công từ tham số hàm
    float corridorExtraLength = balconyExtraLength;
    float corridorWidthRatio = balconyWidthRatio;
    float corridorOffsetX = balconyOffsetX;
    float corridorWidth = (w + corridorExtraLength) * corridorWidthRatio;
    
    auto corridor = createCuboid(
        glm::vec3(corridorWidth, corridorThickness, corridorDepth),
        floorColor, // Match floor
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
    float barSpacing = 0.4f; 
    float totalWidth = corridorWidth;
    int numBars = static_cast<int>(totalWidth / barSpacing);
    
    // Phạm vi thanh chắn
    float minBarX, maxBarX;
    if (useCustomBarRange) {
        minBarX = customBarMinX;
        maxBarX = customBarMaxX;
    } else {
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

static SceneNode::Ptr createIronGate(float width, float height) {
    auto gate = std::make_shared<SceneNode>();
    
    // Frame
    float frameThick = 0.15f;
    glm::vec3 darkMetal(0.1f, 0.1f, 0.12f);
    
    // Outer Frame
    // Left
    gate->AddChild(createCuboid(glm::vec3(frameThick, height, frameThick), darkMetal, glm::vec3(-width/2 + frameThick/2, height/2, 0)));
    // Right
    gate->AddChild(createCuboid(glm::vec3(frameThick, height, frameThick), darkMetal, glm::vec3(width/2 - frameThick/2, height/2, 0)));
    // Top
    gate->AddChild(createCuboid(glm::vec3(width, frameThick, frameThick), darkMetal, glm::vec3(0, height - frameThick/2, 0)));
    // Bottom
    gate->AddChild(createCuboid(glm::vec3(width, frameThick, frameThick), darkMetal, glm::vec3(0, frameThick/2, 0)));

    // Vertical Bars
    int numBars = (int)(width / 0.3f);
    for(int i=1; i<numBars; ++i) {
        float x = -width/2 + i * (width/numBars);
        gate->AddChild(createCuboid(glm::vec3(0.05f, height - 2*frameThick, 0.05f), darkMetal, glm::vec3(x, height/2, 0)));
    }
    return gate;
}

static SceneNode::Ptr createLeverObj() {
    auto leverNode = std::make_shared<SceneNode>();
    
    // Base
    leverNode->AddChild(createCuboid(glm::vec3(0.4f, 0.1f, 0.4f), glm::vec3(0.3f), glm::vec3(0, 0.05f, 0)));
    
    // Handle (Pivot point)
    auto handle = std::make_shared<SceneNode>();
    // Stick
    handle->AddChild(createCuboid(glm::vec3(0.05f, 0.6f, 0.05f), glm::vec3(0.8f, 0.0f, 0.0f), glm::vec3(0, 0.3f, 0)));
    // Knob
    handle->AddChild(createCuboid(glm::vec3(0.15f, 0.15f, 0.15f), glm::vec3(1,0,0), glm::vec3(0, 0.6f, 0)));
    
    handle->SetLocalTransform(glm::translate(glm::mat4(1.0f), glm::vec3(0, 0.1f, 0)));
    
    leverNode->AddChild(handle);
    // Children: 0=Base, 1=Handle
    return leverNode;
}

// Helper to create a flagpole with waving Vietnamese flag
static SceneNode::Ptr createFlagpole(float height = 10.0f)
{
    auto flagpoleNode = std::make_shared<SceneNode>();
    
    // Clear old flag parts if any
    SchoolBuilder::s_flagParts.clear();
    
    
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
        
        // Save for animation
        SchoolBuilder::FlagPart part;
        part.node = flagSegment;
        part.xOffset = xOffset;
        part.initialTransform = flagSegment->GetLocalTransform();
        SchoolBuilder::s_flagParts.push_back(part);
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
                
                // Save for animation
                SchoolBuilder::FlagPart part;
                part.node = pixel;
                part.xOffset = x; // World X relative to pole is approximately x (since pole is at 0,0,0 local)
                part.initialTransform = pixel->GetLocalTransform();
                SchoolBuilder::s_flagParts.push_back(part);
            }
        }
    }

    
    return flagpoleNode;
}

// Helper to create a classical statue on a pedestal
static SceneNode::Ptr createStatue()
{
    auto statueNode = std::make_shared<SceneNode>();
    
    // Color palette
    glm::vec3 stoneGray(0.55f, 0.55f, 0.58f);      // Base stone
    glm::vec3 marbleWhite(0.92f, 0.92f, 0.95f);    // Statue marble
    glm::vec3 darkStone(0.35f, 0.35f, 0.38f);      // Accent stone
    glm::vec3 bronze(0.7f, 0.5f, 0.3f);            // Bronze plaques
    
    // === BASE PLATFORM (3-tiered) ===
    // Bottom tier (largest)
    auto baseTier1 = createCuboid(
        glm::vec3(3.0f, 0.3f, 3.0f),
        darkStone,
        glm::vec3(0.0f, 0.15f, 0.0f)
    );
    statueNode->AddChild(baseTier1);
    
    // Middle tier
    auto baseTier2 = createCuboid(
        glm::vec3(2.6f, 0.25f, 2.6f),
        stoneGray,
        glm::vec3(0.0f, 0.425f, 0.0f)
    );
    statueNode->AddChild(baseTier2);
    
    // Top tier
    auto baseTier3 = createCuboid(
        glm::vec3(2.2f, 0.2f, 2.2f),
        darkStone,
        glm::vec3(0.0f, 0.65f, 0.0f)
    );
    statueNode->AddChild(baseTier3);
    
    // === PEDESTAL ===
    float pedestalHeight = 2.5f;
    float pedestalWidth = 1.2f;
    
    // Main pedestal column
    auto pedestal = createCuboid(
        glm::vec3(pedestalWidth, pedestalHeight, pedestalWidth),
        marbleWhite,
        glm::vec3(0.0f, 0.75f + pedestalHeight/2.0f, 0.0f)
    );
    statueNode->AddChild(pedestal);
    
    // Pedestal top cap
    auto pedestalCap = createCuboid(
        glm::vec3(pedestalWidth + 0.2f, 0.15f, pedestalWidth + 0.2f),
        stoneGray,
        glm::vec3(0.0f, 0.75f + pedestalHeight + 0.075f, 0.0f)
    );
    statueNode->AddChild(pedestalCap);
    
    // Decorative bronze plaques on all four pedestal sides
    float plaqueY = 0.75f + pedestalHeight/2.0f;
    
    // Front plaque
    auto plaqueFront = createCuboid(
        glm::vec3(0.7f, 0.5f, 0.02f),
        bronze,
        glm::vec3(0.0f, plaqueY, pedestalWidth/2.0f + 0.01f)
    );
    statueNode->AddChild(plaqueFront);
    
    // Back plaque
    auto plaqueBack = createCuboid(
        glm::vec3(0.7f, 0.5f, 0.02f),
        bronze,
        glm::vec3(0.0f, plaqueY, -pedestalWidth/2.0f - 0.01f)
    );
    statueNode->AddChild(plaqueBack);
    
    // Left plaque
    auto plaqueLeft = createCuboid(
        glm::vec3(0.02f, 0.5f, 0.7f),
        bronze,
        glm::vec3(-pedestalWidth/2.0f - 0.01f, plaqueY, 0.0f)
    );
    statueNode->AddChild(plaqueLeft);
    
    // Right plaque
    auto plaqueRight = createCuboid(
        glm::vec3(0.02f, 0.5f, 0.7f),
        bronze,
        glm::vec3(pedestalWidth/2.0f + 0.01f, plaqueY, 0.0f)
    );
    statueNode->AddChild(plaqueRight);
    
    // Decorative bands on pedestal
    for (int i = 0; i < 3; ++i) {
        float bandY = 0.75f + 0.5f + i * 0.7f;
        auto band = createCuboid(
            glm::vec3(pedestalWidth + 0.15f, 0.08f, pedestalWidth + 0.15f),
            bronze,
            glm::vec3(0.0f, bandY, 0.0f)
        );
        statueNode->AddChild(band);
    }
    
    // === STATUE FIGURE ===
    float figureBaseY = 0.75f + pedestalHeight + 0.15f;
    
    // Base/feet
    auto feet = createCuboid(
        glm::vec3(0.6f, 0.25f, 0.5f),
        marbleWhite,
        glm::vec3(0.0f, figureBaseY + 0.125f, 0.0f)
    );
    statueNode->AddChild(feet);
    
    // Lower body/robe (flowing)
    auto lowerBody = createCuboid(
        glm::vec3(0.8f, 1.2f, 0.7f),
        marbleWhite,
        glm::vec3(0.0f, figureBaseY + 0.25f + 0.6f, 0.0f)
    );
    statueNode->AddChild(lowerBody);
    
    // Robe detail - flowing fabric
    auto robeDetail = createCuboid(
        glm::vec3(0.85f, 0.3f, 0.65f),
        marbleWhite,
        glm::vec3(0.0f, figureBaseY + 0.5f, 0.0f)
    );
    statueNode->AddChild(robeDetail);
    
    // Upper torso
    auto torso = createCuboid(
        glm::vec3(0.7f, 0.9f, 0.6f),
        marbleWhite,
        glm::vec3(0.0f, figureBaseY + 1.45f + 0.45f, 0.0f)
    );
    statueNode->AddChild(torso);
    
    // Shoulders/chest detail
    auto shoulders = createCuboid(
        glm::vec3(0.9f, 0.25f, 0.55f),
        marbleWhite,
        glm::vec3(0.0f, figureBaseY + 1.9f + 0.125f, 0.0f)
    );
    statueNode->AddChild(shoulders);
    
    // Left arm (raised, holding something)
    auto leftArmUpper = createCuboid(
        glm::vec3(0.18f, 0.5f, 0.18f),
        marbleWhite,
        glm::vec3(-0.5f, figureBaseY + 1.9f, 0.0f)
    );
    statueNode->AddChild(leftArmUpper);
    
    auto leftArmLower = createCuboid(
        glm::vec3(0.16f, 0.5f, 0.16f),
        marbleWhite,
        glm::vec3(-0.5f, figureBaseY + 2.4f, 0.0f)
    );
    statueNode->AddChild(leftArmLower);
    
    // Right arm (at side, bent)
    auto rightArmUpper = createCuboid(
        glm::vec3(0.18f, 0.5f, 0.18f),
        marbleWhite,
        glm::vec3(0.5f, figureBaseY + 1.7f, 0.0f)
    );
    statueNode->AddChild(rightArmUpper);
    
    auto rightArmLower = createCuboid(
        glm::vec3(0.16f, 0.4f, 0.16f),
        marbleWhite,
        glm::vec3(0.5f, figureBaseY + 1.2f, 0.1f)
    );
    statueNode->AddChild(rightArmLower);
    
    // Head (larger, more detailed)
    auto head = createCuboid(
        glm::vec3(0.4f, 0.45f, 0.4f),
        marbleWhite,
        glm::vec3(0.0f, figureBaseY + 2.4f, 0.0f)
    );
    statueNode->AddChild(head);
    
    // Facial features - nose
    auto nose = createCuboid(
        glm::vec3(0.08f, 0.12f, 0.1f),
        marbleWhite,
        glm::vec3(0.0f, figureBaseY + 2.4f, 0.22f)
    );
    statueNode->AddChild(nose);
    
    // Eyes (recessed)
    auto leftEye = createCuboid(
        glm::vec3(0.06f, 0.06f, 0.04f),
        darkStone,
        glm::vec3(-0.1f, figureBaseY + 2.45f, 0.18f)
    );
    statueNode->AddChild(leftEye);
    
    auto rightEye = createCuboid(
        glm::vec3(0.06f, 0.06f, 0.04f),
        darkStone,
        glm::vec3(0.1f, figureBaseY + 2.45f, 0.18f)
    );
    statueNode->AddChild(rightEye);
    
    // Crown/headpiece (more ornate)
    auto crownBase = createCuboid(
        glm::vec3(0.48f, 0.08f, 0.48f),
        bronze,
        glm::vec3(0.0f, figureBaseY + 2.66f, 0.0f)
    );
    statueNode->AddChild(crownBase);
    
    auto crownTop = createCuboid(
        glm::vec3(0.42f, 0.12f, 0.42f),
        bronze,
        glm::vec3(0.0f, figureBaseY + 2.76f, 0.0f)
    );
    statueNode->AddChild(crownTop);
    
    // Crown points/spikes
    for (int i = 0; i < 4; ++i) {
        float angle = i * 90.0f;
        float x = std::cos(glm::radians(angle)) * 0.22f;
        float z = std::sin(glm::radians(angle)) * 0.22f;
        
        auto spike = createCuboid(
            glm::vec3(0.06f, 0.15f, 0.06f),
            bronze,
            glm::vec3(x, figureBaseY + 2.87f, z)
        );
        statueNode->AddChild(spike);
    }
    
    // Book/scroll in raised hand (more detailed)
    auto book = createCuboid(
        glm::vec3(0.3f, 0.22f, 0.26f),
        bronze,
        glm::vec3(-0.5f, figureBaseY + 2.7f, 0.0f)
    );
    statueNode->AddChild(book);
    
    // Book pages detail
    auto bookPages = createCuboid(
        glm::vec3(0.28f, 0.2f, 0.02f),
        marbleWhite,
        glm::vec3(-0.5f, figureBaseY + 2.7f, 0.14f)
    );
    statueNode->AddChild(bookPages);
    
    // Hand holding book
    auto leftHand = createCuboid(
        glm::vec3(0.12f, 0.15f, 0.1f),
        marbleWhite,
        glm::vec3(-0.5f, figureBaseY + 2.55f, 0.0f)
    );
    statueNode->AddChild(leftHand);
    
    // Right hand (at side)
    auto rightHand = createCuboid(
        glm::vec3(0.12f, 0.15f, 0.1f),
        marbleWhite,
        glm::vec3(0.5f, figureBaseY + 1.0f, 0.12f)
    );
    statueNode->AddChild(rightHand);
    
    // Decorative sash/belt (more ornate)
    auto belt = createCuboid(
        glm::vec3(0.85f, 0.12f, 0.65f),
        bronze,
        glm::vec3(0.0f, figureBaseY + 1.45f, 0.0f)
    );
    statueNode->AddChild(belt);
    
    // Belt buckle/ornament
    auto buckle = createCuboid(
        glm::vec3(0.15f, 0.15f, 0.08f),
        bronze,
        glm::vec3(0.0f, figureBaseY + 1.45f, 0.38f)
    );
    statueNode->AddChild(buckle);
    
    // Drapery folds on robe (vertical)
    for (int i = 0; i < 5; ++i) {
        float xPos = -0.3f + i * 0.15f;
        auto fold = createCuboid(
            glm::vec3(0.04f, 0.8f, 0.05f),
            marbleWhite,
            glm::vec3(xPos, figureBaseY + 0.7f, 0.38f)
        );
        statueNode->AddChild(fold);
    }
    
    // Additional horizontal drapery folds
    for (int i = 0; i < 3; ++i) {
        float yPos = figureBaseY + 0.6f + i * 0.3f;
        auto hFold = createCuboid(
            glm::vec3(0.75f, 0.03f, 0.05f),
            marbleWhite,
            glm::vec3(0.0f, yPos, 0.37f)
        );
        statueNode->AddChild(hFold);
    }
    
    // Cloak/cape flowing behind
    auto cape = createCuboid(
        glm::vec3(0.7f, 1.2f, 0.15f),
        marbleWhite,
        glm::vec3(0.0f, figureBaseY + 1.5f, -0.4f)
    );
    statueNode->AddChild(cape);
    
    // Cape folds/texture
    for (int i = 0; i < 4; ++i) {
        float xPos = -0.25f + i * 0.17f;
        auto capeFold = createCuboid(
            glm::vec3(0.04f, 1.0f, 0.06f),
            marbleWhite,
            glm::vec3(xPos, figureBaseY + 1.5f, -0.48f)
        );
        statueNode->AddChild(capeFold);
    }
    
    // Decorative scrollwork on shoulders
    for (int i = 0; i < 2; ++i) {
        float xPos = (i == 0) ? -0.48f : 0.48f;
        auto scroll = createCuboid(
            glm::vec3(0.08f, 0.12f, 0.08f),
            bronze,
            glm::vec3(xPos, figureBaseY + 2.1f, 0.0f)
        );
        statueNode->AddChild(scroll);
    }
    
    // Necklace/collar ornament
    auto collar = createCuboid(
        glm::vec3(0.6f, 0.08f, 0.45f),
        bronze,
        glm::vec3(0.0f, figureBaseY + 1.88f, 0.0f)
    );
    statueNode->AddChild(collar);
    
    // Decorative medallion on chest
    auto medallion = createCuboid(
        glm::vec3(0.15f, 0.15f, 0.05f),
        bronze,
        glm::vec3(0.0f, figureBaseY + 1.7f, 0.32f)
    );
    statueNode->AddChild(medallion);
    
    // Decorative frieze on pedestal (relief band)
    for (int i = 0; i < 8; ++i) {
        float angle = i * 45.0f;
        float x = std::cos(glm::radians(angle)) * (pedestalWidth/2.0f + 0.01f);
        float z = std::sin(glm::radians(angle)) * (pedestalWidth/2.0f + 0.01f);
        
        auto friezeElement = createCuboid(
            glm::vec3(0.12f, 0.12f, 0.03f),
            bronze,
            glm::vec3(x, 0.75f + pedestalHeight * 0.7f, z)
        );
        statueNode->AddChild(friezeElement);
    }
    
    // Decorative elements on base
    // Corner posts with capitals
    for (int i = 0; i < 4; ++i) {
        float angle = i * 90.0f;
        float x = std::cos(glm::radians(angle)) * 1.4f;
        float z = std::sin(glm::radians(angle)) * 1.4f;
        
        // Post base
        auto postBase = createCuboid(
            glm::vec3(0.2f, 0.1f, 0.2f),
            bronze,
            glm::vec3(x, 0.05f, z)
        );
        statueNode->AddChild(postBase);
        
        // Post shaft
        auto post = createCuboid(
            glm::vec3(0.14f, 0.7f, 0.14f),
            darkStone,
            glm::vec3(x, 0.45f, z)
        );
        statueNode->AddChild(post);
        
        // Decorative bands on posts
        for (int j = 0; j < 2; ++j) {
            float bandY = 0.3f + j * 0.3f;
            auto postBand = createCuboid(
                glm::vec3(0.18f, 0.04f, 0.18f),
                bronze,
                glm::vec3(x, bandY, z)
            );
            statueNode->AddChild(postBand);
        }
        
        // Post capital
        auto capital = createCuboid(
            glm::vec3(0.2f, 0.12f, 0.2f),
            bronze,
            glm::vec3(x, 0.86f, z)
        );
        statueNode->AddChild(capital);
        
        // Decorative sphere on top
        auto sphere = createCuboid(
            glm::vec3(0.18f, 0.18f, 0.18f),
            bronze,
            glm::vec3(x, 1.0f, z)
        );
        statueNode->AddChild(sphere);
    }
    
    // Additional decorative reliefs on base tiers
    for (int i = 0; i < 4; ++i) {
        float angle = i * 90.0f + 45.0f; // Offset by 45 degrees
        float x = std::cos(glm::radians(angle)) * 1.05f;
        float z = std::sin(glm::radians(angle)) * 1.05f;
        
        auto relief = createCuboid(
            glm::vec3(0.15f, 0.15f, 0.05f),
            bronze,
            glm::vec3(x, 0.5f, z)
        );
        statueNode->AddChild(relief);
    }
    
    return statueNode;
}

// Helper to create interactive light control panel
static SceneNode::Ptr createControlPanel()
{
    auto panelNode = std::make_shared<SceneNode>();
    
    // Colors
    glm::vec3 panelGray(0.3f, 0.3f, 0.35f);      // Dark gray panel
    glm::vec3 buttonGreen(0.2f, 0.8f, 0.3f);     // Green for +
    glm::vec3 buttonRed(0.9f, 0.2f, 0.2f);       // Red for -
    glm::vec3 switchBase(0.5f, 0.5f, 0.5f);      // Gray switch base
    glm::vec3 switchLever(0.8f, 0.3f, 0.1f);     // Orange lever
    
    // === PANEL BASE ===
    auto panelBase = createCuboid(
        glm::vec3(1.2f, 1.5f, 0.1f),
        panelGray,
        glm::vec3(0.0f, 0.75f, 0.0f)
    );
    panelNode->AddChild(panelBase);
    
    // === TOGGLE SWITCH (ON/OFF) ===
    // Switch base
    auto switchBaseObj = createCuboid(
        glm::vec3(0.3f, 0.15f, 0.15f),
        switchBase,
        glm::vec3(0.0f, 1.0f, 0.08f)
    );
    panelNode->AddChild(switchBaseObj);
    
    // Switch lever (will rotate based on state)
    auto switchLeverObj = createCuboid(
        glm::vec3(0.08f, 0.25f, 0.08f),
        switchLever,
        glm::vec3(0.0f, 1.0f, 0.15f)  // Slightly forward
    );
    panelNode->AddChild(switchLeverObj);
    
    // === PLUS BUTTON ===
    auto plusButton = createCuboid(
        glm::vec3(0.25f, 0.25f, 0.1f),
        buttonGreen,
        glm::vec3(0.0f, 0.6f, 0.08f)
    );
    panelNode->AddChild(plusButton);
    
    // Plus symbol (horizontal)
    auto plusH = createCuboid(
        glm::vec3(0.15f, 0.03f, 0.03f),
        glm::vec3(1.0f, 1.0f, 1.0f),
        glm::vec3(0.0f, 0.6f, 0.14f)
    );
    panelNode->AddChild(plusH);
    
    // Plus symbol (vertical)
    auto plusV = createCuboid(
        glm::vec3(0.03f, 0.15f, 0.03f),
        glm::vec3(1.0f, 1.0f, 1.0f),
        glm::vec3(0.0f, 0.6f, 0.14f)
    );
    panelNode->AddChild(plusV);
    
    // === MINUS BUTTON ===
    auto minusButton = createCuboid(
        glm::vec3(0.25f, 0.25f, 0.1f),
        buttonRed,
        glm::vec3(0.0f, 0.25f, 0.08f)
    );
    panelNode->AddChild(minusButton);
    
    // Minus symbol
    auto minusSymbol = createCuboid(
        glm::vec3(0.15f, 0.03f, 0.03f),
        glm::vec3(1.0f, 1.0f, 1.0f),
        glm::vec3(0.0f, 0.25f, 0.14f)
    );
    panelNode->AddChild(minusSymbol);
    
    return panelNode;
}

// Helper to create a multi-tiered fountain
static SceneNode::Ptr createFountain()
{
    auto fountainNode = std::make_shared<SceneNode>();
    
    // Color palette
    glm::vec3 stoneGray(0.6f, 0.6f, 0.65f);        // Main stone
    glm::vec3 darkStone(0.4f, 0.4f, 0.45f);        // Accent stone
    glm::vec3 waterBlue(0.3f, 0.5f, 0.7f);         // Water color
    glm::vec3 bronze(0.7f, 0.5f, 0.3f);            // Bronze accents
    glm::vec3 marble(0.9f, 0.9f, 0.92f);           // Light marble
    
    // === BASE POOL (Water Basin) - SIMPLIFIED FOR VISIBILITY ===
    float poolRadius = 3.5f;
    float poolHeight = 0.3f;
    
    // Pool base/floor (dark stone)
    auto poolBase = createCuboid(
        glm::vec3(poolRadius * 2, 0.2f, poolRadius * 2),
        darkStone,
        glm::vec3(0.0f, 0.1f, 0.0f)
    );
    fountainNode->AddChild(poolBase);
    
    // Pool walls (4 sides) - TALLER THAN WATER
    float wallHeight = 0.4f;
    float wallThickness = 0.12f;
    
    // Front wall
    auto wallFront = createCuboid(
        glm::vec3(poolRadius * 2, wallHeight, wallThickness),
        darkStone,
        glm::vec3(0.0f, 0.2f + wallHeight / 2.0f, poolRadius - wallThickness / 2.0f)
    );
    fountainNode->AddChild(wallFront);
    
    // Back wall
    auto wallBack = createCuboid(
        glm::vec3(poolRadius * 2, wallHeight, wallThickness),
        darkStone,
        glm::vec3(0.0f, 0.2f + wallHeight / 2.0f, -poolRadius + wallThickness / 2.0f)
    );
    fountainNode->AddChild(wallBack);
    
    // Left wall
    auto wallLeft = createCuboid(
        glm::vec3(wallThickness, wallHeight, poolRadius * 2 - wallThickness * 2),
        darkStone,
        glm::vec3(-poolRadius + wallThickness / 2.0f, 0.2f + wallHeight / 2.0f, 0.0f)
    );
    fountainNode->AddChild(wallLeft);
    
    // Right wall
    auto wallRight = createCuboid(
        glm::vec3(wallThickness, wallHeight, poolRadius * 2 - wallThickness * 2),
        darkStone,
        glm::vec3(poolRadius - wallThickness / 2.0f, 0.2f + wallHeight / 2.0f, 0.0f)
    );
    fountainNode->AddChild(wallRight);
    
    // Pool rim/edge (bronze decorative border - 4 sides around water)
    float rimWidth = 0.15f;
    float rimHeight = 0.08f;
    float rimY = 0.2f + wallHeight;
    
    // Front rim
    auto rimFront = createCuboid(
        glm::vec3(poolRadius * 2 + wallThickness, rimHeight, rimWidth),
        bronze,
        glm::vec3(0.0f, rimY, poolRadius)
    );
    fountainNode->AddChild(rimFront);
    
    // Back rim
    auto rimBack = createCuboid(
        glm::vec3(poolRadius * 2 + wallThickness, rimHeight, rimWidth),
        bronze,
        glm::vec3(0.0f, rimY, -poolRadius)
    );
    fountainNode->AddChild(rimBack);
    
    // Left rim
    auto rimLeft = createCuboid(
        glm::vec3(rimWidth, rimHeight, poolRadius * 2 + wallThickness),
        bronze,
        glm::vec3(-poolRadius, rimY, 0.0f)
    );
    fountainNode->AddChild(rimLeft);
    
    // Right rim
    auto rimRight = createCuboid(
        glm::vec3(rimWidth, rimHeight, poolRadius * 2 + wallThickness),
        bronze,
        glm::vec3(poolRadius, rimY, 0.0f)
    );
    fountainNode->AddChild(rimRight);
    
    // WATER - LOWER LEVEL, INSIDE WALLS
    auto poolWater = createCuboid(
        glm::vec3(poolRadius * 2 - wallThickness * 2 - 0.1f, 0.25f, poolRadius * 2 - wallThickness * 2 - 0.1f),
        glm::vec3(0.4f, 0.75f, 1.0f),  // BRIGHT cyan
        glm::vec3(0.0f, 0.325f, 0.0f)  // Lower: inside walls, below rim
    );
    fountainNode->AddChild(poolWater);
    
    // Decorative pool edge details
    for (int i = 0; i < 12; ++i) {
        float angle = i * 30.0f;
        float x = std::cos(glm::radians(angle)) * (poolRadius - 0.2f);
        float z = std::sin(glm::radians(angle)) * (poolRadius - 0.2f);
        
        auto edgeOrnament = createCuboid(
            glm::vec3(0.15f, 0.2f, 0.15f),
            bronze,
            glm::vec3(x, poolHeight, z)
        );
        fountainNode->AddChild(edgeOrnament);
    }
    
    // === TIER 1 (Bottom tier) ===
    float tier1Y = poolHeight + 0.3f;
    float tier1Radius = 2.0f;
    
    // Tier 1 basin
    auto tier1Basin = createCuboid(
        glm::vec3(tier1Radius * 2, 0.25f, tier1Radius * 2),
        marble,
        glm::vec3(0.0f, tier1Y, 0.0f)
    );
    fountainNode->AddChild(tier1Basin);
    
    // Tier 1 rim
    auto tier1Rim = createCuboid(
        glm::vec3(tier1Radius * 2 + 0.2f, 0.1f, tier1Radius * 2 + 0.2f),
        bronze,
        glm::vec3(0.0f, tier1Y + 0.15f, 0.0f)
    );
    fountainNode->AddChild(tier1Rim);
    
    // Tier 1 water - MORE VISIBLE
    auto tier1Water = createCuboid(
        glm::vec3(tier1Radius * 2 - 0.2f, 0.15f, tier1Radius * 2 - 0.2f),  // Taller
        glm::vec3(0.2f, 0.6f, 0.85f),  // Brighter blue
        glm::vec3(0.0f, tier1Y + 0.15f, 0.0f)  // Higher position
    );
    fountainNode->AddChild(tier1Water);
    
    // Water spouts on tier 1
    for (int i = 0; i < 8; ++i) {
        float angle = i * 45.0f;
        float x = std::cos(glm::radians(angle)) * tier1Radius;
        float z = std::sin(glm::radians(angle)) * tier1Radius;
        
        auto spout = createCuboid(
            glm::vec3(0.08f, 0.15f, 0.08f),
            bronze,
            glm::vec3(x, tier1Y + 0.2f, z)
        );
        fountainNode->AddChild(spout);
    }
    
    // === TIER 2 (Middle tier) ===
    float tier2Y = tier1Y + 0.8f;
    float tier2Radius = 1.3f;
    
    // Tier 2 basin
    auto tier2Basin = createCuboid(
        glm::vec3(tier2Radius * 2, 0.2f, tier2Radius * 2),
        marble,
        glm::vec3(0.0f, tier2Y, 0.0f)
    );
    fountainNode->AddChild(tier2Basin);
    
    // Tier 2 rim
    auto tier2Rim = createCuboid(
        glm::vec3(tier2Radius * 2 + 0.15f, 0.08f, tier2Radius * 2 + 0.15f),
        bronze,
        glm::vec3(0.0f, tier2Y + 0.12f, 0.0f)
    );
    fountainNode->AddChild(tier2Rim);
    
    // Tier 2 water - MORE VISIBLE
    auto tier2Water = createCuboid(
        glm::vec3(tier2Radius * 2 - 0.15f, 0.12f, tier2Radius * 2 - 0.15f),  // Taller
        glm::vec3(0.2f, 0.6f, 0.85f),  // Brighter blue
        glm::vec3(0.0f, tier2Y + 0.12f, 0.0f)  // Higher position
    );
    fountainNode->AddChild(tier2Water);
    
    // === TIER 3 (Upper tier) ===
    float tier3Y = tier2Y + 0.6f;
    float tier3Radius = 0.8f;
    
    // Tier 3 basin
    auto tier3Basin = createCuboid(
        glm::vec3(tier3Radius * 2, 0.15f, tier3Radius * 2),
        marble,
        glm::vec3(0.0f, tier3Y, 0.0f)
    );
    fountainNode->AddChild(tier3Basin);
    
    // Tier 3 rim
    auto tier3Rim = createCuboid(
        glm::vec3(tier3Radius * 2 + 0.1f, 0.06f, tier3Radius * 2 + 0.1f),
        bronze,
        glm::vec3(0.0f, tier3Y + 0.09f, 0.0f)
    );
    fountainNode->AddChild(tier3Rim);
    
    // Tier 3 water - MORE VISIBLE
    auto tier3Water = createCuboid(
        glm::vec3(tier3Radius * 2 - 0.1f, 0.1f, tier3Radius * 2 - 0.1f),  // Taller
        glm::vec3(0.2f, 0.6f, 0.85f),  // Brighter blue
        glm::vec3(0.0f, tier3Y + 0.1f, 0.0f)  // Higher position
    );
    fountainNode->AddChild(tier3Water);
    

    
    // === TOP ORNAMENT ===
    float topY = tier3Y + 0.4f;
    
    // Top pedestal
    auto topPedestal = createCuboid(
        glm::vec3(0.3f, 0.5f, 0.3f),
        marble,
        glm::vec3(0.0f, topY, 0.0f)
    );
    fountainNode->AddChild(topPedestal);
    
    // Top ornament (decorative finial)
    auto topOrnament = createCuboid(
        glm::vec3(0.25f, 0.3f, 0.25f),
        bronze,
        glm::vec3(0.0f, topY + 0.4f, 0.0f)
    );
    fountainNode->AddChild(topOrnament);
    
    // Top sphere
    auto topSphere = createCuboid(
        glm::vec3(0.2f, 0.2f, 0.2f),
        bronze,
        glm::vec3(0.0f, topY + 0.65f, 0.0f)
    );
    fountainNode->AddChild(topSphere);
    
    // === CENTRAL SUPPORT COLUMNS ===
    // Column from pool to tier 1
    auto column1 = createCuboid(
        glm::vec3(0.4f, tier1Y - poolHeight - 0.3f, 0.4f),
        stoneGray,
        glm::vec3(0.0f, poolHeight + (tier1Y - poolHeight - 0.3f)/2.0f, 0.0f)
    );
    fountainNode->AddChild(column1);
    
    // Column from tier 1 to tier 2
    auto column2 = createCuboid(
        glm::vec3(0.3f, tier2Y - tier1Y - 0.25f, 0.3f),
        stoneGray,
        glm::vec3(0.0f, tier1Y + 0.25f + (tier2Y - tier1Y - 0.25f)/2.0f, 0.0f)
    );
    fountainNode->AddChild(column2);
    
    // Column from tier 2 to tier 3
    auto column3 = createCuboid(
        glm::vec3(0.2f, tier3Y - tier2Y - 0.2f, 0.2f),
        stoneGray,
        glm::vec3(0.0f, tier2Y + 0.2f + (tier3Y - tier2Y - 0.2f)/2.0f, 0.0f)
    );
    fountainNode->AddChild(column3);
    
    // Decorative bands on columns
    for (int i = 0; i < 3; ++i) {
        float bandY = poolHeight + 0.5f + i * 0.5f;
        auto band = createCuboid(
            glm::vec3(0.45f, 0.06f, 0.45f),
            bronze,
            glm::vec3(0.0f, bandY, 0.0f)
        );
        fountainNode->AddChild(band);
    }
    
    // === WATER JETS SECTION REMOVED ===
    // Fountain now displays only the decorative structure without water effects
    
    // === ADDITIONAL DECORATIVE ELEMENTS ===
    
    // Decorative carvings on pool rim
    for (int i = 0; i < 8; ++i) {
        float angle = i * 45.0f + 22.5f; // Offset from edge ornaments
        float x = std::cos(glm::radians(angle)) * (poolRadius - 0.1f);
        float z = std::sin(glm::radians(angle)) * (poolRadius - 0.1f);
        
        auto carving = createCuboid(
            glm::vec3(0.2f, 0.08f, 0.08f),
            bronze,
            glm::vec3(x, poolHeight - 0.04f, z)
        );
        fountainNode->AddChild(carving);
    }
    
    // Decorative medallions on tier basins
    for (int tier = 0; tier < 3; ++tier) {
        float tierY = (tier == 0) ? tier1Y : (tier == 1) ? tier2Y : tier3Y;
        float tierRadius = (tier == 0) ? tier1Radius : (tier == 1) ? tier2Radius : tier3Radius;
        int numMedallions = (tier == 0) ? 4 : (tier == 1) ? 4 : 4;
        
        for (int i = 0; i < numMedallions; ++i) {
            float angle = i * (360.0f / numMedallions);
            float x = std::cos(glm::radians(angle)) * (tierRadius * 0.9f);
            float z = std::sin(glm::radians(angle)) * (tierRadius * 0.9f);
            
            auto medallion = createCuboid(
                glm::vec3(0.1f, 0.1f, 0.03f),
                bronze,
                glm::vec3(x, tierY + 0.05f, z)
            );
            fountainNode->AddChild(medallion);
        }
    }
    
    // Decorative scrollwork on tier rims
    for (int i = 0; i < 4; ++i) {
        float angle = i * 90.0f;
        
        // Tier 1 scrollwork
        float x1 = std::cos(glm::radians(angle)) * (tier1Radius + 0.15f);
        float z1 = std::sin(glm::radians(angle)) * (tier1Radius + 0.15f);
        auto scroll1 = createCuboid(
            glm::vec3(0.08f, 0.12f, 0.08f),
            bronze,
            glm::vec3(x1, tier1Y + 0.2f, z1)
        );
        fountainNode->AddChild(scroll1);
        
        // Tier 2 scrollwork
        float x2 = std::cos(glm::radians(angle)) * (tier2Radius + 0.1f);
        float z2 = std::sin(glm::radians(angle)) * (tier2Radius + 0.1f);
        auto scroll2 = createCuboid(
            glm::vec3(0.06f, 0.1f, 0.06f),
            bronze,
            glm::vec3(x2, tier2Y + 0.15f, z2)
        );
        fountainNode->AddChild(scroll2);
    }
    
    return fountainNode;
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
        glm::vec3 pavingColor(0.35f, 0.35f, 0.4f); // Darker concrete to avoid white-out
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
        
        auto centerWing = createWing(wingW, wingH, wingD, false, true,
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
                                     3,      // doorFloor: 3 = CẢ HAI TẦNG (sửa từ 2)
                                     1, 1);  // mask: 1 slot each end
        centerWing->SetLocalTransform(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -10.0f)));
        schoolParams->AddChild(centerWing);
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
                                    3,      // doorFloor: 3 = cả 2 tầng
                                    1, 3);  // Mask 1 start, 3 end (overlap with center)
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
                                     3,      // doorFloor: 3 = cả 2 tầng
                                     3, 1);  // Mask 3 start (overlap with center), 1 end
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

        // NEW: Horizontal Road near Gate (Crosses main path at Z=40 - OUTSIDE)
        auto crossPath = std::make_shared<MeshNode>(MeshType::Plane);
        crossPath->material.albedo = glm::vec3(0.2f, 0.2f, 0.22f); // Dark Asphalt
        glm::mat4 tCross = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.01f, 40.0f)); // Located at Z=40
        tCross = glm::scale(tCross, glm::vec3(100.0f, 1.0f, 10.0f)); // 100m Wide (X), 10m Deep (Z)
        crossPath->SetLocalTransform(tCross); 
        schoolParams->AddChild(crossPath);

        // Dashed Center Lines
        int numDashes = 50;
        float roadWidth = 100.0f;
        float dashLen = 1.0f;
        float gapLen = 1.0f;
        for (int i = 0; i < numDashes; ++i) {
            float x = -roadWidth/2.0f + i * (dashLen + gapLen) + 1.0f;
            auto dash = std::make_shared<MeshNode>(MeshType::Plane);
            dash->material.albedo = glm::vec3(1.0f, 1.0f, 1.0f); // White lines
            glm::mat4 tDash = glm::translate(glm::mat4(1.0f), glm::vec3(x, 0.02f, 40.0f)); 
            tDash = glm::scale(tDash, glm::vec3(dashLen, 1.0f, 0.2f)); 
            dash->SetLocalTransform(tDash);
            schoolParams->AddChild(dash);
        }

        // NEW: Streetlights along Horizontal Road
        // Place lights every 15m from -45 to 45 (100m road: [-50, 50])
        float hLightZ = 40.0f;
        float hLightHeight = 4.0f;
        
        for (float x = -45.0f; x <= 45.0f; x += 15.0f) {
            // Skip center light (X=0) to clear the gate entrance
            if (std::abs(x) < 1.0f) continue;

            // Front side of path (Z = 40 + 6.0)
            auto lightF = createStreetlight(hLightHeight);
            lightF->SetLocalTransform(glm::translate(glm::mat4(1.0f), glm::vec3(x, 0.0f, hLightZ + 6.0f)));
            schoolParams->AddChild(lightF);

            // Back side of path (Z = 40 - 6.0)
            auto lightB = createStreetlight(hLightHeight);
            glm::mat4 tB = glm::translate(glm::mat4(1.0f), glm::vec3(x, 0.0f, hLightZ - 6.0f));
            tB = glm::rotate(tB, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            lightB->SetLocalTransform(tB);
            schoolParams->AddChild(lightB);
        }
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
        int numPairs = 5; // 5 pairs = 10 lights total (extended to gate)
        float lightHeight = 4.0f;
        float spacing = 7.0f; // Distance between pairs
        
        for (int i = 0; i < numPairs; ++i)
        {
            float z = 28.0f - i * spacing; // Z positions: 28, 21, 14, 7, 0 (extends to gate at Z=30)
            
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
    
    // -- Decorative Statue --
    {
        auto statue = createStatue();
        // Position in left front corner near perimeter wall, scaled up for prominence
        glm::mat4 statueTransform = glm::mat4(1.0f);
        statueTransform = glm::translate(statueTransform, glm::vec3(-28.0f, 0.0f, 18.0f)); // Moved to corner
        statueTransform = glm::scale(statueTransform, glm::vec3(2.0f, 2.0f, 2.0f)); // Even larger (2x)
        statue->SetLocalTransform(statueTransform);
        schoolParams->AddChild(statue);
    }
    
    // -- Decorative Fountain --
    {
        auto fountain = createFountain();
        // Position in right front corner (opposite from statue) - ENLARGED
        glm::mat4 fountainTransform = glm::mat4(1.0f);
        fountainTransform = glm::translate(fountainTransform, glm::vec3(28.0f, 0.0f, 18.0f)); // Mirror position
        fountainTransform = glm::scale(fountainTransform, glm::vec3(2.5f, 2.5f, 2.5f)); // Larger: 2.5x scale
        fountain->SetLocalTransform(fountainTransform);
        schoolParams->AddChild(fountain);
    }
    
    // -- Light Control Panel --
    {
        auto controlPanel = createControlPanel();
        // Position near gate, on the right side (facing outward)
        glm::mat4 panelTransform = glm::mat4(1.0f);
        panelTransform = glm::translate(panelTransform, glm::vec3(5.0f, 0.0f, 22.0f)); // Right of gate
        // No rotation - facing outward toward camera
        controlPanel->SetLocalTransform(panelTransform);
        schoolParams->AddChild(controlPanel);
    }
    
    // -- FUNCTIONAL SCHOOL GATE --
    {
        // Place Gates at Z = 30.0 (Inside Parabolic Arch)
        float gateWidth = 5.0f; // Resize smaller to fit in 12m arch
        float gateHeight = 3.0f;
        
        // Hinge Nodes (to pivot around edges)
        
        // Left Gate Hinge
        auto leftHinge = std::make_shared<SceneNode>();
        // Position: X = -5.0, Z = 30.0 (Fit inside arch width 12)
        leftHinge->SetLocalTransform(glm::translate(glm::mat4(1.0f), glm::vec3(-5.0f, 0.0f, 30.0f)));
        
        auto leftGate = createIronGate(gateWidth, gateHeight);
        // Offset gate so pivot is at edge (gate goes from 0 to 5)
        leftGate->SetLocalTransform(glm::translate(glm::mat4(1.0f), glm::vec3(gateWidth/2.0f, 0, 0)));
        
        leftHinge->AddChild(leftGate); 
        schoolParams->AddChild(leftHinge);
        
        s_schoolGateLeft = leftHinge;
        
        // Right Gate Hinge
        auto rightHinge = std::make_shared<SceneNode>();
        // Position: X = 5.0, Z = 30.0
        rightHinge->SetLocalTransform(glm::translate(glm::mat4(1.0f), glm::vec3(5.0f, 0.0f, 30.0f)));
        
        auto rightGate = createIronGate(gateWidth, gateHeight);
        // Offset gate so pivot is at edge (gate goes from -5 to 0)
        rightGate->SetLocalTransform(glm::translate(glm::mat4(1.0f), glm::vec3(-gateWidth/2.0f, 0, 0)));
        
        rightHinge->AddChild(rightGate);
        schoolParams->AddChild(rightHinge);
        
        s_schoolGateRight = rightHinge;
        
        // -- LEVER --
        // Position OUTSIDE (Z > 30)
        auto lever = createLeverObj();
        lever->SetLocalTransform(glm::translate(glm::mat4(1.0f), glm::vec3(-8.0f, 0.0f, 32.0f)));
        schoolParams->AddChild(lever);
        
        s_gateLever = lever;
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

    // -- Animated Cars --
    {
        s_cars.clear();
        
        // Car 1: Red, Lane 1 (Z=37.5), Moving Right (+X)
        auto car1 = createCar(glm::vec3(0.9f, 0.1f, 0.1f));
        glm::mat4 t1 = glm::translate(glm::mat4(1.0f), glm::vec3(-40.0f, 0.0f, 37.5f));
        car1->SetLocalTransform(t1);
        schoolParams->AddChild(car1);
        s_cars.push_back({car1, 8.0f, -60.0f, 60.0f, -40.0f, 1}); // Speed 8 m/s, Limit +/- 60
        
        // Car 2: Blue, Lane 2 (Z=42.5), Moving Left (-X)
        auto car2 = createCar(glm::vec3(0.1f, 0.3f, 0.9f));
        glm::mat4 t2 = glm::translate(glm::mat4(1.0f), glm::vec3(40.0f, 0.0f, 42.5f));
        t2 = glm::rotate(t2, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Face Left
        car2->SetLocalTransform(t2);
        schoolParams->AddChild(car2);
        s_cars.push_back({car2, 10.0f, 60.0f, -60.0f, 40.0f, -1}); // Speed 10 m/s, Limit +/- 60
        
        // Car 3: Yellow, Lane 1 (Z=37.5), Moving Right (+X) - Delayed start
        auto car3 = createCar(glm::vec3(0.9f, 0.8f, 0.1f));
        glm::mat4 t3 = glm::translate(glm::mat4(1.0f), glm::vec3(-10.0f, 0.0f, 37.5f));
        car3->SetLocalTransform(t3);
        schoolParams->AddChild(car3);
        s_cars.push_back({car3, 7.0f, -60.0f, 60.0f, -10.0f, 1}); // Speed 7 m/s, Limit +/- 60
    }

    return root;
}

// Static vector to store people for animation
std::vector<SceneNode::Ptr> SchoolBuilder::s_people;

// Static clock for animation
SceneNode::Ptr SchoolBuilder::s_clock;

// Static clouds for animation
std::vector<SceneNode::Ptr> SchoolBuilder::s_clouds;

// Static birds for animation
std::vector<SceneNode::Ptr> SchoolBuilder::s_birds;

// Static flag parts
std::vector<SchoolBuilder::FlagPart> SchoolBuilder::s_flagParts;



// Interactive Door System
std::vector<SchoolBuilder::Door> SchoolBuilder::s_doors;

// Gate & Lever Static Definitions
SceneNode::Ptr SchoolBuilder::s_schoolGateLeft = nullptr;
SceneNode::Ptr SchoolBuilder::s_schoolGateRight = nullptr;
SceneNode::Ptr SchoolBuilder::s_gateLever = nullptr;
bool SchoolBuilder::s_isGateOpen = false;


// Update bird animation
void SchoolBuilder::updateBirdAnimation(SceneNode::Ptr root, float time)
{
    float speed = 2.0f;
    float radius = 15.0f;
    float height = 12.0f;
    
    for (size_t i = 0; i < s_birds.size(); ++i)
    {
        auto bird = s_birds[i];
        
        // Circular motion
        float angle = time * 0.5f + i * (6.28f / s_birds.size());
        float x = std::sin(angle) * radius;
        float z = std::cos(angle) * radius;
        float y = height + std::sin(time * 1.5f + i) * 1.0f; // Bob up and down
        
        // Orientation (face direction of movement)
        // Tangent to circle is (-z, x)
        float rotAngle = angle - 1.57f; // -90 degrees
        
        glm::mat4 t(1.0f);
        t = glm::translate(t, glm::vec3(x, y, z));
        t = glm::rotate(t, rotAngle, glm::vec3(0.0f, 1.0f, 0.0f));
        t = glm::scale(t, glm::vec3(0.5f)); // Scale down the bird
        
        bird->SetLocalTransform(t);
        
        // Wing flapping
        if (bird->children.size() >= 2) {
            float flap = std::sin(time * 15.0f);
            float wingAngle = flap * 0.5f; // +/- 0.5 radians (approx 30 deg)
            
            // Left wing (child 1)
            auto leftWing = bird->children[1];
            glm::mat4 lw(1.0f);
            lw = glm::translate(lw, glm::vec3(-0.2f, 0.0f, 0.0f));
            lw = glm::rotate(lw, wingAngle, glm::vec3(0.0f, 0.0f, 1.0f));
            lw = glm::translate(lw, glm::vec3(-0.5f, 0.0f, 0.0f)); // Pivot correction
            lw = glm::scale(lw, glm::vec3(1.0f, 0.1f, 0.5f));
            leftWing->SetLocalTransform(lw);
            
            // Right wing (child 2)
            auto rightWing = bird->children[2];
            glm::mat4 rw(1.0f);
            rw = glm::translate(rw, glm::vec3(0.2f, 0.0f, 0.0f));
            rw = glm::rotate(rw, -wingAngle, glm::vec3(0.0f, 0.0f, 1.0f));
            rw = glm::translate(rw, glm::vec3(0.5f, 0.0f, 0.0f)); // Pivot correction
            rw = glm::scale(rw, glm::vec3(1.0f, 0.1f, 0.5f));
            rightWing->SetLocalTransform(rw);
        }
    }
}

// Update flag animation
void SchoolBuilder::updateFlagAnimation(SceneNode::Ptr root, float time)
{
    float amplitude = 0.25f; // Wave height
    float frequency = 2.0f;  // Wave density
    float speed = 5.0f;      // Wave speed
    
    for (auto& part : SchoolBuilder::s_flagParts)
    {
        // Calculate wave Z offset based on X position
        // Wave moves away from pole: (x * freq - time * speed)
        float z = amplitude * std::sin(part.xOffset * frequency - time * speed);
        
        // Create translation matrix for the offset
        glm::mat4 offsetMat = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, z));
        
        // Apply offset to initial transform
        // We apply offset * initial because we want to perturb the existing position
        // Note: initialTransform is Local to the flagpole
        part.node->SetLocalTransform(offsetMat * part.initialTransform);
    }
}



// Update Gate Animation
void SchoolBuilder::updateGateAnimation(float deltaTime) {
    float openAngle = glm::radians(90.0f);
    float speed = 2.0f * deltaTime;
    
    // Interpolate hinge rotations
    if (s_schoolGateLeft && s_schoolGateRight) {
        // We need persistence, but static variables in function are risky if re-created.
        // However, SceneNodes persist. We can read their current Rotation?
        // Let's rely on a helper static float that tracks state.
        static float currentGateAngle = 0.0f;
        
        float target = s_isGateOpen ? openAngle : 0.0f;
        
        if (std::abs(currentGateAngle - target) > 0.001f) {
            float dir = (target > currentGateAngle) ? 1.0f : -1.0f;
            currentGateAngle += dir * speed;
            
            // Clamp
            if ((dir > 0 && currentGateAngle > target) || (dir < 0 && currentGateAngle < target))
                currentGateAngle = target;
                
            // Apply rotations
            // Left Hinge: rotates CW negative? No, opens inward (Z-) or outward (Z+)?
            // Player starts at Z=30 (Outside). Gate at Z=20.
            // "Inward" is Z-. "Outward" is Z+.
            // Let's open Inward (towards school).
            // Left hinge (at X=-7): needs to rotate CW (negative Y rot) to swing in?
            // Right hinge (at X=7): needs to rotate CCW (positive Y rot) to swing in?
            
            // Wait, standard coordinates:
            // X+ Right, Z+ Towards Viewer (Outside). Z- Inside.
            // Left Gate (X=-7): If we rotate +90 (CCW), it swings into Z+. (Out).
            // If we rotate -90 (CW), it swings into Z-. (In).
            
            // So Left -> -Angle. Right -> +Angle.
            
            // But we need to preserve position. SetLocalTransform overwrites.
            // Reconstruct Transform: Translate + Rotate
            
            // Left Hinge at X=-5.0, Z=30.0
            glm::mat4 leftT = glm::translate(glm::mat4(1.0f), glm::vec3(-5.0f, 0.0f, 30.0f));
            leftT = glm::rotate(leftT, -currentGateAngle, glm::vec3(0,1,0));
            s_schoolGateLeft->SetLocalTransform(leftT);
            
            // Right Hinge at X=5.0, Z=30.0
            glm::mat4 rightT = glm::translate(glm::mat4(1.0f), glm::vec3(5.0f, 0.0f, 30.0f));
            rightT = glm::rotate(rightT, currentGateAngle, glm::vec3(0,1,0));
            s_schoolGateRight->SetLocalTransform(rightT);
        }
    }
    
    // Animate Lever
    if (s_gateLever && s_gateLever->children.size() > 1) {
        auto handle = s_gateLever->children[1];
        float leverTarget = s_isGateOpen ? glm::radians(45.0f) : glm::radians(-45.0f);
        static float currentLever = glm::radians(-45.0f);
        
        float lSpeed = 5.0f * deltaTime;
        if (std::abs(currentLever - leverTarget) > 0.01f) {
            float dir = (leverTarget > currentLever) ? 1.0f : -1.0f;
            currentLever += dir * lSpeed;
             if ((dir > 0 && currentLever > leverTarget) || (dir < 0 && currentLever < leverTarget))
                currentLever = leverTarget;
            
            // Pivot is at (0, 0.1, 0)
            glm::mat4 hT = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0.1f, 0));
            hT = glm::rotate(hT, currentLever, glm::vec3(1,0,0)); // Rotate X
            handle->SetLocalTransform(hT);
        }
    }
}


    
// Update people animation
void SchoolBuilder::updatePeopleAnimation(SceneNode::Ptr root, float time)
{
    if (s_people.empty()) return;
    
    // Helper to animate limbs (arms and legs swing)
    // direction: 1.0 = forward, -1.0 = backward
    auto animateLimbs = [](SceneNode::Ptr person, float walkCycle, float direction = 1.0f) {
        if (person->children.size() < 7) return; // Need all limbs
        
        // Walking cycle animation
        float armSwing = std::sin(walkCycle * 2.0f) * 30.0f; // Arms swing opposite to legs
        float legSwing = std::sin(walkCycle * 2.0f) * 25.0f * direction; // Legs swing (reversed when backward)
        
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
        bool movingForward = (std::sin(time * walkSpeed) > 0);
        float angle = movingForward ? 45.0f : -135.0f;
        float direction = movingForward ? 1.0f : -1.0f;
        glm::mat4 t = glm::translate(glm::mat4(1.0f), glm::vec3(x, 0.0f, 10.0f));
        t = glm::rotate(t, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
        s_people[0]->SetLocalTransform(t);
        animateLimbs(s_people[0], time * walkSpeed, direction);
    }
    
    // Person 2: Walk back and forth on right pathway (Z direction)
    if (s_people.size() > 1)
    {
        float walkSpeed = 0.6f;
        float z = 12.0f + std::sin(time * walkSpeed) * 4.0f;
        bool movingForward = (std::sin(time * walkSpeed) > 0);
        float angle = movingForward ? 0.0f : 180.0f;
        float direction = movingForward ? 1.0f : -1.0f;
        glm::mat4 t = glm::translate(glm::mat4(1.0f), glm::vec3(6.0f, 0.0f, z));
        t = glm::rotate(t, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
        s_people[1]->SetLocalTransform(t);
        animateLimbs(s_people[1], time * walkSpeed, direction);
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
        animateLimbs(s_people[2], time * walkSpeed, 1.0f); // Always forward for circular
    }
    
    // Person 4: Walk back and forth near football field
    if (s_people.size() > 3)
    {
        float walkSpeed = 0.55f;
        float z = -8.0f + std::sin(time * walkSpeed) * 5.0f;
        bool movingForward = (std::sin(time * walkSpeed) > 0);
        float angle = movingForward ? 0.0f : 180.0f;
        float direction = movingForward ? 1.0f : -1.0f;
        glm::mat4 t = glm::translate(glm::mat4(1.0f), glm::vec3(22.0f, 0.0f, z));
        t = glm::rotate(t, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
        s_people[3]->SetLocalTransform(t);
        animateLimbs(s_people[3], time * walkSpeed, direction);
    }
    
    // Person 5: Stay at picnic table (no movement, no animation)
    
    // Person 6: Walk near entrance (X direction)
    if (s_people.size() > 5)
    {
        float walkSpeed = 0.45f;
        float x = 2.0f + std::sin(time * walkSpeed) * 4.0f;
        bool movingForward = (std::sin(time * walkSpeed) > 0);
        float angle = movingForward ? 90.0f : -90.0f;
        float direction = movingForward ? 1.0f : -1.0f;
        glm::mat4 t = glm::translate(glm::mat4(1.0f), glm::vec3(x, 0.0f, 20.0f));
        t = glm::rotate(t, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
        s_people[5]->SetLocalTransform(t);
        animateLimbs(s_people[5], time * walkSpeed, direction);
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


// Car Animation Definition
std::vector<SchoolBuilder::Car> SchoolBuilder::s_cars;

void SchoolBuilder::updateCarAnimation(float dt)
{
    for (auto& car : s_cars)
    {
        car.currentX += car.speed * car.direction * dt;
        
        // Loop logic
        if (car.direction == 1 && car.currentX > car.endX) {
             car.currentX = car.startX;
        } else if (car.direction == -1 && car.currentX < car.endX) {
             car.currentX = car.startX;
        }
        
        // Update Transform
        glm::mat4 t = glm::mat4(1.0f);
        // Base position on the road
        float roadZ = (car.direction == 1) ? 37.5f : 42.5f;
        
        t = glm::translate(t, glm::vec3(car.currentX, 0.0f, roadZ));
        if (car.direction == -1) {
            t = glm::rotate(t, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        }
        car.node->SetLocalTransform(t);
        
        // --- Animate Wheels (Spinning) ---
        // Wheels are children indices 3, 4, 5, 6
        // Wheel Radius = 0.4. Circumference = 2*PI*0.4 ≈ 2.51
        // Angle = Distance / Radius (in radians)
        // Distance traveled = currentX (relative to some start)
        // Spin direction depends on car direction.
        // If moving +X (Forward), wheels roll forward.
        // In local Car space, Forward is +X.
        // Cylinder Axle is local Z.
        // Rolling forward means rotating around Z axis?
        // Let's verify: RotX(90) transformed cylinder.
        // Cylinder Y -> World Z. Cylinder X -> World X. Cylinder Z -> World -Y.
        // So in Cylinder Local space:
        // We want to rotate around Cylinder Y axis (World Z) to spin axis? No.
        // We want to rotate around World Z axis.
        // In Local Car Space, coordinates are X, Y, Z.
        // Wheel is at (x, 0.4, z).
        // To roll along X, it must rotate around Z axis.
        // Angle = -currentX / 0.4f (Negative for +X motion?)
        // Let's rely on standard: Rotate(angle, vec3(0,0,1)).
        
        float wheelRadius = 0.4f;
        // Calculate angle based on absolute X position for continuous rolling
        // Note: car.direction affects how "forward" maps to world X, but wheel spin should match movement.
        // If car moves +X, wheels spin negative Z? (Right Hand Rule: Thumb +Z, fingers curl X->Y)
        // Rolling forward (+X) means top goes +X, bottom goes -X?
        // Actually, let's just use angle = -currentX / radius.
        // If Car is flipped 180 (for -X movement), its local Forward is +X in its frame?
        // Yes, we rotated the whole car 180. So car local +X is world -X.
        // So we can always use local rotation relative to car movement.
        // Car moves speed * dt * direction.
        // Total distance? currentX is world position.
        // If we use currentX, we get absolute rotation phase.
        
        float spinAngle = -car.currentX / wheelRadius;
        
        // We need to fetch the wheels and apply transform
        // Indices 3, 4, 5, 6.
        if (car.node->children.size() >= 7) {
             // Offsets for wheels (from createCar)
             float wheelX[] = {1.5f, 1.5f, -1.5f, -1.5f};
             float wheelZ[] = {1.0f, -1.0f, 1.0f, -1.0f};
             
             for (int i = 0; i < 4; ++i) {
                 auto wheel = car.node->children[3 + i];
                 
                 // Reconstruct Wheel Transform
                 glm::mat4 wt(1.0f);
                 wt = glm::translate(wt, glm::vec3(wheelX[i], 0.4f, wheelZ[i])); // Position
                 
                 // Rotate around Z axis for Spin
                 wt = glm::rotate(wt, spinAngle, glm::vec3(0, 0, 1));
                 
                 // Original Orientation: Rotate 90 deg around X axis
                 wt = glm::rotate(wt, glm::radians(90.0f), glm::vec3(1, 0, 0));
                 
                 // Scale
                 wt = glm::scale(wt, glm::vec3(0.8f, 0.4f, 0.8f));
                 
                 wheel->SetLocalTransform(wt);
             }
        }
    }
}
