#include <iostream>

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_inverse.hpp>

#include "Material.h"
#include "Texture.h"
#include <vector>
#include <algorithm>
#include <math.h>


class Square{

public:
    glm::vec3 position;
    glm::vec3 up;
    glm::vec3 forward;
    glm::vec3 scale;  //scale 1 = unit square
    Material *material;
    Texture *texture;
    bool visible;
    float speedFactor;
    glm::vec3 velocity;

    // initialized with face facing +z direction, top edge normal pointing to at +y direction
    Square(glm::vec3 pos, float s, Material* m): position(pos), scale(glm::vec3(s)){
      material = m;
      visible = true;
      up = glm::vec3(0.0, 1.0, 0.0);
      forward = glm::vec3(0.0, 0.0, 1.0);
      velocity = glm::vec3(0.0, 0.0, 0.0);
      speedFactor = 0.0;
      init();
    }

    Square(glm::vec3 pos, glm::vec3 s, Material* m): position(pos), scale(s){
      material = m;
      visible = true;
      up = glm::vec3(0.0, 1.0, 0.0);
      forward = glm::vec3(0.0, 0.0, 1.0);
      velocity = glm::vec3(0.0, 0.0, 0.0);
      speedFactor = 0.0;
      init();
    }

    Square():position(glm::vec3(0, 0, 0)), scale(1.0), speedFactor(0.0) {
      material = new Material(glm::vec4(0.2, 0.2, 0.2, 1.0), glm::vec4(0.5, 0.5, 0.5, 1.0), glm::vec4(1.0, 1.0, 1.0, 1.0), 100.0);
      visible = true;
      up = glm::vec3(0.0, 0.0, 1.0);
      forward = glm::vec3(0.0, 1.0, 0.0);
      velocity = glm::vec3(0.0, 0.0, 0.0);
      speedFactor = 0.0;
      init();
    }

    ~Square() {
        delete material;
    }

    bool isColliding(Square& s) {
      
      //if squares are rotated
      /*
      glm::mat4 _thisLookAt, _sLookAt;
      _thisLookAt = lookAtMatrix(_thisLookAt);
      _sLookAt = s->lookAtMatrix(_sLookAt);
      //glm::vec4 thisPos = _thisLookAt * glm::vec4(this->position, 0.0);
      //glm::vec4 sPos = _sLookAt * glm::(s.position, 0.0);
      */

      std::vector<glm::vec4> thisVertices(4);
      std::vector<glm::vec4> sVertices(4);

      thisVertices[0] = glm::vec4(upperRightVertex(), 0.0);
      thisVertices[1] = glm::vec4(upperLeftVertex(), 0.0);
      thisVertices[2] = glm::vec4(lowerLeftVertex(), 0.0);
      thisVertices[3] = glm::vec4(lowerRightVertex(), 0.0);

      sVertices[0] = glm::vec4(s.upperRightVertex(), 0.0);
      sVertices[1] = glm::vec4(s.upperLeftVertex(), 0.0);
      sVertices[2] = glm::vec4(s.lowerLeftVertex(), 0.0);
      sVertices[3] = glm::vec4(s.lowerRightVertex(), 0.0);

      std::vector<glm::vec3> axes;
      // get unique normals of this square - square assumed for now
      axes.insert(axes.begin(), this->up);  //normal of top edge, along verticle edges
      axes.insert(axes.begin(), glm::normalize(glm::cross(this->up, this->forward))); //normal of right edge, along horizontal edges
      // get unique normals of other square
      axes.insert(axes.begin(), s.up);  //normal of top edge, along verticle edges
      axes.insert(axes.begin(), glm::normalize(glm::cross(s.up, s.forward))); //normal of right edge, along horizontal edges

      for(int i = 0; i < axes.size(); i++) {
        // project vertices onto axis to find min and max of square (rotation assumed)
        // no rotation assumed for now
        float thisMin, thisMax, sMin, sMax;

        std::vector<float> thisMagnitudes(4);
        std::vector<float> sMagnitudes(4);
        for(int j = 0; j < thisMagnitudes.size(); j++) {
          float dot = glm::dot(glm::vec3(thisVertices[j]), axes[i]);
          thisMagnitudes[j] = (glm::length(thisVertices[j]) * dot);
          //printf("thisMag: %.3f\n", thisMagnitudes[j]);
        }

        for(int j = 0; j < sMagnitudes.size(); j++) {
          float dot = glm::dot(glm::vec3(sVertices[j]), axes[i]);
          sMagnitudes[j] = (glm::length(sVertices[j]) * dot);
          //printf("sMag: %.3f\n", sMagnitudes[j]);
        }

        thisMin = *std::min_element(thisMagnitudes.begin(), thisMagnitudes.end());
        thisMax = *std::max_element(thisMagnitudes.begin(), thisMagnitudes.end());

        sMin = *std::min_element(sMagnitudes.begin(), sMagnitudes.end());
        sMax = *std::max_element(sMagnitudes.begin(), sMagnitudes.end());
        //printf("thisMin: %.3f, thisMax: %.3f, sMin: %.3f, sMax: %.3f\n", thisMin, thisMax, sMin, sMax);

        // check if there is separation on this axis. If yes, return false.
        // floating point comparison tho
        if(sMax < thisMin || sMin > thisMax) {
          return false;
        }

      }

      return true;  // no separation on any unique normal axis

    }

    void lookAtMatrix(glm::mat4& m) {
      m = glm::lookAt(position, forward, up);
    }

    void update() {
      position = position + velocity*speedFactor;
    }

    void draw() {
      update();
      //glBindVertexArray(vao);
      //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
      //printf("xyz: %.2f, %.2f, %.2f\n", position.x, position.y, position.z);
      glBegin(GL_TRIANGLE_STRIP);
        glTexCoord2f(vertexData[6], vertexData[7]);
        glVertex3f(vertexData[0], vertexData[1], vertexData[2]);
        glTexCoord2f(vertexData[14], vertexData[15]);
        glVertex3f(vertexData[8], vertexData[9], vertexData[10]);
        glTexCoord2f(vertexData[30], vertexData[31]);
        glVertex3f(vertexData[24], vertexData[25], vertexData[26]);
        glTexCoord2f(vertexData[14], vertexData[15]);
        glVertex3f(vertexData[8], vertexData[9], vertexData[10]);
        glTexCoord2f(vertexData[22], vertexData[23]);
        glVertex3f(vertexData[16], vertexData[17], vertexData[18]);
        glTexCoord2f(vertexData[30], vertexData[31]);
        glVertex3f(vertexData[24], vertexData[25], vertexData[26]);
      glEnd();
    }

    // in local coordinates
    glm::vec3 upperRightVertex(){
      return glm::vec3(position.x+(vertexData[0]*scale.x), position.y+(vertexData[1]*scale.y), position.z+vertexData[2]);
    }

    glm::vec3 lowerRightVertex() {
      return glm::vec3(position.x+(vertexData[8]*scale.x), position.y+(vertexData[9]*scale.y), position.z+vertexData[10]);
    }

    glm::vec3 lowerLeftVertex() {
      return glm::vec3(position.x+(vertexData[16]*scale.x), position.y+(vertexData[17]*scale.y), position.z+vertexData[18]);
    }

    glm::vec3 upperLeftVertex() {
      return glm::vec3(position.x+(vertexData[24]*scale.x), position.y+(vertexData[25]*scale.y), position.z+vertexData[26]);
    }



private:

    unsigned int vao;
    unsigned int vbo;
    unsigned int ebo;

    void init() {
      /*
      glGenVertexArrays(1, &vao);
      glGenBuffers(1, &vbo);
      glBindVertexArray(vao);
      glBindBuffer(GL_ARRAY_BUFFER, vbo);
      glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexData), indexData, GL_STATIC_DRAW);

      // vertex position
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
      glEnableVertexAttribArray(0);
      // vertex color
      glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3* sizeof(float)));
      glEnableVertexAttribArray(1);
      // texture coord attribute
      glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
      glEnableVertexAttribArray(2);
      */
    }
/*
    static float vertexData[] = {
      // positions          // texture coords
       0.5f,  0.5f, 0.0f,   1.0f, 1.0f,   // top right
       0.5f, -0.5f, 0.0f,   1.0f, 0.0f,   // bottom right
      -0.5f, -0.5f, 0.0f,   0.0f, 0.0f,   // bottom left
      -0.5f,  0.5f, 0.0f,   0.0f, 1.0f    // top left 
    };
*/
/*
    const static float vertexData[] = {
    // positions          // colors           // texture coords
       0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
       0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
      -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
      -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left 
    };

    const static unsigned int indexData[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };
*/
    float vertexData[32] = {
    // positions            // colors           // texture coords
       0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
       0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
      -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
      -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left 
    };

    unsigned int indexData[6] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };
};