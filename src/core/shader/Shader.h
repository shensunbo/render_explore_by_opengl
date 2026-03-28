#pragma once

#include "gl/gl_headers.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "log/mylog.h"

#define CHECK_GLES_STATUS()                \
    do {                                   \
        auto err = glGetError();           \
        if (err != GL_NO_ERROR) {          \
            LOG_E("gl error: {:#x}", err); \
        }                                  \
    } while (0)

class Shader
{
public:
    Shader(const char* vertexPath, const char* fragmentPath);

    void use() const { glUseProgram(ID); }

    /**
     * @brief Set a named uniform. Supported types: bool, int, unsigned int,
     *        float, glm::vec2/3/4, glm::mat2/3/4.
     */
    template<typename T>
    void set(const std::string& name, const T& value) const;

    GLuint getBlockIndex(const std::string& block_name) {
        return glGetUniformBlockIndex(ID, block_name.c_str());
    }

    void uniformBlockBind(GLuint block_index, GLuint layout_binding) {
        glUniformBlockBinding(ID, block_index, layout_binding);
    }

private:
    void checkCompileErrors(GLuint shader, std::string type);

public:
    unsigned int ID;
};

// ---------------------------------------------------------------------------
// Explicit specializations (inline so they live entirely in this header)
// ---------------------------------------------------------------------------

template<> inline void Shader::set<bool>(const std::string& name, const bool& v) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), static_cast<int>(v));
}
template<> inline void Shader::set<int>(const std::string& name, const int& v) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), v);
}
template<> inline void Shader::set<unsigned int>(const std::string& name, const unsigned int& v) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), static_cast<int>(v));
}
template<> inline void Shader::set<float>(const std::string& name, const float& v) const {
    glUniform1f(glGetUniformLocation(ID, name.c_str()), v);
}
template<> inline void Shader::set<glm::vec2>(const std::string& name, const glm::vec2& v) const {
    glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &v[0]);
}
template<> inline void Shader::set<glm::vec3>(const std::string& name, const glm::vec3& v) const {
    glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &v[0]);
}
template<> inline void Shader::set<glm::vec4>(const std::string& name, const glm::vec4& v) const {
    glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &v[0]);
}
template<> inline void Shader::set<glm::mat2>(const std::string& name, const glm::mat2& v) const {
    glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &v[0][0]);
}
template<> inline void Shader::set<glm::mat3>(const std::string& name, const glm::mat3& v) const {
    glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &v[0][0]);
}
template<> inline void Shader::set<glm::mat4>(const std::string& name, const glm::mat4& v) const {
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &v[0][0]);
}
