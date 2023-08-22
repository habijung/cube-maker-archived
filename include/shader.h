//
// Created by habi on 2023-08-21.
//

#ifndef CUBE_MAKER_SHADER_H
#define CUBE_MAKER_SHADER_H

#include "glad/glad.h"
#include "glm/glm.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
using namespace std;

class Shader {
public:
    unsigned int ID;

    // Constructor reads and builds the shader
    Shader(const char *vertexPath, const char *fragmentPath) {
        string vertexCode;
        string fragmentCode;
        ifstream vShaderFile;
        ifstream fShaderFile;

        vShaderFile.exceptions(ifstream::failbit | ifstream::badbit);
        fShaderFile.exceptions(ifstream::failbit | ifstream::badbit);

        try {
            // Open files
            vShaderFile.open(vertexPath);
            fShaderFile.open(fragmentPath);
            stringstream vShaderStream, fShaderStream;

            // Read file's buffer contents into streams
            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();
            vShaderFile.close();
            fShaderFile.close();

            // Convert stream into string
            vertexCode = vShaderStream.str();
            fragmentCode = fShaderStream.str();
        } catch (ifstream::failure e) {
            cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ: " << e.what() << endl;
        }

        const char *vShaderCode = vertexCode.c_str();
        const char *fShaderCode = fragmentCode.c_str();
        unsigned int vertex, fragment;

        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glCompileShader(vertex);
        checkCompileErrors(vertex, "VERTEX");

        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);
        checkCompileErrors(fragment, "FRAGMENT");

        ID = glCreateProgram();
        glAttachShader(ID, vertex);
        glAttachShader(ID, fragment);
        glLinkProgram(ID);
        checkCompileErrors(ID, "PROGRAM");

        glDeleteShader(vertex);
        glDeleteShader(fragment);
    };

    // Use/Activate the shader
    void use() {
        glUseProgram(ID);
    };

    // Utility uniform functions
    void setBool(const string &name, bool value) const {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int) value);
    };
    void setInt(const string &name, int value) const {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
    };
    void setFloat(const string &name, float value) const {
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
    };
    void setMat4(const string &name, glm::mat4 &mat) const {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

private:
    void checkCompileErrors(unsigned int shader, string type) {
        int success;
        char infoLog[1024];

        if (type != "PROGRAM") {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success) {
                glGetShaderInfoLog(shader, sizeof(infoLog), NULL, infoLog);
                cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n"
                     << infoLog << "\n -- --------------------------------------------------- -- "
                     << endl;
            }
        } else {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success) {
                glGetProgramInfoLog(shader, sizeof(infoLog), NULL, infoLog);
                cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n"
                     << infoLog << "\n -- --------------------------------------------------- -- "
                     << endl;
            }
        }
    }
};

#endif//CUBE_MAKER_SHADER_H
