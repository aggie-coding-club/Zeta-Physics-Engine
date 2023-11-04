// #include <zeta/physicshandler.h>
#include "app.h"
#include "text.cpp"
#include "ui.cpp"

#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "shader.h"
#include "entity.h"
#include "renderer.h"

float global_dt = 0.0f;
HMM_Vec3 cursor_position = {};
RendererData global_rd = {};

void PrintGLError(){
    int gl_error = glGetError(); 

    GLenum err;
    while((err = glGetError()) != GL_NO_ERROR) {
        std::cout << "[GL Error] " << err << "\n";
    }
}

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
InputManager im = {};

void SetCursorPosition(float x, float y){
    cursor_position.X = x;
    cursor_position.Y = y;
    im.cursorX = x;
    im.cursorY = y;

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
        ShowCursor(last_mouse_x, last_mouse_y);
    }else{
        // -		cursor_position	{X=1225.00000 Y=788.000000 Z=0.00000000 ...}	HMM_Vec3

        // HideCursor(WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f);
        HideCursor(1225.0f, 788.0f);
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
    
    test_entity = E_::create_entity(&em, HMM_Vec3{0, 24, -20.0f}, 1.0f, 0.0f, 0.0f, 0.0f, 
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

    handler.addStaticBody(ground_entity->sb);

    SetupTextRenderer(&trm);
    Setup2dRendering(&trm, &textures_manager);
    im.window = (GLFWwindow *)window;
    init_renderer(&global_rd);
}

float angle = 0.0f;
float dt_accum = 0.0f;
float dt_avg = 1.0f;
int dt_ticks = 0;
int first_frame = 0;
void app_update(float &time_step, float dt){
    global_dt = dt;
    im.dt += dt;
    GLenum err;

#if 1
    global_rd.main_light_pos = {light_entity->sb->pos.x, light_entity->sb->pos.y, light_entity->sb->pos.z};
    // ************
    birch_10_entity->initialized = false;
    pine_5_entity->initialized = false;
    render_entities(&global_rd, &camera, &em.entities[0], &textures_manager, &im);  
    glBindTexture(GL_TEXTURE_2D, textures_manager.GetTextureIdentifier(TEXTURE_STALL));
    DrawRectTextured(&trm, {500.0f, 600.0f}, 300.0f, 300.0f, {255.0f, 255.0f, 255.0f, 255.0f},  textures_manager.GetTextureIdentifier(TEXTURE_STALL));  
    glBindTexture(GL_TEXTURE_2D, 0);
    
    // **************
    int physics_updates = handler.update(time_step);

    if(first_frame < 0){
        printf("GLOBAL DT : %f ---- DT %f \n", time_step, dt);

        printf("---- test entity data ---- \n");
        printf("pos     {x : %f, y : %f}\n", test_entity->rb->pos.x, test_entity->rb->pos.y);
        printf("mass    %f\n", test_entity->rb->mass);
        printf("cor     %f\n", test_entity->rb->cor);
        
        printf("cube pos    {x : %f, y : %f}\n", ((Zeta::Cube *)(test_entity->rb->collider))->pos.x,  ((Zeta::Cube *)(test_entity->rb->collider))->pos.y);
        printf("cube hs     {x : %f, y : %f}\n", ((Zeta::Cube *)(test_entity->rb->collider))->halfSize.x,  ((Zeta::Cube *)(test_entity->rb->collider))->halfSize.y);

        printf("---- ground entity data ---- \n");
        printf("pos     {x : %f, y : %f}\n", ground_entity->sb->pos.x, ground_entity->sb->pos.y);

        printf("ground pos    {x : %f, y : %f}\n", ((Zeta::Cube *)(ground_entity->sb->collider))->pos.x,  ((Zeta::Cube *)(ground_entity->sb->collider))->pos.y);
        printf("ground hs     {x : %f, y : %f}\n", ((Zeta::Cube *)(ground_entity->sb->collider))->halfSize.x,  ((Zeta::Cube *)(ground_entity->sb->collider))->halfSize.y);
        first_frame++;
    }
    ZMath::Vec3D normal = {};
    float ground_cube_colliding = Zeta::CubeAndCube(*((Zeta::Cube *)(test_entity->rb->collider)), *((Zeta::Cube *)ground_entity->sb->collider), normal);
    
    if(ground_cube_colliding){
        for(int i = 0; i < physics_updates; i++){
            test_entity->rb->vel = 0;
            test_entity->rb->netForce -= handler.g * test_entity->rb->mass;
        }
        ground_entity->color = HMM_Vec4{1.0f, 1.0f, 0.0f, 1.0f};
    }else{
        ground_entity->color = HMM_Vec4{1.0f, 1.0f, 1.0f, 1.0f};
    }
    
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

    float x_pos = 100.0f;
    float y_pos = WINDOW_HEIGHT - 60.0f;

    float button_width = 220.0f;
    float button_height = 50.0f;
    float roundness = 5.0f;
    float border_width = 2.0f;
    
    if(g_editor_mode){
        Text(&trm, &im, 0.4f, Create_String("Click Escape to Exit Editor Mode "), {x_pos + 580.0f, WINDOW_HEIGHT - 50.0f},  {255.0f, 100.0f, 0.0f});

        if(Button((void *)1, &im, &trm,  Create_String("Collision Detection Scene"), roundness, border_width, x_pos, y_pos, button_width, button_height, {0.3f, 0.3f, 0.3f, 1.0f})){
            printf("Collision Detection Scene!\n");
        }

        if(Button((void *)3, &im, &trm,  Create_String("QUIT"), roundness, border_width, x_pos + 230.0f, y_pos, button_width, button_height, {0.3f, 0.3f, 0.3f, 1.0f})){
            printf("Quit!\n");
        }
    }else{

        String ground_entity_pos_string = Create_String("Ground E Pos : { ");
        AddToString(&ground_entity_pos_string,  ((Zeta::Cube *)(ground_entity->sb->collider))->pos.x);
        AddToString(&ground_entity_pos_string, ',');
        AddToString(&ground_entity_pos_string,  ((Zeta::Cube *)(ground_entity->sb->collider))->pos.y);
        AddToString(&ground_entity_pos_string, '}');
        
        String test_entity_pos_string = Create_String("Test E Pos : { ");
        AddToString(&test_entity_pos_string,  ((Zeta::Cube *)(test_entity->rb->collider))->pos.x);
        AddToString(&test_entity_pos_string, ',');
        AddToString(&test_entity_pos_string,  ((Zeta::Cube *)(test_entity->rb->collider))->pos.y);
        AddToString(&test_entity_pos_string, '}');
        
        Text(&trm, &im, 0.4f, ground_entity_pos_string, {WINDOW_WIDTH - 250.0f, WINDOW_HEIGHT - 200.0f},  {255.0f, 180.0f, 0.0f});
        Text(&trm, &im, 0.4f, test_entity_pos_string, {WINDOW_WIDTH - 250.0f, WINDOW_HEIGHT - 300.0f},  {255.0f, 180.0f, 0.0f});
    }

    String picker_selection_string = Create_String("Picker ID : ");
    AddToString(&picker_selection_string, (float)global_rd.picker_selection);
    Text(&trm, &im, 0.4f, picker_selection_string, {WINDOW_WIDTH - 250.0f, WINDOW_HEIGHT - 350.0f},  {255.0f, 180.0f, 0.0f});
    
    String cursor_pos_string = Create_String("Cursor {");
    AddToString(&cursor_pos_string, (float)im.cursorX);
    AddToString(&cursor_pos_string, ',');
    AddToString(&cursor_pos_string, (float)im.cursorY);
    AddToString(&cursor_pos_string, '}');
    Text(&trm, &im, 0.4f, cursor_pos_string, {WINDOW_WIDTH - 250.0f, WINDOW_HEIGHT - 375.0f},  {255.0f, 180.0f, 0.0f});

    String fps_string = Create_String("F P S : ");
    AddToString(&fps_string, 1 / dt_avg);
    Text(&trm, &im, 0.4f, fps_string, {x_pos + 470.0f, WINDOW_HEIGHT - 50.0f},  {255.0f, 180.0f, 0.0f});
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

