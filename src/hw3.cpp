#include "hw3.h"
#include "3rdparty/glad.h" // needs to be included before GLFW!
#include "3rdparty/glfw/include/GLFW/glfw3.h"
#include "hw3_scenes.h"
#include <fstream> 
#include <filesystem>

using namespace hw3;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
std::string loadShaderSourceFromFile(const std::string& filepath);

void framebuffer_size_callback(GLFWwindow* window, int width, int height){
        glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window) {
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
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
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.0f,  0.5f, 0.0f
    }; 

    //vertex buffer & array objects
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);  
    glBindVertexArray(VAO);

    unsigned int VBO;
    glGenBuffers(1, &VBO);  
    glBindBuffer(GL_ARRAY_BUFFER, VBO); 

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    //shader initialization
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);

    std::string vertsourcestr = loadShaderSourceFromFile("../src//shaders/hw3_2_vert.vert");
    const char* vertsource = vertsourcestr.c_str();
    glShaderSource(vertexShader, 1, &vertsource, NULL);
    glCompileShader(vertexShader);
    int  success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cerr << "Vertex Shader Compilation Failed:\n" << infoLog << std::endl;
    }

    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    std::string fragsourcestr = loadShaderSourceFromFile("../src/shaders/hw3_2_frag.frag");
    const char* fragsource = fragsourcestr.c_str();
    glShaderSource(fragmentShader, 1, &fragsource, NULL);
    glCompileShader(fragmentShader);
    int  successf;
    char infoLogf[512];
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &successf);
    if (!successf) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLogf);
        std::cerr << "Frag Shader Compilation Failed:\n" << infoLog << std::endl;
    }

    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);    
    glBindVertexArray(0);

    //Render loop
    while(!glfwWindowShouldClose(window))
    {
        //input
        processInput(window);
        
        //rendering
        glClearColor(0.2f, 0.5f, 0.7f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

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
}

void hw_3_4(const std::vector<std::string> &params) {
    // HW 3.4: Render a scene with lighting
    if (params.size() == 0) {
        return;
    }

    Scene scene = parse_scene(params[0]);
    std::cout << scene << std::endl;
}
