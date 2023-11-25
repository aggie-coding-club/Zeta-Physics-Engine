// #include <zeta/physicshandler.h>
#include "app.h"
#include "text.cpp"
#include "ui.cpp"

#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "shader.h"
#include "entity.h"
#include "renderer.h"
#include "scene.h"

float global_dt = 0.0f;
HMM_Vec3 cursor_position = {};
RendererData global_rd = {};
Scene::SceneManager global_sm = {};
Scene::Scene *current_scene = 0;

template <typename Out>
void SplitString(const std::string &s, char delim, Out result) {
    std::istringstream iss(s);
    std::string item;
    while (std::getline(iss, item, delim)) {
        *result++ = item;
    }
}

std::vector<std::string> SplitString(const std::string &s, char delim) {
    std::vector<std::string> elems;
    SplitString(s, delim, std::back_inserter(elems));
    return elems;
}


// makes sure all the normals and vertex data are lined up together
void ProcessVertex(std::vector<std::string> vertexData, std::vector<int> *indices, std::vector<HMM_Vec3> *textures,
    std::vector<HMM_Vec3> *normals, std::vector<float> *texturesArray, std::vector<float> *normalsArray, float textureIndex){
    
    int currentVertexPointer = (int)std::stof(vertexData[0]) - 1;
    indices->push_back(currentVertexPointer);

    HMM_Vec3 currentTex = textures->at((int)std::stof(vertexData[1]) - 1);

    texturesArray->at(currentVertexPointer * 3) = currentTex.X;
    texturesArray->at(currentVertexPointer * 3 + 1) = 1 - currentTex.Y;
    texturesArray->at(currentVertexPointer * 3 + 2) = textureIndex;

    HMM_Vec3 currentNormal = normals->at((int)std::stof(vertexData[2]) - 1);

    normalsArray->at(currentVertexPointer * 3) = currentNormal.X;
    normalsArray->at(currentVertexPointer * 3 + 1) = currentNormal.Y;
    normalsArray->at(currentVertexPointer * 3 + 2) = currentNormal.Z;
}

RawModel load_obj_model(std::string fileName, HMM_Vec4 color){
    // NOTE (Lenny) : Check for invalid files

    #if __EMSCRIPTEN__
    std::ifstream objFile("vendor/" + fileName);
    #else
    std::ifstream objFile(fileName);
    #endif

    if(!objFile){
        printf("Error loading obj file -> \n", &fileName[0]);
    }
    std::string objData;

    std::string objLine;

    std::vector<HMM_Vec3> vertices;
    std::vector<HMM_Vec3> textures; // tex coords
    std::vector<HMM_Vec3> normals;

    std::vector<int> indicesArray;
    std::vector<float> verticesArray;
    std::vector<float> normalsArray;
    std::vector<float> texturesArray;
    std::vector<float> colorsArray;

    int index = 0;
    // Use a while loop together with the getline() function to read the file line by  line
    while (std::getline (objFile, objLine)) {
        std::vector<std::string> s = SplitString(objLine, ' ');
            
        if(s[0] == "v"){ // vertex position
            HMM_Vec3 vertex = {std::stof(s[1]), std::stof(s[2]), std::stof(s[3])};
            vertices.push_back(vertex);
            
            // NOTE(Lenny) : Inefficient....BAD CODE...REWRITE
            texturesArray.push_back(0);
            texturesArray.push_back(0);
            texturesArray.push_back(0);
            
            normalsArray.push_back(0);
            normalsArray.push_back(0);
            normalsArray.push_back(0);

        } else if (s[0] == "vt"){ // texture coord
            HMM_Vec3 textureCoord = {std::stof(s[1]), std::stof(s[2]), 0.0};
            textures.push_back(textureCoord);
            
        } else if(s[0] == "vn"){ // vertex normal
            // HMM_Vec3 normal = {std::stof(s[1]), std::stof(s[2]), std::stof(s[3])};
            HMM_Vec3 normal = {};

            normal = {std::stof(s[1]), std::stof(s[2]), std::stof(s[3])};
            // normal = {1.0f, 0.0f, 0.0f};

            normals.push_back(normal);
            index++;
            // printf("x : %f, y : %f, z : %f", normal.x, normal.y, normal.z);
        
        } else if(s[0] == "f"){ // indicies
            // for(int i = 0; i < s.size(); i++){
            //     PrintString(s[i]);
            // }
            
            // printf("\n"); 
            std::vector<std::string> vertex1 = SplitString(s[1], '/');
            std::vector<std::string> vertex2 = SplitString(s[2], '/');
            std::vector<std::string> vertex3 = SplitString(s[3], '/');

            ProcessVertex(vertex1, &indicesArray, &textures, &normals, &texturesArray, &normalsArray, 0);
            ProcessVertex(vertex2, &indicesArray, &textures, &normals, &texturesArray, &normalsArray, 0);
            ProcessVertex(vertex3, &indicesArray, &textures, &normals, &texturesArray, &normalsArray, 0);
            // break;
        } 
    };

    // Close the file
    objFile.close();

    index = 0;
    for(HMM_Vec3 vertex:vertices){
        verticesArray.push_back(vertex.X);
        verticesArray.push_back(vertex.Y);
        verticesArray.push_back(vertex.Z);

        colorsArray.push_back(color.X);
        colorsArray.push_back(color.Y);
        colorsArray.push_back(color.Z);
    }

    VertexData vertexData = {};
    vertexData.normals = &normalsArray[0];
    vertexData.positions = &verticesArray[0];
    vertexData.colors = &colorsArray[0];
    vertexData.len_colors = colorsArray.size();
    vertexData.len_normals = normalsArray.size();
    vertexData.len_positions = verticesArray.size();

    vertexData.indices = &indicesArray[0];
    vertexData.len_indices = indicesArray.size();
    vertexData.tex_coords = &texturesArray[0];
    vertexData.len_tex_coords = texturesArray.size();

    RawModel result = load_to_VAO(&global_rd, &vertexData);

    return result;
}

RawModel load_obj_model(std::string fileName, HMM_Vec4 color, int texturesCount){
    // NOTE (Lenny) : Check for invalid files

    #if __EMSCRIPTEN__
    std::ifstream objFile("vendor/" + fileName);
    #else
    std::ifstream objFile(fileName);
    #endif

    if(!objFile){
        printf("Error loading obj file -> \n", &fileName[0]);
    }
    std::string objData;

    std::string objLine;

    std::vector<HMM_Vec3> vertices;
    std::vector<HMM_Vec3> textures; // tex coords
    std::vector<HMM_Vec3> normals;

    std::vector<int> indicesArray;
    std::vector<float> verticesArray;
    std::vector<float> normalsArray;
    std::vector<float> texturesArray;
    std::vector<float> colorsArray;

    int textureIndex = 0;
    int index = 0;
    // Use a while loop together with the getline() function to read the file line by  line
    while (std::getline (objFile, objLine)) {
        std::vector<std::string> s = SplitString(objLine, ' ');
            
        if(s[0] == "v"){ // vertex position
            HMM_Vec3 vertex = {std::stof(s[1]), std::stof(s[2]), std::stof(s[3])};
            vertices.push_back(vertex);
            
            // NOTE(Lenny) : Inefficient....BAD CODE...REWRITE
            texturesArray.push_back(0);
            texturesArray.push_back(0);
            texturesArray.push_back(0);
            
            normalsArray.push_back(0);
            normalsArray.push_back(0);
            normalsArray.push_back(0);

        } else if (s[0] == "vt"){ // texture coord
            HMM_Vec3 textureCoord = {std::stof(s[1]), std::stof(s[2]), (float)textureIndex};
            textures.push_back(textureCoord);
            
        } else if(s[0] == "vn"){ // vertex normal
            // HMM_Vec3 normal = {std::stof(s[1]), std::stof(s[2]), std::stof(s[3])};
            HMM_Vec3 normal = {};

            normal = {std::stof(s[1]), std::stof(s[2]), std::stof(s[3])};
            // normal = {1.0f, 0.0f, 0.0f};

            normals.push_back(normal);
            index++;
            // printf("x : %f, y : %f, z : %f", normal.x, normal.y, normal.z);
        
        } else if(s[0] == "f"){ // indicies
            // for(int i = 0; i < s.size(); i++){
            //     PrintString(s[i]);
            // }
            
            // printf("\n"); 
            std::vector<std::string> vertex1 = SplitString(s[1], '/');
            std::vector<std::string> vertex2 = SplitString(s[2], '/');
            std::vector<std::string> vertex3 = SplitString(s[3], '/');

            ProcessVertex(vertex1, &indicesArray, &textures, &normals, &texturesArray, &normalsArray, textureIndex - 1);
            ProcessVertex(vertex2, &indicesArray, &textures, &normals, &texturesArray, &normalsArray, textureIndex - 1);
            ProcessVertex(vertex3, &indicesArray, &textures, &normals, &texturesArray, &normalsArray, textureIndex - 1);
            // break;
        } else if(s[0] == "usemtl"){
            textureIndex++;
        }
    };

    // Close the file
    objFile.close();

    index = 0;
    for(HMM_Vec3 vertex:vertices){
        verticesArray.push_back(vertex.X);
        verticesArray.push_back(vertex.Y);
        verticesArray.push_back(vertex.Z);

        colorsArray.push_back(color.X);
        colorsArray.push_back(color.Y);
        colorsArray.push_back(color.Z);
    }

    VertexData vertexData = {};
    vertexData.normals = &normalsArray[0];
    vertexData.positions = &verticesArray[0];
    vertexData.colors = &colorsArray[0];
    vertexData.len_colors = colorsArray.size();
    vertexData.len_normals = normalsArray.size();
    vertexData.len_positions = verticesArray.size();

    vertexData.indices = &indicesArray[0];
    vertexData.len_indices = indicesArray.size();
    vertexData.tex_coords = &texturesArray[0];
    vertexData.len_tex_coords = texturesArray.size();

    RawModel result = load_to_VAO(&global_rd, &vertexData);


    return result;
}

Zeta::Handler handler(ZMath::Vec3D(0, -5.8f, 0));

float time_btw_physics_updates = 1.0f / 60.0f;
float count_down = time_btw_physics_updates;
float start_time = (float)glfwGetTime();

Scene::Scene *gravity_scene = {};

RawModel model = {};
RawModel ground_model = {};
E_::Entity *test_entity = 0;
E_::Entity *light_entity = 0;
E_::Entity *ground_entity = 0;
E_::Entity *dragon_entity = 0;
E_::Entity *stall_entity = 0;
E_::Entity *test_cube_entity = 0;
E_::Entity *pine_5_entity = 0;
E_::Entity *birch_10_entity = 0;
E_::Entity *debug_xmover_entity = 0;
E_::Entity *debug_ymover_entity = 0;
E_::Entity *debug_zmover_entity = 0;

E_::EntityManager em = {};

// HMM_Mat4 projection;
// HMM_Mat4 view_matrix;

HMM_Vec4 mouse_ndc = {};

Camera camera = {};
// float projection_fov = DEFAULT_FOV;
bool first_mouse = 1;
float last_mouse_x = 0.0f;
float last_mouse_y = 0.0f;
float g_editor_mode = 0;

TexturesManager textures_manager;
TextRendererManager trm = {};
InputManager global_im = {};

void SetCursorPosition(float x, float y){
    cursor_position.X = x;
    cursor_position.Y = y;
    global_im.cursorX = x;
    global_im.cursorY = y;

    if(!g_editor_mode){
        if(first_mouse){
            last_mouse_x = cursor_position.X;
            last_mouse_y = cursor_position.Y;
            first_mouse = false;
        }

        float x_offset = cursor_position.X - last_mouse_x;
        float y_offset = cursor_position.Y - last_mouse_y;

        last_mouse_x = cursor_position.X;
        last_mouse_y = cursor_position.Y;

        float sensitivity = 0.1f;
        x_offset *= sensitivity;
        y_offset *= sensitivity;

        camera.yaw += x_offset;
        camera.pitch -= y_offset;
        
        // prevent vertical flipping
        if(camera.pitch > 89.0f){
            camera.pitch = 89.0f;
        }

        if(camera.pitch < -89.0f){
            camera.pitch = -89.0f;
        }

        HMM_Vec3 camera_direction = {};
        camera_direction.X = HMM_CosF(HMM_DegToRad * camera.yaw) * HMM_CosF(HMM_DegToRad * camera.pitch);
        camera_direction.Y =  HMM_SinF(HMM_DegToRad * camera.pitch);
        camera_direction.Z = HMM_SinF(HMM_DegToRad * camera.yaw) * HMM_CosF(HMM_DegToRad * camera.pitch);

        camera.front = HMM_Norm(camera_direction);
    }
}

void SetScroll(float x_offset, float y_offset){
    global_rd.projection_fov -= (float)y_offset;
    if(global_rd.projection_fov < 1.0f){
        global_rd.projection_fov = 1.0f;
    }

    if(global_rd.projection_fov > DEFAULT_FOV){
        global_rd.projection_fov = DEFAULT_FOV;
    }
}

void SetEditMode(int mode){
    g_editor_mode = mode;

    if(g_editor_mode){
        ShowCursor(WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f);
    }else{
        HideCursor(last_mouse_x, last_mouse_y);
    }   
}

void TempLightMovement(int key, int state);

// TODO: not smooth
void GameInputCamera(int key, int state){

    float temp_speed = camera.speed * global_dt;
    if(state != GLFW_PRESS)
        return;

    if (key == GLFW_KEY_ESCAPE){
        printf("escaping\n");
        SetEditMode(!g_editor_mode);
    }
    
    if(!g_editor_mode){  
        // hide cursor
        if (key == GLFW_KEY_D){
            camera.position += HMM_Norm(HMM_Cross(camera.front, camera.world_up)) * temp_speed;
        }

        if (key == GLFW_KEY_A){
            camera.position -= HMM_Norm(HMM_Cross(camera.front, camera.world_up)) * temp_speed; 
        }

        if (key == GLFW_KEY_W){
            camera.position += camera.front * temp_speed;
        }

        if (key == GLFW_KEY_S){
            camera.position -= camera.front * temp_speed;
        }


        TempLightMovement(key, state);
    }
}

void GameInputMouse(int button, int action){
    if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS){
        global_im.left_click = true;

        global_im.left_press = true;
        global_im.left_release = false;
    }if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE){
        global_im.left_click = false;
        
        global_im.left_release = true;
        global_im.left_press = false;
    }
}

void TempLightMovement(int key, int state){
    if(key == GLFW_KEY_LEFT){
        printf("right,...\n");
        light_entity->sb->pos.x += 1000 * global_dt;
    } else if(key == GLFW_KEY_RIGHT){
        printf("left,...\n");
        light_entity->sb->pos.x -= 1000 * global_dt;
    }else if(key == GLFW_KEY_UP){    
        printf("right,...\n");
        light_entity->sb->pos.y += 1000 * global_dt;
    } else if(key == GLFW_KEY_DOWN){
        printf("left,...\n");
        light_entity->sb->pos.y -= 1000 * global_dt;
    }
}

void test_entity_physics_behavior(E_::Entity *entity, float &time_step, int physics_updates){
    if(entity == test_entity){
        ZMath::Vec3D normal = {};
        float ground_cube_colliding = Zeta::CubeAndCube(*((Zeta::Cube *)(entity->rb->collider)), *((Zeta::Cube *)ground_entity->sb->collider), normal);
        
        if(ground_cube_colliding){
            for(int i = 0; i < physics_updates; i++){
                test_entity->rb->vel = 0;
                test_entity->rb->netForce -= handler.g * test_entity->rb->mass;
            }
            ground_entity->color = HMM_Vec4{1.0f, 1.0f, 0.0f, 1.0f};
        }else{
            ground_entity->color = HMM_Vec4{1.0f, 1.0f, 1.0f, 1.0f};
        }
    }
}

void app_start(void *window){
    SetEditMode(0);

    printf("Program Started\n");
    textures_manager = TexturesManager();
    global_rd.projection_fov = DEFAULT_FOV;

    // >>>>>> Shader Stuff
    // =====================================
    global_rd.picker_shader.program = load_shaders((char *)"shaders/picker_shader_vs.glsl", (char *)"shaders/picker_shader_fs.glsl");
    global_rd.main_shader.program = load_shaders((char *)"shaders/web_v_shader.glsl", (char *)"shaders/web_f_shader.glsl");
    global_rd.shadow_map_shader.program = load_shaders((char *)"shaders/shadow_map_vs.glsl", (char *)"shaders/shadow_map_fs.glsl"); 

    glUseProgram(global_rd.main_shader.program);

    unsigned int u_specular_strength = get_uniform_location(&global_rd.main_shader, (char *)"specular_strength");
    unsigned int u_reflectivity = get_uniform_location(&global_rd.main_shader, (char *)"reflectivity");
    
    set_uniform_value(u_specular_strength, 0.25f);
    set_uniform_value(u_reflectivity, 64.0f);
    
    glUseProgram(0);
    
    // >>>>>> Texture Stuff
    // =====================================
    textures_manager.add_texture((char *)"white.png", TEXTURE_WHITE, TEX_FORMAT_PNG);
    textures_manager.add_texture((char *)"thin/stallTexture.png", TEXTURE_STALL, TEX_FORMAT_PNG); 
    // =====================================
    camera.speed = 10000.0f;
    camera.position.X = -29.0; 
    camera.position.Y = 21.0; 
    camera.position.Z = -53.0;

    camera.world_up = {0.0f, 1.0f, 0.0f};
    camera.front = {0.0f, 0.0f, -1.0f};
    
    // >>>>>> Entity Stufff
    // ========================================
    ZMath::Vec3D debug_movers_halfsize = {8.0f, 0.5f, 0.5f};
    debug_xmover_entity = create_entity(&em, HMM_Vec3{0, 30.0f, -20.0f}, 1.0f, 0.0f, 0.0f, 0.0f, 
        Zeta::StaticBodyCollider::STATIC_CUBE_COLLIDER, new Zeta::Cube(-debug_movers_halfsize, {debug_movers_halfsize.x, debug_movers_halfsize.y, debug_movers_halfsize.z}, 0.0f, 0.0f));
    debug_xmover_entity->color = {1.0f, 0.0, 0.0f};
    debug_xmover_entity->def_texture = TEXTURE_WHITE;

    debug_ymover_entity = create_entity(&em, HMM_Vec3{0, 30.0f, -20.0f}, 1.0f, 0.0f, 0.0f, 0.0f, 
        Zeta::StaticBodyCollider::STATIC_CUBE_COLLIDER, new Zeta::Cube(-ZMath::Vec3D{debug_movers_halfsize.y, debug_movers_halfsize.x, debug_movers_halfsize.z}, {debug_movers_halfsize.y, debug_movers_halfsize.x, debug_movers_halfsize.z}, 0.0f, 0.0f));
    debug_ymover_entity->color = {0.0f, 1.0, 0.0f};
    debug_ymover_entity->def_texture = TEXTURE_WHITE;

    debug_zmover_entity = create_entity(&em, HMM_Vec3{0, 30.0f, -20.0f}, 1.0f, 0.0f, 0.0f, 0.0f, 
        Zeta::StaticBodyCollider::STATIC_CUBE_COLLIDER, new Zeta::Cube(-ZMath::Vec3D{debug_movers_halfsize.y, debug_movers_halfsize.z, debug_movers_halfsize.x}, {debug_movers_halfsize.y, debug_movers_halfsize.z, debug_movers_halfsize.x}, 0.0f, 0.0f));
    debug_zmover_entity->color = {0.0f, 0.0, 1.0f};
    debug_zmover_entity->def_texture = TEXTURE_WHITE;

    test_entity = E_::create_entity(&em, HMM_Vec3{0, 48, -20.0f}, 1.0f, 0.0f, 0.0f, 0.0f, 
        Zeta::RigidBodyCollider::RIGID_CUBE_COLLIDER, new Zeta::Cube({-6, -2, -2}, {6, 2, 6}, 0, 0));

    test_entity->color = {0.0f, 1.0f, 0.0f};
    test_entity->def_texture = TEXTURE_WHITE;

    light_entity = E_::create_entity(&em, HMM_Vec3{13, 43, -20.0f}, 1.0f, 0.0f, 0.0f, 0.0f,  
        Zeta::StaticBodyCollider::STATIC_CUBE_COLLIDER, new Zeta::Cube({-6, -2, -2}, {6, 2, 6}, 0, 0));
        
    light_entity->color = {0.8f, 0.8f, 0.8f};
    light_entity->def_texture = TEXTURE_WHITE;
    
    ground_entity = E_::create_entity(&em, HMM_Vec3{0, -8, -20.0f}, 2.0f, 0.0f, 0.0f, 0.0f,  
        Zeta::StaticBodyCollider::STATIC_CUBE_COLLIDER, new Zeta::Cube({-30.0f, -3.0f, -30.0f}, {30.0f, 3.0f, 30.0f}, 0, 0));
    ground_entity->color = {0.2f, 0.8f, 1.0f};
    ground_entity->def_texture = TEXTURE_WHITE;
    
    dragon_entity = E_::create_entity(&em, HMM_Vec3{10, 4, -10.0f}, 1.0f, 0.0f, 90.0f, 0.0f, 
        Zeta::StaticBodyCollider::STATIC_CUBE_COLLIDER, new Zeta::Cube({-6, -2, -2}, {6, 2, 6}, 0, 0));
    dragon_entity->color = {1.0f, 0.0f, 1.0f};
    dragon_entity->def_texture = TEXTURE_WHITE;
    
    stall_entity = E_::create_entity(&em, HMM_Vec3{-11, 4, -5.0f}, 1.0f, 0.0f, 90.0f, 0.0f, 
        Zeta::StaticBodyCollider::STATIC_CUBE_COLLIDER, new Zeta::Cube({-6, -2, -2}, {6, 2, 6}, 0, 0));
    stall_entity->color = {1.0f, 1.0f, 1.0f};
    stall_entity->def_texture = TEXTURE_STALL;

    test_cube_entity = E_::create_entity(&em, HMM_Vec3{11, 16, -5.0f}, 4.0f, 0.0f, 0.0f, 0.0f, 
        Zeta::StaticBodyCollider::STATIC_CUBE_COLLIDER, new Zeta::Cube({-6, -2, -2}, {6, 2, 6}, 0, 0));
    test_cube_entity->color = {0.8f, 0.3f, 0.3f};
    test_cube_entity->def_texture = TEXTURE_WHITE;

    pine_5_entity = E_::create_entity(&em, HMM_Vec3{21, 0, -20.0f}, 4.0f, 0.0f, 0.0f, 0.0f, 
        Zeta::StaticBodyCollider::STATIC_CUBE_COLLIDER, new Zeta::Cube({-6, -2, -2}, {6, 2, 6}, 0, 0));
    pine_5_entity->isTransparent = true;
    pine_5_entity->color = {1.0f, 1.0f, 1.0f};
    pine_5_entity->def_texture = TEXTURE_PINE_LEAVES;
    E_::add_texture(pine_5_entity, textures_manager.GetTextureIdentifier(TEXTURE_PINE_LEAVES));
    E_::add_texture(pine_5_entity, textures_manager.GetTextureIdentifier(TEXTURE_TREE_BARK));

    
    birch_10_entity = E_::create_entity(&em, HMM_Vec3{41, 0, -20.0f}, 4.0f, 0.0f, 0.0f, 0.0f, 
        Zeta::StaticBodyCollider::STATIC_CUBE_COLLIDER, new Zeta::Cube({-6, -2, -2}, {6, 2, 6}, 0, 0));
    birch_10_entity->color = {1.0f, 1.0f, 1.0f};
    birch_10_entity->isTransparent = true;
    birch_10_entity->def_texture = TEXTURE_BIRCH_LEAVES;
    E_::add_texture(birch_10_entity, textures_manager.GetTextureIdentifier(TEXTURE_BIRCH_LEAVES));
    E_::add_texture(birch_10_entity, textures_manager.GetTextureIdentifier(TEXTURE_TREE_BARK));

    init(test_entity,  create_cube_raw_model(&global_rd, (((Zeta::Cube *)test_entity->rb->collider)->getVertices())));
    
    handler.addRigidBody(test_entity->rb);

    RawModel dragon_model = load_obj_model("thin/stall.obj", dragon_entity->color);
    RawModel stall_model = load_obj_model("thin/stall.obj", stall_entity->color);
    RawModel test_cube_model = load_obj_model("cube.obj", {1.0f, 1.0f, 1.0f, 1.0f});
    // RawModel pine_5_model = load_obj_model("Pine_5.obj", {1.0f, 1.0f, 1.0f, 1.0f});
    RawModel pine_5_model_2 = load_obj_model("Pine_5.obj", {1.0f, 1.0f, 1.0f, 1.0f}, 2);
    RawModel birch_10_model = load_obj_model("Birch_10.obj", {1.0f, 1.0f, 1.0f, 1.0f}, 2);
    init(birch_10_entity, birch_10_model);
    init(pine_5_entity, pine_5_model_2);
    init(dragon_entity, dragon_model);
    init(stall_entity, stall_model);

    init(light_entity, test_cube_model);
    init(test_cube_entity, test_cube_model);
    init(ground_entity, create_cube_raw_model(&global_rd, (((Zeta::Cube *)ground_entity->sb->collider)->getVertices())));
    init(debug_xmover_entity, create_cube_raw_model(&global_rd, (((Zeta::Cube *)debug_xmover_entity->sb->collider)->getVertices())));
    // init(debug_xmover_entity, test_cube_model);
    init(debug_ymover_entity, create_cube_raw_model(&global_rd, (((Zeta::Cube *)debug_ymover_entity->sb->collider)->getVertices())));
    init(debug_zmover_entity, create_cube_raw_model(&global_rd, (((Zeta::Cube *)debug_zmover_entity->sb->collider)->getVertices())));

    handler.addStaticBody(ground_entity->sb);

    SetupTextRenderer(&trm);
    Setup2dRendering(&trm, &textures_manager);
    global_im.window = (GLFWwindow *)window;
    init_renderer(&global_rd);

    // entity behaviors
    test_entity->physics_behavior = &test_entity_physics_behavior;

    // scene setup
    gravity_scene = Scene::new_scene(&global_sm);
    Scene::setup(gravity_scene);
    Scene::add_entity(gravity_scene,  test_entity); 
    Scene::add_entity(gravity_scene,  ground_entity); 

    float time_step = 0;
    Scene::play(gravity_scene, time_step);

    current_scene = gravity_scene;
}

float angle = 0.0f;
float dt_accum = 0.0f;
float dt_avg = 1.0f;
int dt_ticks = 0;
void app_update(float &time_step, float dt){
    global_dt = dt;
    global_im.dt += dt;
    GLenum err;

    global_im.cursor_world_pos_x = 2.0f * global_im.cursorX / WINDOW_WIDTH - 1.0f;
    global_im.cursor_world_pos_y = 1.0f - 2.0f * global_im.cursorY / WINDOW_HEIGHT;

    HMM_Vec4 cursor_world_pos = HMM_Mul(HMM_InvOrthographic(global_rd.projection_matrix), HMM_Vec4{global_im.cursor_world_pos_x, global_im.cursor_world_pos_y, 1.0, 1.0});
    global_im.cursor_world_pos_x = cursor_world_pos.X;
    global_im.cursor_world_pos_y = cursor_world_pos.Y;

    Scene::update(gravity_scene, time_step, &global_rd, &camera, &textures_manager, &global_im);

#if 1
    global_rd.main_light_pos = {light_entity->sb->pos.x, light_entity->sb->pos.y, light_entity->sb->pos.z};
    // ************
    birch_10_entity->initialized = false;
    pine_5_entity->initialized = false;

    ((Zeta::Cube *)(debug_xmover_entity->sb->collider))->theta += dt * 100.0f;
    ((Zeta::Cube *)(debug_xmover_entity->sb->collider))->phi += dt * 100.0f;

    // render_entities(&global_rd, &camera, em.entity_pointers, em.index, &textures_manager, &global_im);  
    //glBindTexture(GL_TEXTURE_2D, textures_manager.GetTextureIdentifier(TEXTURE_STALL));
    // DrawRectTextured(&trm, {500.0f, 600.0f}, 300.0f, 300.0f, {255.0f, 255.0f, 255.0f, 255.0f},  textures_manager.GetTextureIdentifier(TEXTURE_STALL));  
    //glBindTexture(GL_TEXTURE_2D, 0);
    
    // **************
    // getting the average fps
    dt_ticks++;
    dt_accum += dt;
    if(dt_ticks >= 1000){
        dt_accum /= dt_ticks;  
        dt_avg = dt_accum;

        dt_accum = 0.0f;
        dt_ticks = 0;
    }

    String dt_string = Create_String("dt : ");
    AddToString(&dt_string, dt_avg);
#endif

    float x_pos = 150.0f;
    float y_pos = WINDOW_HEIGHT - 60.0f;

    float button_width = 220.0f;
    float button_height = 50.0f;
    float roundness = 5.0f;
    float border_width = 2.0f;
    
    if(g_editor_mode){
        
        Text(&trm, &global_im, 0.4f, Create_String("Click Escape to Exit Editor"),  {WINDOW_WIDTH - 200.0f, WINDOW_HEIGHT - 65.0f},  {255.0f, 160.0f, 160.0f});

        if(Button((void *)1, &global_im, &trm,  Create_String("Collision Detection Scene"), roundness, border_width, x_pos, y_pos, button_width, button_height, {76.5f, 76.5f, 76.5f, 255.0f})){
            printf("Collision Detection Scene!\n");
        }

        // if(Button((void *)3, &global_im, &trm,  Create_String("QUIT"), roundness, border_width, x_pos + 230.0f, y_pos, button_width, button_height, {76.5f, 76.5f, 76.5f, 255.0f})){
        //     printf("Quit!\n");
        // }
        
        if(current_scene){
            if(current_scene->phase == Scene::SCENE_PHASE_PLAYING){
                if(Button((void*) Scene::pause, &global_im, &trm,  Create_String("PAUSE"), roundness, border_width, x_pos + button_width + 30.0f, y_pos, button_width, button_height, {76.5f, 76.5f, 76.5f, 255.0f})){
                    Scene::pause(gravity_scene, time_step);
                
                    printf("Scene Pause!\n");
                }
            } else {
                if(Button((void*) Scene::play, &global_im, &trm,  Create_String("PLAY"), roundness, border_width, x_pos + button_width  + 30.0f, y_pos, button_width, button_height, {76.5f, 76.5f, 76.5f, 255.0f})){
                    Scene::play(current_scene, time_step); 
                    printf("Scene Play!\n");
                }
            }

            if(current_scene->phase != Scene::SCENE_PHASE_SETUP){
                if(Button((void*) Scene::reset, &global_im, &trm,  Create_String("RESET"), roundness, border_width, x_pos + button_width * 2 + 70.0f, y_pos, button_width, button_height, {76.5f, 76.5f, 76.5f, 255.0f})){
                    Scene::reset(current_scene); 
                    printf("Scene Reset!\n");
                }
            }            
        }

    }else{
        Text(&trm, &global_im, 0.4f, Create_String("Click Escape to Enter Editor"),  {WINDOW_WIDTH - 200.0f, WINDOW_HEIGHT - 65.0f},  {255.0f, 160.0f, 160.0f});
    }

    if(current_scene){
        String scene_status_str = {};
        if(current_scene->phase == Scene::SCENE_PHASE_PLAYING){
            scene_status_str = Create_String("Scene Playing");
        }else if(current_scene->phase == Scene::SCENE_PHASE_PAUSED){
            scene_status_str = Create_String("Scene Paused");
        } else if(current_scene->phase == Scene::SCENE_PHASE_SETUP){
            scene_status_str = Create_String("Scene Setup Mode");
        }

        Text(&trm, &global_im, 0.4f, scene_status_str, {WINDOW_WIDTH - 200.0f, WINDOW_HEIGHT - 50.0f},  {175.0f, 175.0f, 175.0f});
    }

    String picker_selection_string = Create_String("Picker ID : ");
    AddToString(&picker_selection_string, (float)global_rd.picker_selection);
    Text(&trm, &global_im, 0.4f, picker_selection_string, {WINDOW_WIDTH - 250.0f, WINDOW_HEIGHT - 350.0f},  {255.0f, 180.0f, 0.0f});
    
    // --------- Draw Selected Entity Data
    E_::Entity *selected_entity = (E_::Entity *)global_im.selected_entity;
    if(selected_entity){  
        
        HMM_Vec2 entity_panel_size = {200.0f, 300.0f};
        HMM_Vec2 entity_panel_pos = {WINDOW_WIDTH - 250.0F, WINDOW_HEIGHT - 200.0f}; // top left

        float padding = 30.0f;

        DrawRect(&trm, {entity_panel_pos.X, entity_panel_pos.Y - entity_panel_size.Y}, entity_panel_size.X, entity_panel_size.Y, {255.0f, 100.0f, 0.0f, 255.0f});

        Text(&trm, &global_im, 0.4f, Create_String("ENTITY DATA"), {entity_panel_pos.X, entity_panel_pos.Y - DEFAULT_TEXT_PIXEL_HEIGHT},  {255.0f, 180.0f, 0.0f});
        entity_panel_pos.Y -= padding;
        
        String entity_identifier_str = Create_String("Identifier : ");
        AddToString(&entity_identifier_str,  (float)selected_entity->internal_identifier);

        Text(&trm, &global_im, 0.4f, entity_identifier_str, {entity_panel_pos.X, entity_panel_pos.Y - DEFAULT_TEXT_PIXEL_HEIGHT},  {255.0f, 180.0f, 0.0f});
        entity_panel_pos.Y -= padding;

        String entity_pos_str = Create_String("Pos : { ");
        HMM_Vec3 pos = {};
        if(selected_entity->rb){
            pos = {selected_entity->rb->pos.x, selected_entity->rb->pos.y, selected_entity->rb->pos.z}; 
        }else if(selected_entity->sb){
            pos = {selected_entity->sb->pos.x, selected_entity->sb->pos.y, selected_entity->sb->pos.z};
        }else{
            Assert(!"Entity has no collider!");
        }
        AddToString(&entity_pos_str, pos.X);
        AddToString(&entity_pos_str, ',');
        AddToString(&entity_pos_str,  pos.Y);
        AddToString(&entity_pos_str, ',');
        AddToString(&entity_pos_str,  pos.Z);
        AddToString(&entity_pos_str, '}');
        
        Text(&trm, &global_im, 0.4f, entity_pos_str, {entity_panel_pos.X, entity_panel_pos.Y - DEFAULT_TEXT_PIXEL_HEIGHT},  {255.0f, 180.0f, 0.0f});
    }

    // ---------- Moving Entities Around Editor
    HMM_Vec2 cursor_current_pos = {(float)global_im.cursorX, (float)global_im.cursorY};
    HMM_Vec2 normalized_cursor_pos = {(cursor_current_pos.X * 2) / WINDOW_WIDTH, (cursor_current_pos.Y * 2) / (WINDOW_HEIGHT)};
    normalized_cursor_pos.X -= 1.0f;
    normalized_cursor_pos.Y -= 1.0f;
    normalized_cursor_pos.Y *= -1.0f;
    
    HMM_Vec4 clip_coords = {normalized_cursor_pos.X, normalized_cursor_pos.Y, -1.0f, 1.0f};

    // to eye coords
    HMM_Vec4 eye_coords = HMM_MulM4V4(HMM_InvOrthographic(global_rd.projection_matrix), clip_coords);
    eye_coords.Z  = -1.0f;
    eye_coords.W  = 1.0f;

    HMM_Vec4 world_coords = HMM_MulM4V4(HMM_InvGeneralM4(global_rd.view_matrix), eye_coords);
    HMM_Vec4 world_ray = {world_coords.X, world_coords.Y, world_coords.Z};

    String cursor_pos_str = Create_String("Current Cursor {");
    AddToString(&cursor_pos_str, (float)cursor_position.X);
    AddToString(&cursor_pos_str, ',');
    AddToString(&cursor_pos_str, (float)cursor_position.Y);
    AddToString(&cursor_pos_str, ',');
    AddToString(&cursor_pos_str, (float)cursor_position.Z);
    AddToString(&cursor_pos_str, '}');
    Text(&trm, &global_im, 0.4f, cursor_pos_str, {470.0f, WINDOW_HEIGHT - 350.0f},  {255.0f, 180.0f, 0.0f});
    
    String cursor_last_pos_str = Create_String("Last Cursor {");
    AddToString(&cursor_last_pos_str, (float)last_mouse_x);
    AddToString(&cursor_last_pos_str, ',');
    AddToString(&cursor_last_pos_str, (float)last_mouse_x);
    AddToString(&cursor_last_pos_str, ',');
    AddToString(&cursor_last_pos_str, (float)0);
    AddToString(&cursor_last_pos_str, '}');
    Text(&trm, &global_im, 0.4f, cursor_last_pos_str, {470.0f, WINDOW_HEIGHT - 370.0f},  {255.0f, 180.0f, 0.0f});


    if(global_im.active_entity == debug_xmover_entity){
        printf("IT IS!\n");
        // HMM_Vec2 unit_vector = {(cursor_current_pos.X - cursor_initial_pos.X) / cursor_magnitude, (cursor_current_pos.X - cursor_initial_pos.X) / cursor_magnitude};

        // if(moving && im->selected_entity){
        //     if(((E_::Entity *)im->selected_entity)->rb){
        //         ((E_::Entity *)im->selected_entity)->rb->pos = {original_entity_pos.X, original_entity_pos.Y + cursor_magnitude * unit_vector.X * (1), original_entity_pos.Z};
        //     } else if(((E_::Entity *)im->selected_entity)->sb){
        //         ((E_::Entity *)im->selected_entity)->sb->pos = {original_entity_pos.X, original_entity_pos.Y + cursor_magnitude * unit_vector.X * (1), original_entity_pos.Z};
        //     }
        // }
        // printf("{x : %f, y : %f}\n", unit_vector.X, unit_vector.Y);

        // ((debug_xmover_entity->sb->pos)).x = global_im.picker_entity_initial_pos.X - (cursor_magnitude);

        // String magnitude_str = Create_String("magnitude  ---  ");
        // AddToString(&magnitude_str, cursor_magnitude);
        // Text(&trm, &global_im, 0.4f, magnitude_str, {470.0f, WINDOW_HEIGHT - 300.0f},  {255.0f, 180.0f, 0.0f});
        // Text(&trm, &global_im, 0.4f, Create_String("MAGNITUDE : %f"), {470.0f, WINDOW_HEIGHT - 300.0f},  {255.0f, 180.0f, 0.0f});
        
        
    }

#if 1
    String cursor_pos_string = Create_String("Cursor {");
    AddToString(&cursor_pos_string, (float)global_im.cursor_world_pos_x);
    AddToString(&cursor_pos_string, ',');
    AddToString(&cursor_pos_string, (float)global_im.cursor_world_pos_y);
    AddToString(&cursor_pos_string, '}');
    Text(&trm, &global_im, 0.4f, cursor_pos_string, {WINDOW_WIDTH - 250.0f, WINDOW_HEIGHT - 375.0f},  {255.0f, 180.0f, 0.0f});
#endif

    String fps_string = Create_String("F P S : ");
    AddToString(&fps_string, 1 / dt_avg);
    Text(&trm, &global_im, 0.4f, fps_string, {10.0f, WINDOW_HEIGHT - 50.0f},  {255.0f, 180.0f, 150.0f});
    DeleteString(&dt_string);
    DeleteString(&fps_string);
}

void clean_up() {
	CleanTextRenderer(&trm);
	for (unsigned int vao : global_rd.vaos) {
		glDeleteVertexArrays(1, &vao);
	}

	for (unsigned int vbo : global_rd.vbos) {
		glDeleteBuffers(1, &vbo);
	}

	for (unsigned int texture : global_rd.textures) {
		glDeleteTextures(1, &texture);
	}

    // clean entities
    for(int i = 0; i < em.index; i++){
        E_::Entity *e = &em.entities[0];
    }
}

