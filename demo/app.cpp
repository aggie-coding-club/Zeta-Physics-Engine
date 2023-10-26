// #include <zeta/physicshandler.h>
#include "app.h"
#include "text.cpp"
#include "ui.cpp"

#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "shader.h"
#include "entity.h"
#include "renderer.h"

std::vector<unsigned int> vaos;
std::vector<unsigned int> vbos;
std::vector<unsigned int> textures;
float global_dt = 0.0f;
HMM_Vec3 cursor_position = {};

void PrintGLError(){
    int gl_error = glGetError(); 
    printf("GL Error %i \n", gl_error);
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

    RawModel result = load_to_VAO(&vertexData);


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

    RawModel result = load_to_VAO(&vertexData);


    return result;
}

unsigned int create_VAO(){
    unsigned int result = 0;
    vaos.push_back(result);

    glGenVertexArrays(1, &result);
    glBindVertexArray(result);

    return result;
}

void store_data_in_attribute_list(int attribute_num, float data[], int num_of_components, int data_size){
    unsigned int vbo_ID = 0;
    glGenBuffers(1, &vbo_ID);

    vbos.push_back(vbo_ID);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_ID);
    
    glBufferData(GL_ARRAY_BUFFER, data_size, data, GL_STATIC_DRAW);
    // Note(Lenny): 2nd param may not hold for other atttribs
    // int num_of_components = (int)(data_size / sizeof(float) / sizeof(float));
    // printf("num of comps %i \n", num_of_components);
    glVertexAttribPointer(attribute_num, num_of_components, GL_FLOAT, false, 0, 0);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);    
}

void unbind_VAO(){
    glBindVertexArray(0);
}

void createIndicesBuffer(int indices[], int indices_size){
    unsigned int ibo_ID = 0;
    glGenBuffers(1, &ibo_ID);

    vbos.push_back(ibo_ID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_ID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_size, indices, GL_STATIC_DRAW);
}

RawModel load_to_VAO(VertexData *vertex_data){
    RawModel result = {};
    result.vao_ID = create_VAO();
    result.vertex_count = vertex_data->len_indices;

    createIndicesBuffer(vertex_data->indices, vertex_data->len_indices * sizeof(int));

    store_data_in_attribute_list(0, vertex_data->positions, 3, vertex_data->len_positions * sizeof(float));
    store_data_in_attribute_list(1, vertex_data->tex_coords, 3, vertex_data->len_tex_coords * sizeof(float));
    store_data_in_attribute_list(2, vertex_data->normals, 3, vertex_data->len_normals * sizeof(float));
    store_data_in_attribute_list(3, vertex_data->colors, 3, vertex_data->len_colors * sizeof(float));
    
    unbind_VAO();
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
RendererData rd = {};

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
    rd.projection_fov -= (float)y_offset;
    if(rd.projection_fov < 1.0f){
        rd.projection_fov = 1.0f;
    }

    if(rd.projection_fov > DEFAULT_FOV){
        rd.projection_fov = DEFAULT_FOV;
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
    SetEditMode(1);

    printf("Program Started\n");
    textures_manager = TexturesManager();
    rd.main_shader.program = load_shaders("shaders/web_v_shader.glsl", "shaders/web_f_shader.glsl");
    rd.projection_fov = DEFAULT_FOV;

    // >>>>>> Shader Stuff
    // =====================================

    glUseProgram(rd.main_shader.program);

    unsigned int u_specular_strength = get_uniform_location(&rd.main_shader, "specular_strength");
    unsigned int u_reflectivity = get_uniform_location(&rd.main_shader, "reflectivity");
    
    set_uniform_value(u_specular_strength, 0.25f);
    set_uniform_value(u_reflectivity, 64.0f);
    
    glUseProgram(0);
    
    // >>>>>> Texture Stuff
    // =====================================
    textures_manager.add_texture("white.png", TEXTURE_WHITE, TEX_FORMAT_PNG);
    textures_manager.add_texture("thin/stallTexture.png", TEXTURE_STALL, TEX_FORMAT_PNG);
    textures_manager.add_texture("Birch_Leaves_Green.png", TEXTURE_BIRCH_LEAVES, TEX_FORMAT_PNG);
    textures_manager.add_texture("Pine_Leaves.png", TEXTURE_PINE_LEAVES, TEX_FORMAT_PNG);
    textures_manager.add_texture("Tree_Bark.jpg", TEXTURE_TREE_BARK, TEX_FORMAT_JPG);

    // >>>>>> View Stuff
    // =====================================
    camera.speed = 10000.0f;
    camera.position.X = -29.0; 
    camera.position.Y = 21.0; 
    camera.position.Z = -53.0;

    camera.world_up = {0.0f, 1.0f, 0.0f};
    camera.front = {0.0f, 0.0f, -1.0f};
    
    // >>>>>> Entity Stufff
    // ========================================
    Zeta::Cube cube1({-2, -2, -2}, {2, 2, 2}, 0, 0);
    Zeta::Cube ground_cube({-30.0f, -3.0f, -30.0f}, {30.0f, 3.0f, 30.0f}, 0, 0);
    
    test_entity = E_::create_entity(&em, HMM_Vec3{0, 6, -20.0f}, 1.0f, 0.0f, 0.0f, 0.0f, 
        Zeta::RigidBodyCollider::RIGID_CUBE_COLLIDER, &cube1);

    test_entity->color = {0.0f, 1.0f, 0.0f};
    test_entity->def_texture = TEXTURE_WHITE;

    light_entity = E_::create_entity(&em, HMM_Vec3{13, 43, -20.0f}, 1.0f, 0.0f, 0.0f, 0.0f,  
        Zeta::StaticBodyCollider::STATIC_CUBE_COLLIDER, &cube1);
        
    light_entity->color = {0.8f, 0.8f, 0.8f};
    light_entity->def_texture = TEXTURE_WHITE;
    
    ground_entity = E_::create_entity(&em, HMM_Vec3{0, -8, -20.0f}, 2.0f, 0.0f, 0.0f, 0.0f,  
        Zeta::StaticBodyCollider::STATIC_CUBE_COLLIDER, &ground_cube);
    ground_entity->color = {0.2f, 0.8f, 1.0f};
    ground_entity->def_texture = TEXTURE_WHITE;
    
    dragon_entity = E_::create_entity(&em, HMM_Vec3{10, 4, -10.0f}, 1.0f, 0.0f, 90.0f, 0.0f, 
        Zeta::StaticBodyCollider::STATIC_CUBE_COLLIDER, &cube1);
    dragon_entity->color = {1.0f, 0.0f, 1.0f};
    dragon_entity->def_texture = TEXTURE_WHITE;
    
    stall_entity = E_::create_entity(&em, HMM_Vec3{-11, 4, -5.0f}, 1.0f, 0.0f, 90.0f, 0.0f, 
        Zeta::StaticBodyCollider::STATIC_CUBE_COLLIDER, &cube1);
    stall_entity->color = {1.0f, 1.0f, 1.0f};
    stall_entity->def_texture = TEXTURE_STALL;

    test_cube_entity = E_::create_entity(&em, HMM_Vec3{11, 16, -5.0f}, 4.0f, 0.0f, 0.0f, 0.0f, 
        Zeta::StaticBodyCollider::STATIC_CUBE_COLLIDER, &cube1);
    test_cube_entity->color = {0.8f, 0.3f, 0.3f};
    test_cube_entity->def_texture = TEXTURE_WHITE;

    pine_5_entity = E_::create_entity(&em, HMM_Vec3{21, 0, -20.0f}, 4.0f, 0.0f, 0.0f, 0.0f, 
        Zeta::StaticBodyCollider::STATIC_CUBE_COLLIDER, &cube1);
    pine_5_entity->isTransparent = true;
    pine_5_entity->color = {1.0f, 1.0f, 1.0f};
    pine_5_entity->def_texture = TEXTURE_PINE_LEAVES;
    E_::add_texture(pine_5_entity, textures_manager.GetTextureIdentifier(TEXTURE_PINE_LEAVES));
    E_::add_texture(pine_5_entity, textures_manager.GetTextureIdentifier(TEXTURE_TREE_BARK));

    
    birch_10_entity = E_::create_entity(&em, HMM_Vec3{41, 0, -20.0f}, 4.0f, 0.0f, 0.0f, 0.0f, 
        Zeta::StaticBodyCollider::STATIC_CUBE_COLLIDER, &cube1);
    birch_10_entity->color = {1.0f, 1.0f, 1.0f};
    birch_10_entity->isTransparent = true;
    birch_10_entity->def_texture = TEXTURE_BIRCH_LEAVES;
    E_::add_texture(birch_10_entity, textures_manager.GetTextureIdentifier(TEXTURE_BIRCH_LEAVES));
    E_::add_texture(birch_10_entity, textures_manager.GetTextureIdentifier(TEXTURE_TREE_BARK));

    init(test_entity);
    
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
    init(ground_entity);

    SetupTextRenderer(&trm);
    Setup2dRendering(&trm);
    im.window = (GLFWwindow *)window;
    
}

float angle = 0.0f;
float dt_accum = 0.0f;
float dt_avg = 1.0f;
int dt_ticks = 0;
void app_update(float &time_step, float dt){
    global_dt = dt;
    im.dt += dt;

    prepare_renderer(&rd, &camera);
#if 1
    rd.main_light_pos = {light_entity->sb->pos.x, light_entity->sb->pos.y, light_entity->sb->pos.z};
    // ************
    // render(pine_5_entity, &textures_manager, &rd.main_shader);
    // render(birch_10_entity, &textures_manager, &rd.main_shader);
    render(light_entity, &textures_manager, &rd.main_shader);    
    render(test_cube_entity, &textures_manager, &rd.main_shader);
    render(test_entity, &textures_manager, &rd.main_shader);
    render(ground_entity, &textures_manager, &rd.main_shader);
    render(dragon_entity, &textures_manager, &rd.main_shader);
    render(stall_entity, &textures_manager, &rd.main_shader);
    

    // **************
    int physics_updates = handler.update(time_step);
    
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
    }

    String fps_string = Create_String("F P S : ");
    AddToString(&fps_string, 1 / dt_avg);
    Text(&trm, &im, 0.4f, fps_string, {x_pos + 470.0f, WINDOW_HEIGHT - 50.0f},  {255.0f, 180.0f, 0.0f});
    DeleteString(&dt_string);
    DeleteString(&fps_string);
}

void clean_up() {
	CleanTextRenderer(&trm);
	for (unsigned int vao : vaos) {
		glDeleteVertexArrays(1, &vao);
	}

	for (unsigned int vbo : vbos) {
		glDeleteBuffers(1, &vbo);
	}

	for (unsigned int texture : textures) {
		glDeleteTextures(1, &texture);
	}
}

