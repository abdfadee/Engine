#pragma once
#include<glad/glad.h>
#include<glm/glm.hpp>
#include<string>
#include<fstream>
#include<sstream>
#include<iostream>
#include<cerrno>


using namespace std;


class Shader {
public:
    GLuint ID;

    Shader(GLuint shaderId) : ID(shaderId) {}

    Shader(const char* vertexPath, const char* fragmentPath , const char* geometryPath = NULL)
    {
        int  success;
        char infoLog[512];


        // Vertex Shader Program
        GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
        std::string vertexShaderCode = get_file_contents(vertexPath);
        const char* vertexShaderSource = vertexShaderCode.c_str();
        glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
        glCompileShader(vertexShader);
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        }


        // Fragment Shader Program
        GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        std::string fragmentShaderCode = get_file_contents(fragmentPath);
        const char* fragmentShaderSource = fragmentShaderCode.c_str();
        glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
        glCompileShader(fragmentShader);
        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::Fragment::COMPILATION_FAILED\n" << infoLog << std::endl;
        }


        // Geometry Shader Program
        GLuint geometryShader;
        if (geometryPath != NULL) {
            geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
            std::string geometryShaderCode = get_file_contents(geometryPath);
            const char* geometryShaderSource = geometryShaderCode.c_str();
            glShaderSource(geometryShader, 1, &geometryShaderSource, NULL);
            glCompileShader(geometryShader);
            glGetShaderiv(geometryShader, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(geometryShader, 512, NULL, infoLog);
                std::cout << "ERROR::SHADER::Geometry::COMPILATION_FAILED\n" << infoLog << std::endl;
            }
        }


        ID = glCreateProgram();
        glAttachShader(ID, vertexShader);
        glAttachShader(ID, fragmentShader);
        if (geometryPath != NULL) glAttachShader(ID, geometryShader);
        glLinkProgram(ID);
        glGetProgramiv(ID, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(ID, 512, NULL, infoLog);
        }

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
    }


    // activate the shader
    // ------------------------------------------------------------------------
    void use() const
    {
        glUseProgram(ID);
    }


    // utility uniform buffer object functions
    // ------------------------------------------------------------------------
    void bindBlock(string buffer_name, GLuint blockBind) const {
        GLuint blockIndex = glGetUniformBlockIndex(ID, buffer_name.c_str());
        glUniformBlockBinding(ID, blockIndex, blockBind);
    }


    // utility uniform functions
    // ------------------------------------------------------------------------
    void setBool(const std::string& name, bool value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
    }
    // ------------------------------------------------------------------------
    void setInt(const std::string& name, int value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
    }
    // ------------------------------------------------------------------------
    void setFloat(const std::string& name, float value) const
    {
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
    }
    // ------------------------------------------------------------------------
    void setVec2(const std::string& name, const glm::vec2& value) const
    {
        glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    void setVec2(const std::string& name, float x, float y) const
    {
        glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
    }
    // ------------------------------------------------------------------------
    void setVec3(const std::string& name, const glm::vec3& value) const
    {
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    void setVec3(const std::string& name, float x, float y, float z) const
    {
        glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
    }
    // ------------------------------------------------------------------------
    void setVec4(const std::string& name, const glm::vec4& value) const
    {
        glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    void setVec4(const std::string& name, float x, float y, float z, float w) const
    {
        glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
    }
    // ------------------------------------------------------------------------
    void setMat2(const std::string& name, const glm::mat2& mat) const
    {
        glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    // ------------------------------------------------------------------------
    void setMat3(const std::string& name, const glm::mat3& mat) const
    {
        glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    // ------------------------------------------------------------------------
    void setMat4(const std::string& name, const glm::mat4& mat) const
    {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }


    // remove the shader
    // ------------------------------------------------------------------------
    void remove() const
    {
        glDeleteProgram(ID);
    }


    std::string get_file_contents(const char* filename)
    {
        std::ifstream in(filename, std::ios::binary);
        if (in)
        {
            std::string contents;
            in.seekg(0, std::ios::end);
            contents.resize(in.tellg());
            in.seekg(0, std::ios::beg);
            in.read(&contents[0], contents.size());
            in.close();
            return(contents);
        }
        throw(errno);
    }

};