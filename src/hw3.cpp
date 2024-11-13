#include "hw3.h"
#include "3rdparty/glad.h" // needs to be included before GLFW!
#include "3rdparty/glfw/include/GLFW/glfw3.h"
#include "hw3_scenes.h"
#include <fstream> 
#include <filesystem>
#include "shaders/shader.h"
#include "3rdparty/glm/gtc/matrix_transform.hpp"
#include "3rdparty/glm/glm.hpp"
#include "3rdparty/glm/gtc/type_ptr.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "3rdparty/glm/gtx/string_cast.hpp"

using namespace hw3;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void processInputCam(GLFWwindow *window, glm::vec3 &cameraPos, glm::vec3 &cameraFront, glm::vec3 &cameraUp);
std::string loadShaderSourceFromFile(const std::string& filepath);

void framebuffer_size_callback(GLFWwindow* window, int width, int height){
        glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window) {
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void processInputCam(GLFWwindow *window, glm::vec3 &cameraPos, glm::vec3 &cameraFront, glm::vec3 &cameraUp) {
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    const float cameraSpeed = 0.05f; // adjust accordingly
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

std::string loadShaderSourceFromFile(const std::string& filepath) {
    std::cout << "Attempting to open file: " << filepath << std::endl;
    std::ifstream shaderFile(filepath);
    if (!shaderFile.is_open()) {
        std::cerr << "Failed to open shader file: " << filepath << std::endl;
        return "";
    }

    std::stringstream shaderStream;
    shaderStream << shaderFile.rdbuf();  // Read file contents into stringstream
    shaderFile.close();
    
    return shaderStream.str();
}

void hw_3_1(const std::vector<std::string> &params) {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1280, 720, "HW3", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    }    

    glViewport(0, 0, 1280, 720);

    //Register callbacks here
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    //Render loop
    while(!glfwWindowShouldClose(window))
    {
        //input
        processInput(window);
        
        //rendering
        glClearColor(0.2f, 0.5f, 0.7f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        //end loop, swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();    
    }



}

void hw_3_2(const std::vector<std::string> &params) {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1280, 720, "HW3", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    }    

    glViewport(0, 0, 1280, 720);

    //Register callbacks here
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    //vertex data
    float vertices[] = {
        0.5f,  0.5f, 0.0f,  // top right
        0.5f, -0.5f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  // bottom left
        -0.5f,  0.5f, 0.0f   // top left 
    };

    unsigned int indices[] = {  // note that we start from 0!
        0, 1, 3   // first triangle
        //1, 2, 3    // second triangle
    }; 

    //vertex buffer & array objects
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);  
    glBindVertexArray(VAO);

    unsigned int VBO;
    glGenBuffers(1, &VBO); 
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    unsigned int EBO;
    glGenBuffers(1, &EBO);  
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); 

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    //shader initialization
    Shader shader("../src/shaders/hw3_2_vert.vert", "../src/shaders/hw3_2_frag.frag");

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);    

    //Render loop
    while(!glfwWindowShouldClose(window))
    {
        //input
        processInput(window);
        
        //rendering
        glClearColor(0.2f, 0.5f, 0.7f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        float timeValue = (glfwGetTime() * 10);
        shader.use();
        shader.setFloat("angle", timeValue);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(indices[0]), GL_UNSIGNED_INT, 0);

        //end loop, swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();    
    }

    glfwTerminate();
    return;
}

void hw_3_3(const std::vector<std::string> &params) {
    // HW 3.3: Render a scene
    if (params.size() == 0) {
        return;
    }

    Scene scene = parse_scene(params[0]);
    std::cout << scene << std::endl;

    int resx = scene.camera.resolution.x;
    int resy = scene.camera.resolution.y;
    float aspect = ((float)resx)/resy;

    glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f, 0.0f); 
    glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f, 0.0f);
    
    float s = scene.camera.s;
    float z_near = scene.camera.z_near;
    float z_far = 100.0;
    Vector3 bg = scene.background;

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    GLFWwindow* window = glfwCreateWindow(resx, resy, "HW3", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    }    

    glEnable(GL_MULTISAMPLE);  
    glViewport(0, 0, resx, resy);

    //Register callbacks here
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    //shader initialization
    Shader shader("../src/shaders/hw3_3_vert.vert", "../src/shaders/hw3_3_frag.frag");

    unsigned int VAOs[scene.meshes.size()];
    int count = 0;

    for(TriangleMesh mesh : scene.meshes){

        //filling arrays for vertices, colors, and indices
        std::vector<Vector3f> vertsVec = mesh.vertices;
        std::vector<Vector3i> indicesVec = mesh.faces;
        std::vector<Vector3f> colorsVec = mesh.vertex_colors;

        float verts[vertsVec.size() * 3];
        unsigned int indices[indicesVec.size() * 3];
        float colors[colorsVec.size() * 3];
        for(int i = 0; i < vertsVec.size(); i++){
            for(int j = 0; j < 3; j++){
                verts[i*3 + j] = vertsVec[i][j];
                colors[i*3 + j] = colorsVec[i][j];

                //std::cout << verts[i*3 + j] << std::endl;
            }
        }

        for(int i = 0; i < indicesVec.size(); i++){
            for(int j = 0; j < 3; j++){
                indices[i*3 + j] = indicesVec[i][j];
            }
        }

        //vertex buffer & array objects
        unsigned int VAO;
        glGenVertexArrays(1, &VAO);  
        glBindVertexArray(VAO);
        VAOs[count] = VAO;

        unsigned int VBO;
        glGenBuffers(1, &VBO); 
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

        unsigned int EBO;
        glGenBuffers(1, &EBO);  
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); 
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);    

        unsigned int CBO;
        glGenBuffers(1, &CBO); 
        glBindBuffer(GL_ARRAY_BUFFER, CBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1); 

        count++;
        
    }

    glEnable(GL_DEPTH_TEST);

    Matrix4x4 CW = scene.camera.cam_to_world;
    glm::mat4 cw = glm::mat4(1.0f);
    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 4; j++){
            cw[j][i] = CW(i, j);
        }
    }

    glm::mat4 viewDirect = glm::inverse(cw);
    std::cout << glm::to_string(viewDirect) << "          ";

    glm::vec4 camPos(0.0, 0.0, 0.0, 1.0);
    glm::vec4 camDir(0.0, 0.0, -1.0, 0.0);
    glm::vec4 worldCamPos = cw * camPos;
    glm::vec4 worldCamDir = cw * camDir;

    cameraPos.x = worldCamPos.x/worldCamPos.w;
    cameraPos.y = worldCamPos.y/worldCamPos.w;
    cameraPos.z = worldCamPos.z/worldCamPos.w;

    cameraFront.x = worldCamDir.x;
    cameraFront.y = worldCamDir.y;
    cameraFront.z = worldCamDir.z;

    bool printed = false;

    //Render loop
    while(!glfwWindowShouldClose(window))
    {
        //input
        processInputCam(window, cameraPos, cameraFront, cameraUp);
        
        //rendering
        shader.use();

        glClearColor(bg.x, bg.y, bg.z, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

        if(!printed){
            std::cout << glm::to_string(view) << std::endl;
            printed = true;
        }
        

        for(int i = 0; i < scene.meshes.size(); i++){
            glBindVertexArray(VAOs[i]);
            glm::mat4 model = glm::mat4(1.0f);
            for(int j = 0; j < 4; j++){
                for(int k = 0; k < 4; k++){
                    model[k][j] = scene.meshes[i].model_matrix(j, k);
                }
            }

            glm::mat4 proj = glm::perspective(2 * glm::atan(s), aspect, z_near, z_far);

            int modelLoc = glGetUniformLocation(shader.ID, "model");
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

            int viewLoc = glGetUniformLocation(shader.ID, "view");
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

            int projLoc = glGetUniformLocation(shader.ID, "projection");
            glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(proj));

            glDrawElements(GL_TRIANGLES, scene.meshes[i].faces.size() * 3, GL_UNSIGNED_INT, 0);
        }

        //end loop, swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();    
    }

    glfwTerminate();
    return;
}

void hw_3_4(const std::vector<std::string> &params) {
    // HW 3.4: Render a scene
    if (params.size() == 0) {
        return;
    }

    Scene scene = parse_scene(params[0]);
    std::cout << scene << std::endl;

    int resx = scene.camera.resolution.x;
    int resy = scene.camera.resolution.y;
    float aspect = ((float)resx)/resy;

    glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f, 0.0f); 
    glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f, 0.0f);
    
    float s = scene.camera.s;
    float z_near = scene.camera.z_near;
    float z_far = 100.0;
    Vector3 bg = scene.background;

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    GLFWwindow* window = glfwCreateWindow(resx, resy, "HW3", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    }    

    glEnable(GL_MULTISAMPLE);  
    glViewport(0, 0, resx, resy);

    //Register callbacks here
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    //shader initialization
    Shader shader("../src/shaders/hw3_4_vert.vert", "../src/shaders/hw3_4_frag.frag");

    unsigned int VAOs[scene.meshes.size()];
    int count = 0;

    //test comme

    for(TriangleMesh mesh : scene.meshes){

        //filling arrays for vertices, colors, and indices
        std::vector<Vector3f> vertsVec = mesh.vertices;
        std::vector<Vector3i> indicesVec = mesh.faces;
        std::vector<Vector3f> colorsVec = mesh.vertex_colors;
        std::vector<Vector3f> normalsVec = mesh.vertex_normals;

        float verts[vertsVec.size() * 3];
        unsigned int indices[indicesVec.size() * 3];
        float colors[colorsVec.size() * 3];
        float normals[normalsVec.size() * 3];
        for(int i = 0; i < vertsVec.size(); i++){
            for(int j = 0; j < 3; j++){
                verts[i*3 + j] = vertsVec[i][j];
                colors[i*3 + j] = colorsVec[i][j];
                normals[i*3 + j] = normalsVec[i][j];

                //std::cout << verts[i*3 + j] << std::endl;
            }
        }

        for(int i = 0; i < indicesVec.size(); i++){
            for(int j = 0; j < 3; j++){
                indices[i*3 + j] = indicesVec[i][j];
            }
        }

        //vertex buffer & array objects
        unsigned int VAO;
        glGenVertexArrays(1, &VAO);  
        glBindVertexArray(VAO);
        VAOs[count] = VAO;

        unsigned int VBO;
        glGenBuffers(1, &VBO); 
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

        unsigned int EBO;
        glGenBuffers(1, &EBO);  
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); 
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);    

        unsigned int CBO;
        glGenBuffers(1, &CBO); 
        glBindBuffer(GL_ARRAY_BUFFER, CBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1); 

        unsigned int NBO;
        glGenBuffers(1, &NBO); 
        glBindBuffer(GL_ARRAY_BUFFER, NBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW);

        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(2); 

        count++;
        
    }

    glEnable(GL_DEPTH_TEST);

    Matrix4x4 CW = scene.camera.cam_to_world;
    glm::mat4 cw = glm::mat4(1.0f);
    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 4; j++){
            cw[j][i] = CW(i, j);
        }
    }

    glm::mat4 viewDirect = glm::inverse(cw);
    std::cout << glm::to_string(viewDirect) << "          ";

    glm::vec4 camPos(0.0, 0.0, 0.0, 1.0);
    glm::vec4 camDir(0.0, 0.0, -1.0, 0.0);
    glm::vec4 worldCamPos = cw * camPos;
    glm::vec4 worldCamDir = cw * camDir;

    cameraPos.x = worldCamPos.x/worldCamPos.w;
    cameraPos.y = worldCamPos.y/worldCamPos.w;
    cameraPos.z = worldCamPos.z/worldCamPos.w;

    cameraFront.x = worldCamDir.x;
    cameraFront.y = worldCamDir.y;
    cameraFront.z = worldCamDir.z;

    //lighting variables
    glm::vec3 lightColor(0.6, 0.6, 0.6);
    glm::vec3 lightDir = glm::normalize(glm::vec3(1.0, 1.0, -1.0));

    //Render loop
    while(!glfwWindowShouldClose(window))
    {
        //input
        processInputCam(window, cameraPos, cameraFront, cameraUp);
        
        //rendering
        shader.use();

        glClearColor(bg.x, bg.y, bg.z, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp); 

        for(int i = 0; i < scene.meshes.size(); i++){
            glBindVertexArray(VAOs[i]);
            glm::mat4 model = glm::mat4(1.0f);
            for(int j = 0; j < 4; j++){
                for(int k = 0; k < 4; k++){
                    model[k][j] = scene.meshes[i].model_matrix(j, k);
                }
            }

            glm::mat4 proj = glm::perspective(2 * glm::atan(s), aspect, z_near, z_far);

            int modelLoc = glGetUniformLocation(shader.ID, "model");
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

            int viewLoc = glGetUniformLocation(shader.ID, "view");
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

            int projLoc = glGetUniformLocation(shader.ID, "projection");
            glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(proj));

            int lightColorLoc = glGetUniformLocation(shader.ID, "lightColor");
            glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

            int lightDirLoc = glGetUniformLocation(shader.ID, "lightDir");
            glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDir));
            
            int cameraPosLoc = glGetUniformLocation(shader.ID, "cameraPos");
            glUniform3fv(cameraPosLoc, 1, glm::value_ptr(cameraPos));

            glDrawElements(GL_TRIANGLES, scene.meshes[i].faces.size() * 3, GL_UNSIGNED_INT, 0);
        }

        //end loop, swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();    
    }

    glfwTerminate();
    return;
}
