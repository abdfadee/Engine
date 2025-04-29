#pragma once

#include <filesystem>
#include <glad/glad.h>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include "stbloader.h"
#include "../AssetManager.h"


using namespace std;


class Texture {
    unsigned int id;
    std::string path;

public:
    Texture (unsigned int id , string path) : id(id) , path(path) {}

    static void deactivateTextureUnit(GLuint unit) {
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    static Texture* T_2D (string path, bool gamma = false) {
        GLuint id = AssetManager::getTexture(path);
        if (id != -1)
            return new Texture(id, path);
        id = loadTexture(path.c_str(), gamma);
        AssetManager::addTexture(path, id);
        return new Texture(id,path);
    }

    static Texture* T_HDRI(string path) {
        GLuint id = AssetManager::getTexture(path);
        if (id != -1)
            return new Texture(id, path);
        id = loadHDRI(path.c_str());
        AssetManager::addTexture(path, id);
        return new Texture(id, path);
    }


	void bind(unsigned int textureUnit = 0) {
		glActiveTexture(GL_TEXTURE0 + textureUnit);
		glBindTexture(GL_TEXTURE_2D, id);
	}


    GLuint getId() { return id; }


    static GLuint loadTexture(const char* path, bool gamma = false , bool flip = false)
    {
        if (flip)
            stbi_set_flip_vertically_on_load(true);

        unsigned int textureID;
        glGenTextures(1, &textureID);

        int width, height, nrComponents;
        unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
        if (data)
        {
            GLenum format, internalFormat;
            if (nrComponents == 1) {
                format = GL_RED;
                internalFormat = GL_RED;
            }
            else if (nrComponents == 3) {
                format = GL_RGB;
                internalFormat = (gamma) ? GL_SRGB : GL_RGB;
            }
            else if (nrComponents == 4) {
                format = GL_RGBA;
                internalFormat = (gamma) ? GL_SRGB_ALPHA : GL_RGBA;
            }

            glBindTexture(GL_TEXTURE_2D, textureID);
            glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            glBindTexture(GL_TEXTURE_2D, 0);
            stbi_image_free(data);
        }
        else
        {
            cout << "Texture failed to load at path: " << path << endl;
            stbi_image_free(data);
        }

        return textureID;
    }


    static GLuint loadCubemap(vector<string> faces, bool gamma = false)
    {
        stbi_set_flip_vertically_on_load(true);

        unsigned int textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

        int width, height, nrComponents;
        for (unsigned int i = 0; i < faces.size(); i++)
        {
            unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrComponents, 0);
            if (data)
            {
                GLenum format, internalFormat;
                if (nrComponents == 1) {
                    format = GL_RED;
                    internalFormat = GL_RED;
                }
                else if (nrComponents == 3) {
                    format = GL_RGB;
                    internalFormat = (gamma) ? GL_SRGB : GL_RGB;
                }
                else if (nrComponents == 4) {
                    format = GL_RGBA;
                    internalFormat = (gamma) ? GL_SRGB_ALPHA : GL_RGBA;
                }

                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                    0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data
                );
                stbi_image_free(data);
            }
            else
            {
                std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
                stbi_image_free(data);
            }
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        return textureID;
    }


    static GLuint loadHDRI(const char* path) {
        stbi_set_flip_vertically_on_load(true);

        int width, height, nrComponents;
        float* data = stbi_loadf(path, &width, &height, &nrComponents, 0);
        unsigned int hdrTexture;
        if (data)
        {
            glGenTextures(1, &hdrTexture);
            glBindTexture(GL_TEXTURE_2D, hdrTexture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            stbi_image_free(data);
        }
        else
        {
            std::cout << "Failed to load HDR image." << std::endl;
        }

        return hdrTexture;
    }

};