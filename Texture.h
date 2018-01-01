#include <iostream>
#include <glm/vec3.hpp>

#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

class Texture{
public:
  GLuint texID;

  Texture(const char *filename) {
    int texture_width, texture_height, nrChannels;
    glEnable(GL_TEXTURE_2D);
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    unsigned char* data = stbi_load(filename, &texture_width, &texture_height, &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture_width, texture_height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
    //unbind();
  }

  ~Texture() {
    glBindTexture(GL_TEXTURE_2D, 0);
    glDeleteTextures(1, &texID);
  }

  void bind() {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texID);
  }

  void unbind() {
    glBindTexture(GL_TEXTURE_2D, 0);
  }

};

#endif