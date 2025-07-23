#include <fstream>
#include <sstream>
#include <iostream>
#include <GL/glew.h>
#include "shader.hpp"

static const char* infileVert =
    R"(#version 430 core
    layout(location = 0) in vec2 aPos;

    out vec2 screenPos;

    void main() {
        gl_Position = vec4(aPos, 0.0, 1.0);
        
        screenPos = aPos;
    })";

static const char* infileFrag =
    R"(#version 430 core
    in vec2 screenPos;
    out vec4 FragColor;

    uniform vec2 uLight;    // in [0,1]
    uniform float uRadius;  // in [0,1]

    void main() {
        vec4 lightColor = vec4(1.0, 0.5, 1.0, 1.0);
        float d = length(abs(screenPos - uLight));
        float intensity = 1.0 - smoothstep(uRadius * 0.8, uRadius, d);
        FragColor = lightColor * intensity;
    })";

Shader::Shader(const char* Vert, const char* Frag) {
    GLuint v = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(v, 1, &infileVert, nullptr);
    // glShaderSource(v, 1, &Vert, nullptr);
    glCompileShader(v);

    GLint ok;
    glGetShaderiv(v, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[512]; glGetShaderInfoLog(v,512,nullptr,log);
        std::cerr << "Vert compile error:\n" << log << "\n";
    }

    GLuint f = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(f, 1, &infileFrag, nullptr);
    // glShaderSource(f, 1, &Frag, nullptr);
    glCompileShader(f);

    glGetShaderiv(f, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[512]; glGetShaderInfoLog(f,512,nullptr,log);
        std::cerr << "Frag compile error:\n" << log << "\n";
    }

    ID = glCreateProgram();
    glAttachShader(ID, v);
    glAttachShader(ID, f);
    glLinkProgram(ID);

    glGetProgramiv(ID, GL_LINK_STATUS, &ok);
    if (!ok) {
        char log[512]; glGetProgramInfoLog(ID,512,nullptr,log);
        std::cerr << "Link error:\n" << log << "\n";
    }

    glDeleteShader(v);
    glDeleteShader(f);
}

void Shader::use() {
    glUseProgram(ID);
}

void Shader::setFloat(const std::string &name, float value) const {
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setVec2(const std::string &name, float x, float y) const {
    glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
}