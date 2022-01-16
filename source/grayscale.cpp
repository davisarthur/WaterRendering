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

    unsigned int shaderProgram = loadShaders("shaders/grayscale.vs", "shaders/grayscale.fs");
    glUseProgram(shaderProgram);

    // read in mesh data
    float amplitude = 0.005;
    float Lx = 5.0;
    float Lz = 5.0;
    int M = 64;
    int N = 64;
    glm::vec2 wind_vector(2.0, 0.0);
    water_grid water(amplitude, Lx, Lz, M, N, wind_vector);
    float time = 0.0;
    float fps = 45.0;
    float delta_time = 1.0 / fps;
    vector<Triangle> triangles = water.gen_triangles();

    write_to_file("data/fourier_grid.txt", print_vector_2D(water.fourier_grid));
    write_to_file("data/water_grid.txt", print_vector_2D(water.position_grid));
    write_to_file("data/omega_grid.txt", print_vector_2D_real(water.omega_grid));
    write_to_file("data/slope_x.txt", print_vector_2D(water.slope_grid_x));
    write_to_file("data/slope_z.txt", print_vector_2D(water.slope_grid_z));
    
    int numBytes = triangles.size() * sizeof(triangles[0]);
    int vertexSize = sizeof(triangles[0].vertex1);

    glm::mat4 projMatrix = glm::ortho(-Lx / 2.0f, Lx / 2.0f - 2 * Lx / N, -Lz / 2.0f, Lz / 2.0f - 2 * Lz / N, -10.0f, 10.0f);

    GLint projID = glGetUniformLocation(shaderProgram, "proj");
    GLint minID = glGetUniformLocation(shaderProgram, "miny");
    GLint maxID = glGetUniformLocation(shaderProgram, "maxy");

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, numBytes, triangles.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertexSize, (void*)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, vertexSize, (void*)(sizeof(float) * 3));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

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
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // update geometry
        water.eval_grids(time);
        triangles = water.gen_triangles();
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, numBytes, triangles.data(), GL_STATIC_DRAW);

        // draw our first triangle
        glUniformMatrix4fv(projID, 1, GL_FALSE, glm::value_ptr(projMatrix));
        glUniform1f(minID, water.min);
        glUniform1f(maxID, water.max);
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
        std::string fnameTemp = "movies/grayscale/img" + std::to_string(n) + ".png";
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