// Based on templates from learnopengl.com
#include "helperFunctions.h"
#include <GL/glew.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "glm/ext.hpp"
#include <math.h>
#include <iostream>
#include <fstream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
unsigned int loadShaders(string vertex_fname, string frag_fname);
void saveImage(char* filepath, GLFWwindow* w);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int main() {
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // // glew: load all OpenGL function pointers
    glewInit();

    unsigned int shaderProgram = loadShaders("shaders/source.vs", "shaders/source.fs");
    glUseProgram(shaderProgram);

    // read in mesh data
    float amplitude = 0.003;
    float Lx = 5.0;
    float Lz = 5.0;
    int M = 64;
    int N = 64;
    glm::vec2 wind_vector(5.0, 0.0);
    water_grid water(amplitude, Lx, Lz, M, N, wind_vector);
    float time = 0.0;
    float fps = 30.0;
    float delta_time = 1.0 / fps;
    vector<Triangle> triangles = water.gen_triangles();
    
    int numBytes = triangles.size() * sizeof(triangles[0]);
    int vertexSize = sizeof(triangles[0].vertex1);

    float fov = 60.0f * M_PI / 180.0f;
    float aspect = (float) SCR_WIDTH / SCR_HEIGHT;
    float znear = 0.1;
    float zfar = 1000.0;
    glm::vec3 eye = glm::vec3(6.0, 2.0, 0.0);
    glm::vec3 viewing = glm::vec3(0.0, 0.0, 0.0);
    glm::vec3 up = glm::vec3(0.0, 1.0, 0.0);
    glm::mat4 lookAt = glm::lookAt(eye, viewing, up);
    glm::mat4 projMatrix = glm::perspective(fov, aspect, znear, zfar);
    glm::mat4 transformMatrix = projMatrix * lookAt;
    GLint pMatID = glGetUniformLocation(shaderProgram, "transformMatrix");
    GLint eyeID = glGetUniformLocation(shaderProgram, "eye");

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, numBytes, triangles.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertexSize, (void*)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, vertexSize, (void*)(sizeof(float) * 3));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glEnable(GL_DEPTH_TEST);  

    // uncomment this call to draw in wireframe polygons.
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    int n = 0;

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window)) {
        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.4, 0.6, 0.8, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // update geometry
        water.eval_grids(time);
        triangles = water.gen_triangles();
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, numBytes, triangles.data(), GL_STATIC_DRAW);

        // draw our first triangle
        glUniformMatrix4fv(pMatID, 1, GL_FALSE, glm::value_ptr(transformMatrix));
        glUniform3f(eyeID, eye.x, eye.y, eye.z);
        glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
        glDrawArrays(GL_TRIANGLES, 0, triangles.size() * 3);
        // glBindVertexArray(0); // no need to unbind it every time 
 
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
        time += delta_time;
        

        // save image
        /*
        std::string fnameTemp = "movies/render/img" + std::to_string(n) + ".png";
        char fname[fnameTemp.length()];
        strcpy(fname, fnameTemp.c_str());
        saveImage(fname, window);
        n++;
        */
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

unsigned int loadShaders(string vertex_fname, string frag_fname) {
    string vertexShaderSourceString = readFile(vertex_fname);
    string fragmentShaderSourceString = readFile(frag_fname);
    char* vertexShaderSource = &vertexShaderSourceString[0];
    char* fragmentShaderSource = &fragmentShaderSourceString[0];

    // build and compile our shader program
    // ------------------------------------
    // vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // link shaders
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    glUseProgram(shaderProgram);
    return shaderProgram;
}

void saveImage(char* filepath, GLFWwindow* w) {
   int width, height;
   glfwGetFramebufferSize(w, &width, &height);
   GLsizei nrChannels = 3;
   GLsizei stride = nrChannels * width;
   stride += (stride % 4) ? (4 - stride % 4) : 0;
   GLsizei bufferSize = stride * height;
   std::vector<char> buffer(bufferSize);
   glPixelStorei(GL_PACK_ALIGNMENT, 4);
   glReadBuffer(GL_BACK);
   glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, buffer.data());
   stbi_flip_vertically_on_write(true);
   stbi_write_png(filepath, width, height, nrChannels, buffer.data(), stride);
}