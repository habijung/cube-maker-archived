//
// Created by habi on 2023-08-21.
//

#ifndef CUBE_MAKER_SHADER_H
#define CUBE_MAKER_SHADER_H

#include "glad/glad.h"
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
        int success;
        char infoLog[512];

        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glCompileShader(vertex);
        glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(vertex, sizeof(infoLog), NULL, infoLog);
            cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
                 << infoLog << endl;
        }

        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);
        glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(fragment, sizeof(infoLog), NULL, infoLog);
            cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
                 << infoLog << endl;
        }

        ID = glCreateProgram();
        glAttachShader(ID, vertex);
        glAttachShader(ID, fragment);
        glLinkProgram(ID);
        glGetProgramiv(ID, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(ID, sizeof(infoLog), NULL, infoLog);
            cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
                 << infoLog << endl;
        }

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
};

#endif//CUBE_MAKER_SHADER_H
