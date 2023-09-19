// #include <zeta/physicshandler.h>
#include "app.h"
#include "text.cpp"
#include "ui.cpp"

#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "shader.h"
#include "entity.h"

std::vector<unsigned int> vaos;
std::vector<unsigned int> vbos;
std::vector<unsigned int> textures;
float global_dt = 0.0f;
HMM_Vec3 cursor_position = {};

void PrintGLError(){
    int gl_error = glGetError(); 
    printf("GL Error %i \n", gl_error);
}

#if 1
class Entity{

    public:
        RawModel raw_model;
        unsigned int def_texture;
        
        HMM_Vec4 color = {};
        float scale = 0.0f;
        float rotation_x = 0.0f;
        float rotation_y = 0.0f;
        float rotation_z = 0.0f;

        Zeta::RigidBody3D *rb = 0;
        Zeta::StaticBody3D *sb = 0;
    
        Entity(HMM_Vec3 position, float scale, 
            float rotation_x, float rotation_y, float rotation_z, Zeta::RigidBodyCollider colliderType, void *collider){
            this->scale = scale;
            this->rotation_x = rotation_x;
            this->rotation_y = rotation_y;
            this->rotation_z = rotation_z;

            this->rb = new Zeta::RigidBody3D(
                {position.X, position.Y, position.Z}, 
                100.0f, 0.1f, 1.0f, colliderType, collider);
        }

        Entity(HMM_Vec3 position, float scale, 
            float rotation_x, float rotation_y, float rotation_z,  Zeta::StaticBodyCollider colliderType, void *collider){
            this->scale = scale;
            this->rotation_x = rotation_x;
            this->rotation_y = rotation_y;
            this->rotation_z = rotation_z;

            this->sb = new Zeta::StaticBody3D(
            {position.X, position.Y, position.Z}, 
            colliderType, collider);   
        }

        // call after `AddCollider()`
        void Init(){
            
            std::vector<float> tex_coords = {
                0,0,
                0,1,
                1,1,
                1,0,			
                0,0,
                0,1,
                1,1,
                1,0,			
                0,0,
                0,1,
                1,1,
                1,0,
                0,0,
                0,1,
                1,1,
                1,0,
                0,0,
                0,1,
                1,1,
                1,0,
                0,0,
                0,1,
                1,1,
                1,0
            };

            HMM_Vec4 color = {1.0f, 1.0f, 1.0f, 1.0f};
            std::vector<float> cube_colors = {
                // TOP
                color.X, color.Y, color.Z,
                color.X, color.Y, color.Z,
                color.X, color.Y, color.Z,
                color.X, color.Y, color.Z,

                // TOP
                color.X, color.Y, color.Z,
                color.X, color.Y, color.Z,
                color.X, color.Y, color.Z,
                color.X, color.Y, color.Z,
                
                // TOP
                color.X, color.Y, color.Z,
                color.X, color.Y, color.Z,
                color.X, color.Y, color.Z,
                color.X, color.Y, color.Z,
                
                // TOP
                color.X, color.Y, color.Z,
                color.X, color.Y, color.Z,
                color.X, color.Y, color.Z,
                color.X, color.Y, color.Z,
                
                // TOP
                color.X, color.Y, color.Z,
                color.X, color.Y, color.Z,
                color.X, color.Y, color.Z,
                color.X, color.Y, color.Z,
                
                // TOP
                color.X, color.Y, color.Z,
                color.X, color.Y, color.Z,
                color.X, color.Y, color.Z,
                color.X, color.Y, color.Z,
            };

            VertexData vertex_data = {};
            vertex_data.positions = new float[3 * 4 * 6 * sizeof(float)]();
            vertex_data.normals = new float[3 * 4 * 6 * sizeof(float)]();
            vertex_data.colors = new float[3 * 4 * 6 * sizeof(float)]();
            vertex_data.indices = new int[3 * 2 * 6 * sizeof(int)]();

            vertex_data.tex_coords = new float[2 * 4 * 6 * sizeof(float)];
            vertex_data.len_tex_coords = 2 * 4 * 6;

            if(sb){
                E_::ZetaVertsToEq(sb->collider.cube.getVertices(), &vertex_data);
            }else if(rb){
                E_::ZetaVertsToEq(rb->collider.cube.getVertices(), &vertex_data);
            }else{
                Assert(!"No RigidBody or StaticBody Attached");
            }

            vertex_data.colors = &cube_colors[0];
            vertex_data.len_colors = vertex_data.len_positions;
            vertex_data.tex_coords = &tex_coords[0];
            vertex_data.len_tex_coords = 48;

            if(sb){
                sb->collider.cube.pos = sb->pos;
            }else if(rb){
                rb->collider.cube.pos = rb->pos;
            }

            raw_model = load_to_VAO(&vertex_data);
        }

        void Init(RawModel model){
            raw_model = model;
        }

        void AddCollider(Zeta::RigidBodyCollider colliderType, void *collider){
            if(colliderType == Zeta::RigidBodyCollider::RIGID_CUBE_COLLIDER){
                Zeta::Cube *cube = (Zeta::Cube *)collider;

                // cube->pos = rb->pos;
                rb->collider.cube = *cube;
            }
        }

        void AddCollider(Zeta::StaticBodyCollider colliderType, void *collider){
            if(colliderType == Zeta::StaticBodyCollider::STATIC_CUBE_COLLIDER){
                Zeta::Cube *cube = (Zeta::Cube *)collider;

                sb->collider.cube = *cube;
            }
        }

        void IncreaseRotation(float dx, float dy, float dz){
            rotation_x += dx;
            rotation_y += dy;
            rotation_z += dz;
        }
    
};

#endif

class TexturesManager{

    public:
        int textures_count = 0;
        std::vector<Texture> textures;
        
        TexturesManager(){

        }

    public:
        void AddTexture(std::string path, unsigned int def_name){
            textures_count++;

            Texture result = {};
            result.file_path = path;
            std::string texture_src = path;
            std::string web_texture_src = "vendor/" + texture_src;

            int width = 0;
            int height = 0;
            int nr_channels = 0;

            // Note(Lenny) : might need to be flipped
            #if __EMSCRIPTEN__
            unsigned char *data = stbi_load(&web_texture_src[0], &width, &height, &nr_channels, 0);
            #else
            unsigned char *data = stbi_load(&texture_src[0], &width, &height, &nr_channels, 0);
            #endif
            if(data){
                std::cout << "loaded png \n" << texture_src << std::endl;
            } else {
                std::cout << "failed to load png \n" << texture_src << std::endl;
            }

            glGenTextures(1, &result.id);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, result.id);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
                GL_UNSIGNED_BYTE, data);
            glBindTexture(GL_TEXTURE_2D, 0);

            stbi_image_free(data);

            result.def_name = def_name;
            textures.push_back(result);
        }

        // massive optimization to be done here
        // perhaps use a fancy finding algorithm
        unsigned int GetTextureIdentifier(unsigned int def_name){
            unsigned int result = 0;
            for(int i = 0; i < textures.size(); i++){
                if(textures.at(i).def_name == def_name){
                    result = textures.at(i).id;
                    break;
                }
            }
            return result;
        }

        Texture GetTexture(unsigned int identifer){
            Texture result = {};
            for(int i = 0; i < textures.size(); i++){
                if(textures.at(i).def_name == identifer){
                    result = textures.at(i);
                    break;
                }
            }
            return result;
        }
};

Shader test_shader = {};

Texture LoadTextures(std::string filename){
    Texture result = {};
    result.file_path = filename;
    std::string texture_src = filename;
    std::string web_texture_src = "vendor/" + texture_src;

    int width = 0;
    int height = 0;
    int nr_channels = 0;

    // Note(Lenny) : might need to be flipped
    #if __EMSCRIPTEN__
    unsigned char *data = stbi_load(&web_texture_src[0], &width, &height, &nr_channels, 0);
    #else
    unsigned char *data = stbi_load(&texture_src[0], &width, &height, &nr_channels, 0);
    #endif
    if(data){
        std::cout << "loaded png \n" << texture_src << std::endl;
    } else {
        std::cout << "failed to load png \n" << texture_src << std::endl;
    }

    glGenTextures(1, &result.id);
    glBindTexture(GL_TEXTURE_2D, result.id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
        GL_UNSIGNED_BYTE, data);
    glBindTexture(GL_TEXTURE_2D, 0);

    stbi_image_free(data);

    return result;
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
void ProcessVertex(std::vector<std::string> vertexData, std::vector<int> *indices, std::vector<HMM_Vec2> *textures,
    std::vector<HMM_Vec3> *normals, std::vector<float> *texturesArray, std::vector<float> *normalsArray){
    
    int currentVertexPointer = (int)std::stof(vertexData[0]) - 1;
    indices->push_back(currentVertexPointer);

    HMM_Vec2 currentTex = textures->at((int)std::stof(vertexData[1]) - 1);

    texturesArray->at(currentVertexPointer * 2) = currentTex.X;
    texturesArray->at(currentVertexPointer * 2 + 1) = 1 - currentTex.Y;

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
    std::vector<HMM_Vec2> textures; // tex coords
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
            
            normalsArray.push_back(0);
            normalsArray.push_back(0);
            normalsArray.push_back(0);

        } else if (s[0] == "vt"){ // texture coord
            HMM_Vec2 textureCoord = {std::stof(s[1]), std::stof(s[2])};
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

            ProcessVertex(vertex1, &indicesArray, &textures, &normals, &texturesArray, &normalsArray);
            ProcessVertex(vertex2, &indicesArray, &textures, &normals, &texturesArray, &normalsArray);
            ProcessVertex(vertex3, &indicesArray, &textures, &normals, &texturesArray, &normalsArray);
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
    store_data_in_attribute_list(1, vertex_data->tex_coords, 2, vertex_data->len_tex_coords * sizeof(float));
    store_data_in_attribute_list(2, vertex_data->normals, 3, vertex_data->len_normals * sizeof(float));
    store_data_in_attribute_list(3, vertex_data->colors, 3, vertex_data->len_colors * sizeof(float));
    
    unbind_VAO();
    return result;
}

void render(Entity *entity, TexturesManager *textures_manager){
    
    HMM_Mat4 transformation;
    if(entity->sb){
        transformation = HMM_Translate({entity->sb->pos.x, entity->sb->pos.y, entity->sb->pos.z});
    } else {
        transformation = HMM_Translate({entity->rb->pos.x, entity->rb->pos.y, entity->rb->pos.z});
    }

    // u_transform_matrix = GetUniformLocation("transformation_matrix");
    // u_projection_matrix = GetUniformLocation("projection_matrix");
    // u_view_matrix = GetUniformLocation("view_matrix");
    // u_camera_position = GetUniformLocation("camera_position");
    
    transformation = HMM_Mul(transformation, HMM_Rotate_RH(HMM_ToRad(entity->rotation_x), HMM_Vec3{1.0f, 0.0f, 0.0f}));
    transformation = HMM_Mul(transformation, HMM_Rotate_RH(HMM_ToRad(entity->rotation_y), HMM_Vec3{0.0f, 1.0f, 0.0f}));
    transformation = HMM_Mul(transformation, HMM_Rotate_RH(HMM_ToRad(entity->rotation_z), HMM_Vec3{0.0f, 0.0f, 1.0f}));
    transformation = HMM_Mul(transformation, HMM_Scale(HMM_Vec3{entity->scale, entity->scale, entity->scale}));
    unsigned int u_transform_matrix = GetUniformLocation(&test_shader, "transformation_matrix");
    SetUniformValue(u_transform_matrix, transformation);
    // test_shader->LoadTransformationMatrix(transformation);
    // test_shader->LoadColor(entity->color);
    unsigned int u_entity_color = GetUniformLocation(&test_shader, "u_color");
    SetUniformValue(u_entity_color, entity->color);

    glBindVertexArray(entity->raw_model.vao_ID);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);    
    glEnableVertexAttribArray(2);    
    glEnableVertexAttribArray(3);    

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textures_manager->GetTextureIdentifier(entity->def_texture));

    glDrawElements(GL_TRIANGLES, entity->raw_model.vertex_count, GL_UNSIGNED_INT, 0);
    
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(3);
    glBindVertexArray(0);
    
}

Zeta::Handler handler(ZMath::Vec3D(0, -5.8f, 0));
// Zeta::RigidBody3D *test_body_1;
// Zeta::StaticBody3D *ground_body;

float time_btw_physics_updates = 1.0f / 60.0f;
float count_down = time_btw_physics_updates;
float start_time = (float)glfwGetTime();

RawModel model = {};
RawModel ground_model = {};
Entity *test_entity = 0;
Entity *light_entity = 0;
Entity *ground_entity = 0;
Entity *dragon_entity = 0;
Entity *stall_entity = 0;
Entity *test_cube_entity = 0;

HMM_Mat4 projection;
HMM_Mat4 view_matrix;

struct Camera{
    HMM_Vec3 position;
    float speed;
    float pitch;
    float yaw;
    float roll;
};

HMM_Vec4 mouse_ndc = {};

Camera camera = {};
float projection_fov = DEFAULT_FOV;
HMM_Vec3 world_up = {0.0f, 1.0f, 0.0f};
HMM_Vec3 camera_front = {0.0f, 0.0f, -1.0f};
bool first_mouse = 1;
float last_mouse_x = 0.0f;
float last_mouse_y = 0.0f;
float g_editor_mode = 0;

TexturesManager textures_manager;
TextRendererManager trm = {};
InputManager im = {};

void CreateViewMatrix(){
    view_matrix = HMM_LookAt_RH(camera.position, camera.position + camera_front, world_up);
}

void CreateProjectionMatrix(){
    float near_plane = 0.1f;
    float far_plane = 1000.0f;

    projection = HMM_Perspective_RH_ZO(HMM_DegToRad * projection_fov, WINDOW_WIDTH/WINDOW_HEIGHT, near_plane, far_plane);
}


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

        camera_front = HMM_Norm(camera_direction);
    }
    // camera_front = {0.0f, 0.0f, -1.0f};
}

void SetScroll(float x_offset, float y_offset){
    projection_fov -= (float)y_offset;
    if(projection_fov < 1.0f){
        projection_fov = 1.0f;
    }

    if(projection_fov > DEFAULT_FOV){
        projection_fov = DEFAULT_FOV;
    }
}

void SetEditMode(int mode){
    g_editor_mode = mode;

    if(g_editor_mode){
        ShowCursor(last_mouse_x, last_mouse_y);
    }else{
        HideCursor(WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f);
    }   
}

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
            camera.position += HMM_Norm(HMM_Cross(camera_front, world_up)) * temp_speed;
        }

        if (key == GLFW_KEY_A){
            camera.position -= HMM_Norm(HMM_Cross(camera_front, world_up)) * temp_speed; 
        }

        if (key == GLFW_KEY_W){
            camera.position += camera_front * temp_speed;
        }

        if (key == GLFW_KEY_S){
            camera.position -= camera_front * temp_speed;
        }
    }
}

#if 0
void ZetaVertsToEq(ZMath::Vec3D *zeta_verts, VertexData *vertex_data){

    // TOP
    AddVertexPosition(vertex_data, zeta_verts[4].x, zeta_verts[4].y, zeta_verts[4].z); // front top left
    AddVertexPosition(vertex_data, zeta_verts[7].x, zeta_verts[7].y, zeta_verts[7].z); // front top right
    AddVertexPosition(vertex_data, zeta_verts[6].x, zeta_verts[6].y, zeta_verts[6].z); // back top right
    AddVertexPosition(vertex_data, zeta_verts[5].x, zeta_verts[5].y, zeta_verts[5].z); // back top left

    // BOTTOM
    AddVertexPosition(vertex_data, zeta_verts[0].x, zeta_verts[0].y, zeta_verts[0].z); // back  bottom left
    AddVertexPosition(vertex_data, zeta_verts[3].x, zeta_verts[3].y, zeta_verts[3].z); // back bottom right
    AddVertexPosition(vertex_data, zeta_verts[2].x, zeta_verts[2].y, zeta_verts[2].z); // front bottom right
    AddVertexPosition(vertex_data, zeta_verts[1].x, zeta_verts[1].y, zeta_verts[1].z); // front bottom left

    // FRONT
    AddVertexPosition(vertex_data, zeta_verts[1].x, zeta_verts[1].y, zeta_verts[1].z); // front bottom left
    AddVertexPosition(vertex_data, zeta_verts[2].x, zeta_verts[2].y, zeta_verts[2].z); // front bottom right
    AddVertexPosition(vertex_data, zeta_verts[7].x, zeta_verts[7].y, zeta_verts[7].z); // front top right
    AddVertexPosition(vertex_data, zeta_verts[4].x, zeta_verts[4].y, zeta_verts[4].z); // front top left

    // BACK
    AddVertexPosition(vertex_data, zeta_verts[3].x, zeta_verts[3].y, zeta_verts[3].z); // back bottom right
    AddVertexPosition(vertex_data, zeta_verts[0].x, zeta_verts[0].y, zeta_verts[0].z); // back bottom left
    AddVertexPosition(vertex_data, zeta_verts[5].x, zeta_verts[5].y, zeta_verts[5].z); // back top left
    AddVertexPosition(vertex_data, zeta_verts[6].x, zeta_verts[6].y, zeta_verts[6].z); // back top right

    // RIGHT
    AddVertexPosition(vertex_data, zeta_verts[3].x, zeta_verts[3].y, zeta_verts[3].z); // back bottom right
    AddVertexPosition(vertex_data, zeta_verts[6].x, zeta_verts[6].y, zeta_verts[6].z); // back top right
    AddVertexPosition(vertex_data, zeta_verts[7].x, zeta_verts[7].y, zeta_verts[7].z); // front top right
    AddVertexPosition(vertex_data, zeta_verts[2].x, zeta_verts[2].y, zeta_verts[2].z); // front bottom right

    // LEFT
    AddVertexPosition(vertex_data, zeta_verts[0].x, zeta_verts[0].y, zeta_verts[0].z); // back bottom left
    AddVertexPosition(vertex_data, zeta_verts[1].x, zeta_verts[1].y, zeta_verts[1].z); // front bottom left
    AddVertexPosition(vertex_data, zeta_verts[4].x, zeta_verts[4].y, zeta_verts[4].z); // front top left
    AddVertexPosition(vertex_data, zeta_verts[5].x, zeta_verts[5].y, zeta_verts[5].z); // back top left

    vertex_data->index = 0;
    // TOP
    AddVertexIndice(vertex_data, 0, 1, 2);
    AddVertexIndice(vertex_data, 2, 3, 0);

    // BOTTOM
    AddVertexIndice(vertex_data, 4, 5, 6);
    AddVertexIndice(vertex_data, 6, 7, 4);

    // FRONT
    AddVertexIndice(vertex_data, 8, 9, 10);
    AddVertexIndice(vertex_data, 10, 11, 8);

    // BACK
    AddVertexIndice(vertex_data, 12, 13, 14);
    AddVertexIndice(vertex_data, 14, 15, 12);

    // RIGHT
    AddVertexIndice(vertex_data, 16, 17, 18);
    AddVertexIndice(vertex_data, 18, 19, 16);

    // LEFT
    AddVertexIndice(vertex_data, 20, 21, 22);
    AddVertexIndice(vertex_data, 22, 23, 20);

    // NOTE(Lenny): Calculations may not work for other shapes besides prisms :(
    // vertex_data->index = 0;
    float *positions = vertex_data->positions;
    vertex_data->len_normals = vertex_data->len_positions;
   
    for (int i = 0; i < vertex_data->len_indices; i += 3) {
        int index_1 = vertex_data->indices[i];
        int index_2 = vertex_data->indices[i + 1];
        int index_3 = vertex_data->indices[i + 2];

        HMM_Vec3 pos_1 = {positions[index_1 * 3], positions[index_1 * 3 + 1], positions[index_1 * 3 + 2]};
        HMM_Vec3 pos_2 = {positions[index_2 * 3], positions[index_2 * 3 + 1], positions[index_2 * 3 + 2]};
        HMM_Vec3 pos_3 = {positions[index_3 * 3], positions[index_3 * 3 + 1], positions[index_3 * 3 + 2]};

        HMM_Vec3 normal = HMM_Cross(pos_2 - pos_1, pos_3 - pos_1);
        normal = HMM_NormV3(normal);

        vertex_data->normals[index_1 * 3] += normal.X;
        vertex_data->normals[index_1 * 3 + 1] += normal.Y;
        vertex_data->normals[index_1 * 3 + 2] += normal.Z;
        
        
        vertex_data->normals[index_2 * 3] += normal.X;
        vertex_data->normals[index_2 * 3 + 1] += normal.Y;
        vertex_data->normals[index_2 * 3  + 2] += normal.Z;
        
        
        vertex_data->normals[index_3 * 3] += normal.X;
        vertex_data->normals[index_3 * 3 + 1] += normal.Y;
        vertex_data->normals[index_3 * 3 + 2] += normal.Z;

        int x = 0;
    }

    vertex_data->index = 0;
}

#endif

void app_start(void *window){
    SetEditMode(1);

    printf("Program Started\n");
    textures_manager = TexturesManager();


    // Shader Stuff
    // =====================================

    // test_shader = new Shader("web_v_shader.glsl", "web_f_shader.glsl");
    test_shader.program = LoadShaders("web_v_shader.glsl", "web_f_shader.glsl");
    glUseProgram(test_shader.program);
    
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW); 

    // >>>>>> Texture Stuff
    textures_manager.AddTexture("white.png", TEXTURE_WHITE);
    textures_manager.AddTexture("thin/stallTexture.png", TEXTURE_STALL);
    
    // LoadTextures("white.png");
    // model = load_obj_model("thin/stall.obj");
    // model = load_obj_model("cube.obj");
    
    camera.position.X = 1.0f;
    camera.position.Y = 50.0f;
    camera.position.Z = 10.0f;
    camera.pitch = -60.0f;
    camera.yaw = -90.0f;
    camera.speed = 10000.0f;

    CreateProjectionMatrix();

    // u_light_position = GetUniformLocation("light_position");
    // u_light_color = GetUniformLocation("light_color");
    unsigned int u_specular_strength = GetUniformLocation(&test_shader, "specular_strength");
    unsigned int u_reflectivity = GetUniformLocation(&test_shader, "reflectivity");
    // u_color = GetUniformLocation("u_color");

    // test_shader->LoadShineVariables(0.25f, 64.0f);
    SetUniformValue(u_specular_strength, 0.25f);
    SetUniformValue(u_reflectivity, 64.0f);

    glUseProgram(0);

    // ========================================
    Zeta::Cube cube1({-2, -2, -2}, {2, 2, 2}, 0, 0);
    Zeta::Cube ground_cube({-30.0f, -3.0f, -30.0f}, {30.0f, 3.0f, 30.0f}, 0, 0);

    test_entity = new Entity(HMM_Vec3{0, 6, -20.0f}, 1.0f, 0.0f, 0.0f, 0.0f, 
        Zeta::RigidBodyCollider::RIGID_CUBE_COLLIDER, &cube1);
    test_entity->color = {0.0f, 1.0f, 0.0f};
    test_entity->def_texture = TEXTURE_WHITE;

    light_entity = new Entity(HMM_Vec3{13, 13, -20.0f}, 1.0f, 0.0f, 0.0f, 0.0f,  
        Zeta::StaticBodyCollider::STATIC_CUBE_COLLIDER, &cube1);
    light_entity->color = {0.8f, 0.8f, 0.8f};
    light_entity->def_texture = TEXTURE_WHITE;
    
    ground_entity = new Entity(HMM_Vec3{0, -4, -20.0f}, 1.0f, 0.0f, 0.0f, 0.0f,  
        Zeta::StaticBodyCollider::STATIC_CUBE_COLLIDER, &ground_cube);
    ground_entity->color = {0.2f, 0.8f, 1.0f};
    ground_entity->def_texture = TEXTURE_WHITE;
    
    dragon_entity = new Entity(HMM_Vec3{10, 4, -10.0f}, 1.0f, 0.0f, 90.0f, 0.0f, 
        Zeta::StaticBodyCollider::STATIC_CUBE_COLLIDER, &cube1);
    dragon_entity->color = {1.0f, 0.0f, 1.0f};
    dragon_entity->def_texture = TEXTURE_WHITE;
    
    stall_entity = new Entity(HMM_Vec3{-11, 4, -5.0f}, 1.0f, 0.0f, 90.0f, 0.0f, 
        Zeta::StaticBodyCollider::STATIC_CUBE_COLLIDER, &cube1);
    stall_entity->color = {1.0f, 1.0f, 1.0f};
    stall_entity->def_texture = TEXTURE_STALL;

    test_cube_entity = new Entity(HMM_Vec3{11, 16, -5.0f}, 4.0f, 0.0f, 0.0f, 0.0f, 
        Zeta::StaticBodyCollider::STATIC_CUBE_COLLIDER, &cube1);
    test_cube_entity->color = {0.8f, 0.3f, 0.3f};
    test_cube_entity->def_texture = TEXTURE_WHITE;

    test_entity->Init();
    
    handler.addRigidBody(test_entity->rb);

    RawModel dragon_model = load_obj_model("thin/stall.obj", dragon_entity->color);
    RawModel stall_model = load_obj_model("thin/stall.obj", stall_entity->color);
    RawModel test_cube_model = load_obj_model("cube.obj", {1.0f, 1.0f, 1.0f, 1.0f});
    dragon_entity->Init(dragon_model);
    stall_entity->Init(stall_model);

    light_entity->Init(test_cube_model);
    test_cube_entity->Init(test_cube_model);
    ground_entity->Init();

    SetupTextRenderer(&trm);
    Setup2dRendering(&trm);
    im.window = (GLFWwindow *)window;
    
    // test_cube_entity->Init(test_cube_model);
}

float angle = 0.0f;
float dt_accum = 0.0f;
float dt_avg = 1.0f;
int dt_ticks = 0;
void app_update(float &time_step, float dt){
    global_dt = dt;
    im.dt += 1 * dt;
#if 1
    glUseProgram(test_shader.program);

    CreateViewMatrix();

    CreateProjectionMatrix();
    // test_shader->LoadProjectionMatrix(projection);
    unsigned int u_projection_matrix = GetUniformLocation(&test_shader, "projection_matrix");
    SetUniformValue(u_projection_matrix, projection);
    // test_shader->LoadViewMatrix(view_matrix);    
    unsigned int u_view_matrix = GetUniformLocation(&test_shader, "view_matrix");
    SetUniformValue(u_view_matrix, view_matrix);
    // test_shader->LoadCameraPosition(camera.position);
    // test_shader->LoadLight({light_entity->sb->pos.x, light_entity->sb->pos.y, light_entity->sb->pos.z}, {1.0f, 1.0f, 1.0f, 1.0f});
    unsigned int u_camera_position = GetUniformLocation(&test_shader, "camera_position");
    SetUniformValue(u_camera_position, camera.position);
    
    unsigned int u_light_position = GetUniformLocation(&test_shader, "light_position");
    SetUniformValue(u_light_position, HMM_Vec3{light_entity->sb->pos.x, light_entity->sb->pos.y, light_entity->sb->pos.z});
    
    unsigned int u_light_color = GetUniformLocation(&test_shader, "light_color");
    SetUniformValue(u_light_color, HMM_Vec3{1.0f, 1.0f, 1.0f});
    
    // ************
    render(light_entity, &textures_manager);    
    render(test_entity, &textures_manager);
    render(ground_entity, &textures_manager);
    render(dragon_entity, &textures_manager);
    render(stall_entity, &textures_manager);
    render(test_cube_entity, &textures_manager);
    
    // **************
    
    int physics_updates = handler.update(time_step);
    
    ZMath::Vec3D normal = {};
    float ground_cube_colliding = Zeta::CubeAndCube(test_entity->rb->collider.cube, ground_entity->sb->collider.cube, normal);
    
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
    
    glUseProgram(0);
    String dt_string = Create_String("dt : ");
    AddToString(&dt_string, dt_avg);
    // RenderText(&trm, Create_String("Zeta Has Text Now!!! Gig'em"), HMM_Vec3{255.0f, 0.0f, 0.0f}, HMM_Vec2{50.0f, 100.0f});
    // RenderText(&trm, Create_String("MORE TEXT. very good."), HMM_Vec3{195.0f, 155.0f, 55.0f}, HMM_Vec2{50.0f, 50.0f});
    // RenderText(&trm, dt_string, HMM_Vec3{115.0f, 195.0f, 55.0f}, HMM_Vec2{WINDOW_WIDTH - 450.0f, WINDOW_HEIGHT - 75.0f});
#endif

#if 0
    if(Button(app_update, &im, &trm,  Create_String("Settings"), WINDOW_WIDTH / 2.0f, (WINDOW_HEIGHT / 2.0f) + 100.0f, {0.3f, 0.3f, 0.3f, 1.0f})){
        printf("Settings!\n");
    }

    if(Button(&angle, &im, &trm,  Create_String("Quit"), HMM_Vec2{0.0f, 50.0f}, {0.3f, 0.3f, 0.3f, 1.0f})){
        printf("Quit!\n");
        glfwSetWindowShouldClose(im.window, GLFW_TRUE);
    }
#endif

    float x_pos = (WINDOW_WIDTH / 2.0) - (BUTTON_WIDTH / 2.0);
    float y_pos = 0.0f;
    static float rad = 50.0f;

    // x_pos = 200 + HMM_SinF(rad) * 20;
    // y_pos = 100 + HMM_SinF(rad) * 20;
    
    y_pos = 150;
    
    // rad += 10 * dt;
    
    if(Button((void *)1, &im, &trm,  Create_String("SETTINGS"), x_pos, y_pos + 200.0f, {0.3f, 0.3f, 0.3f, 1.0f})){
        printf("Button 1!\n");
    }

    if(Button((void *)2, &im, &trm,  Create_String("SAVE"), x_pos, y_pos + 100.0f, {0.3f, 0.3f, 0.3f, 1.0f})){
        printf("Button 2!\n");
    }

    if(Button((void *)3, &im, &trm,  Create_String("QUIT"), x_pos, y_pos, {0.3f, 0.3f, 0.3f, 1.0f})){
        printf("Button 3!\n");
    }

    Text(&trm, &im, 0.4f, Create_String("F P S : "), {100.0f, WINDOW_HEIGHT - 100.0f},  {255.0f, 100.0f, 0.0f});

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

