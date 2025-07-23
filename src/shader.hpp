#ifndef SHADER_HPP
#define SHADER_HPP

#include <string>
#include <GL/glew.h>

class Shader {
    public:
        GLuint ID;

        Shader(const char* vertexPath, const char* fragmentPath);
        void use();
        void setFloat(const std::string &name, float value) const;
        void setVec2(const std::string &name, float x, float y) const;
};

#endif