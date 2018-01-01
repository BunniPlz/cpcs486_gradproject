// 
// Collision detection
// 
// Squares with different textures bounce around the scene and collide with each other.
//
//

#include <tuple>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <string>
#include <sys/time.h>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_inverse.hpp>

#include "GLFWApp.h"
#include "GLSLShader.h"
#include "glut_teapot.h"

#include "SpinningLight.h"
#include "Camera.h"
#include "UtahTeapot.h"
#include "Square.h"

void msglVersion(void){
  fprintf(stderr, "OpenGL Version Information:\n");
  fprintf(stderr, "\tVendor: %s\n", glGetString(GL_VENDOR));
  fprintf(stderr, "\tRenderer: %s\n", glGetString(GL_RENDERER));
  fprintf(stderr, "\tOpenGL Version: %s\n", glGetString(GL_VERSION));
  fprintf(stderr, "\tGLSL Version: %s\n",
  glGetString(GL_SHADING_LANGUAGE_VERSION));
}


class CollisionDetectionApp : public GLFWApp{
private:
  float rotationDelta;

  glm::vec3 centerPosition;
  Camera* currentCamera;
  // main point of view camera
  Camera mainCamera;
  // Bird's eye view camera
  Camera bevCamera;

  glm::mat4 modelViewMatrix;
  glm::mat4 projectionMatrix;
  glm::mat4 normalMatrix;
  
  GLSLProgram shaderProgram;

  SpinningLight light0;
  SpinningLight light1; 

  UtahTeapot* teapots[20];
  const int teapotCount = 20;

  Square* squares[10];
  const unsigned int squareCount = 10;

  Square* boundingBox[4];

  Texture *texhappyface, *texwhitesquare;

  bool debugMaterialFlag;

  // Variables to set uniform params for lighting fragment shader 
  unsigned int uModelViewMatrix;
  unsigned int uProjectionMatrix;
  unsigned int uNormalMatrix;
  unsigned int uLight0_position;
  unsigned int uLight0_color;
  unsigned int uLight1_position;
  unsigned int uLight1_color;
  unsigned int uAmbient;
  unsigned int uDiffuse;
  unsigned int uSpecular;
  unsigned int uShininess;
  unsigned int uTexture;
  
public:
  CollisionDetectionApp(int argc, char* argv[]) :
    GLFWApp(argc, argv, std::string("Collision Detection").c_str( ), 
            600, 600){ }
  
  void initCenterPosition( ){
    centerPosition = glm::vec3(0.0, 0.0, 0.0);
  }
  
  void initTeapots( ){
    std::srand(time(NULL));
    for(int i = 0; i < teapotCount; i++){
      glm::vec3 _diffuseColor = glm::linearRand(glm::vec3(0.2), glm::vec3(1.0));
      //std::cerr << glm::to_string(_diffuseColor) << std::endl;
      glm::vec4 diffuseColor = glm::vec4(_diffuseColor, 1.0);
      Material* m = new Material(glm::vec4(0.2, 0.2, 0.2, 1.0), diffuseColor, glm::vec4(1.0, 1.0, 1.0, 1.0), 100.0);
      glm::vec2 xy = glm::diskRand(30.0);
      glm::vec3 position = glm::vec3(xy, 0.0);
      teapots[i] = new UtahTeapot(position, 1.0, m);
      teapots[i]->visible = true;
    }
  }

  void initSquares() {
    texhappyface = new Texture("textures/awesomeface.png");
    texwhitesquare = new Texture("textures/whitesquare.png");
    std::srand(time(NULL));
    for(int i = 0; i < squareCount; i++){
      glm::vec3 _diffuseColor = glm::linearRand(glm::vec3(0.2), glm::vec3(1.0));
      glm::vec4 diffuseColor = glm::vec4(_diffuseColor, 1.0);
      Material* m = new Material(glm::vec4(0.2, 0.2, 0.2, 1.0), diffuseColor, glm::vec4(1.0, 1.0, 1.0, 1.0), 100.0);
      //squares[i]->texture = t;
      glm::vec2 xy = glm::diskRand(0.3);
      glm::vec3 position = glm::vec3(xy, 0.0);
      squares[i] = new Square(position, glm::vec3(1.0), m);
      float randSpeedFactor = 0.001 + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(0.100-0.001)));
      squares[i]->speedFactor = randSpeedFactor;
      squares[i]->visible = true;
      for(int j = 0; j < i; j++) {
        while(squares[i]->isColliding(*squares[j])) {
          glm::vec2 xy = glm::diskRand(3.0);
          position = glm::vec3(xy, 0.0);
          squares[i]->position = position;
          printf("square%i.isColliding(square%i), regenerating coordinates\n", i, j);
        }
      }
      squares[i]->velocity = -glm::normalize(position);
      printf("square #: %i, visible: %i, position: %.2f, %.2f, %.2f\n", i, squares[i]->visible, position.x, position.y, position.z);
      std::cerr << glm::to_string(_diffuseColor) << std::endl;
    }
    
    /*
    glm::vec4 diffuseColor = glm::vec4(0.5, 0.0, 0.0, 1.0);
    Material* m0 = new Material(glm::vec4(0.2, 0.2, 0.2, 1.0), diffuseColor, glm::vec4(1.0, 1.0, 1.0, 1.0), 100.0);
    glm::vec3 position = glm::vec3(-1.54, 0.82, 0.0);
    //glm::vec3 position = glm::vec3(-0.68, 1.60, 0.0);
    squares[0] = new Square(position, 1.0, m0);

    diffuseColor = glm::vec4(0.0, 0.5, 0.0, 1.0);
    Material* m1 = new Material(glm::vec4(0.2, 0.2, 0.2, 1.0), diffuseColor, glm::vec4(1.0, 1.0, 1.0, 1.0), 100.0);
    position = glm::vec3(-0.68, 1.60, 0.0);
    //position = glm::vec3(-1.54, 0.82, 0.0);
    squares[1] = new Square(position, 1.0, m1);

    printf("redSquare.isColliding(greenSquare): %i\n", squares[0]->isColliding(*squares[1]));
    */
  }

  void initBoundingBox() {
    glm::vec4 diffuseColor = glm::vec4(1.0, 1.0, 1.0, 1.0);
    Material* m = new Material(glm::vec4(0.2, 0.2, 0.2, 1.0), diffuseColor, glm::vec4(1.0, 1.0, 1.0, 1.0), 100.0);
    boundingBox[0] = new Square(glm::vec3(- 8.0,  0.0,  0.0), glm::vec3(1.0, 18.0, 0.0), m);  //left
    boundingBox[1] = new Square(glm::vec3(  0.0,  8.0,  0.0), glm::vec3(18.0, 1.0, 0.0), m);  //top
    boundingBox[2] = new Square(glm::vec3(  8.0,  0.0,  0.0), glm::vec3(1.0, 18.0, 0.0), m);  //right
    boundingBox[3] = new Square(glm::vec3(  0.0, -8.0,  0.0), glm::vec3(18.0, 1.0, 0.0), m);  //bottom
  }

  void initCamera( ){
    // Main point of view camera
    mainCamera = Camera(glm::vec3(0.0, 0.0, 20.0), glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 0.0), 45.0, 0.2, 50.0);
    // Bird's eye view camera
    bevCamera =  Camera(glm::vec3(0.0, 0.0, 70.0), glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 0.0), 45.0, 10.0, 150.0);
    currentCamera = &mainCamera;
  }

  void initRotationDelta( ){
    rotationDelta = 0.05;
  }
   
  void initLights( ){
    glm::vec3 color0(1.0, 1.0, 1.0);
    glm::vec3 position0(0.0, 20.0, 20.0);
    glm::vec3 color1(1.0, 1.0, 1.0);
    glm::vec3 position1(0.0, 20.0, -20.0);
    light0 = SpinningLight(color0, position0, centerPosition);
    light1 = SpinningLight(color1, position1, centerPosition);
  }

  bool begin( ){
    msglError( );
    initCenterPosition( );
    initBoundingBox();
    initSquares( );
    initCamera( );
    initRotationDelta( );
    initLights( );
    debugMaterialFlag = false;

    // Load shader programs
    const char* vertexShaderSource = "blinn_phong.vert.glsl";
    const char* fragmentShaderSource = "blinn_phong.frag.glsl";
    FragmentShader fragmentShader(fragmentShaderSource);
    VertexShader vertexShader(vertexShaderSource);
    shaderProgram.attach(vertexShader);
    shaderProgram.attach(fragmentShader);
    shaderProgram.link( );
    shaderProgram.activate( );

    /*
    GLuint tex1ID;
    int texture_width, texture_height, nrChannels;
    glEnable(GL_TEXTURE_2D);
    glGenTextures(1, &tex1ID);
    glBindTexture(GL_TEXTURE_2D, tex1ID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    unsigned char* data = stbi_load("textures/awesomeface.png", &texture_width, &texture_height, &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture_width, texture_height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
    */
    
    printf("Shader program built from %s and %s.\n",
           vertexShaderSource, fragmentShaderSource);
    if( shaderProgram.isActive( ) ){
      printf("Shader program is loaded and active with id %d.\n", shaderProgram.id( ) );
    }else{
      printf("Shader program is not active, id: %d\n.", shaderProgram.id( ));
    }
    
    // Set up uniform variables for the shader program
    uModelViewMatrix = glGetUniformLocation(shaderProgram.id( ), "modelViewMatrix");
    uProjectionMatrix = glGetUniformLocation(shaderProgram.id( ), "projectionMatrix");
    uNormalMatrix = glGetUniformLocation(shaderProgram.id( ), "normalMatrix");
    uLight0_position = glGetUniformLocation(shaderProgram.id( ), "light0_position");
    uLight0_color = glGetUniformLocation(shaderProgram.id( ), "light0_color");
    uLight1_position = glGetUniformLocation(shaderProgram.id( ), "light1_position");
    uLight1_color = glGetUniformLocation(shaderProgram.id( ), "light1_color");
    uAmbient = glGetUniformLocation(shaderProgram.id( ), "ambient");
    uDiffuse = glGetUniformLocation(shaderProgram.id( ), "diffuse");
    uSpecular = glGetUniformLocation(shaderProgram.id( ), "specular");
    uShininess = glGetUniformLocation(shaderProgram.id( ), "shininess");
    uTexture = glGetUniformLocation(shaderProgram.id(), "texture");

    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    msglVersion( );
    
    return !msglError( );
  }
  
  bool end( ){
    windowShouldClose( );
    return true;
  }

  void activateUniformsWithTexture(glm::vec4& _light0, glm::vec4& _light1, Material* m, Texture* t) {
    activateUniforms(_light0, _light1, m);
    t->bind();
    glUniform1i(uTexture, 0);

  }
  
  void activateUniforms(glm::vec4& _light0, glm::vec4& _light1, Material* m){
    glUniformMatrix4fv(uModelViewMatrix, 1, false, glm::value_ptr(modelViewMatrix));
    glUniformMatrix4fv(uProjectionMatrix, 1, false, glm::value_ptr(projectionMatrix));
    glUniformMatrix4fv(uNormalMatrix, 1, false, glm::value_ptr(normalMatrix));

    glUniform4fv(uLight0_position, 1, glm::value_ptr(_light0));
    glUniform4fv(uLight0_color, 1, glm::value_ptr(light0.color( )));
    
    glUniform4fv(uLight1_position, 1, glm::value_ptr(_light1));
    glUniform4fv(uLight1_color, 1, glm::value_ptr(light1.color( )));

    glUniform4fv(uAmbient, 1, glm::value_ptr(m->ambient));
    glUniform4fv(uDiffuse, 1, glm::value_ptr(m->diffuse));
    glUniform4fv(uSpecular, 1, glm::value_ptr(m->specular));
    glUniform1f(uShininess, m->shininess);
  }

  bool render( ){
    glm::vec4 _light0;
    glm::vec4 _light1;
    glm::mat4 lookAtMatrix;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    std::tuple<int, int> w = windowSize( );
    double ratio = double(std::get<0>(w)) / double(std::get<1>(w));


    mainCamera.perspectiveMatrix(projectionMatrix, ratio);

    mainCamera.lookAtMatrix(lookAtMatrix);

    // Set light & material properties for the teapot;
    // lights are transformed by current modelview matrix
    // such that they are positioned correctly in the scene.
    _light0 = lookAtMatrix * light0.position4( );
    _light1 = lookAtMatrix * light1.position4( );

    for(int i = 0; i < 4; i++) {
      modelViewMatrix = glm::translate(lookAtMatrix, boundingBox[i]->position);
      modelViewMatrix = glm::scale(modelViewMatrix, boundingBox[i]->scale*glm::vec3(1.0));
      normalMatrix = glm::inverseTranspose(modelViewMatrix);
      shaderProgram.activate( );
      activateUniformsWithTexture(_light0, _light1, boundingBox[i]->material, texwhitesquare);
      boundingBox[i]->draw();
      texwhitesquare->unbind();
    }
    
    for(int i = 0; i < squareCount; i++){
      if(squares[i]->visible){
        for(int j = 0; j < 4; j++) {
          if(squares[i]->isColliding(*boundingBox[j])) {
            //printf("Square# %i collided with wall# %i\n", i, j);
            glm::vec3 surfaceNormal;
            switch(j) {
              case 0: //left wall
                surfaceNormal = glm::vec3(1.0, 0.0, 0.0);
                squares[i]->position = glm::vec3(-7.0, squares[i]->position.y, 0.0);
                break;
              case 1: //top wall
                surfaceNormal = glm::vec3(0.0, -1.0, 0.0);
                squares[i]->position = glm::vec3(squares[i]->position.x, 7.0, 0.0);
                break;
              case 2: //right wall
                surfaceNormal = glm::vec3(-1.0, 0.0, 0.0);
                squares[i]->position = glm::vec3(7.0, squares[i]->position.y, 0.0);
                break;
              case 3: //bottom wall
                surfaceNormal = glm::vec3(0.0, 1.0, 0.0);
                squares[i]->position = glm::vec3(squares[i]->position.x, -7.0, 0.0);
                break;
              default:
                break;
            }
            squares[i]->velocity = glm::reflect(squares[i]->velocity, surfaceNormal);
          }
        }

        for(int j = 0; j < squareCount; j++) {
          if(i != j) {
            if(squares[i]->isColliding(*squares[j])) {
              //find direction of collision
              glm::vec3 ji = squares[i]->position - squares[j]->position;
              float dot = glm::dot(ji, squares[j]->up);
              float angle = acos(dot);
              float direction = glm::dot(glm::cross(ji, squares[j]->up), squares[j]->up); // coming from right or left
              glm::vec3 surfaceNormal;
              if(direction < 0) {// coming from the left
                angle = -angle;
              }

              if(fabs(angle) < 45.0) { // approaching from above
                surfaceNormal = glm::vec3(0.0, 1.0, 0.0);
              } else if(fabs(angle) > 135.0f) {  // approaching from below
                surfaceNormal = glm::vec3(0.0, -1.0, 0.0);
              } else if(angle < 0) {  // approaching from the left
                surfaceNormal = glm::vec3(-1.0, 0.0, 0.0);
              } else{ // approaching from the right, probably
                surfaceNormal = glm::vec3(1.0, 0.0, 0.0);
              }
              
              squares[i]->velocity = glm::reflect(squares[i]->velocity, surfaceNormal);
              squares[j]->velocity = glm::reflect(squares[j]->velocity, -surfaceNormal);
            }
          }
        }

        modelViewMatrix = glm::translate(lookAtMatrix, squares[i]->position);
        modelViewMatrix = glm::scale(modelViewMatrix, squares[i]->scale*glm::vec3(1.0));
        normalMatrix = glm::inverseTranspose(modelViewMatrix);
        shaderProgram.activate( );
        activateUniformsWithTexture(_light0, _light1, squares[i]->material, texhappyface);
        squares[i]->draw( );
        texhappyface->unbind();
        //printf("square #: %i, visible: %i, position: %.2f, %.2f, %.2f\n", i, squares[i]->visible, squares[i]->position.x, squares[i]->position.y, squares[i]->position.z);
      }
    }

    if(isKeyPressed('R')){
      /*initEyePosition( );
      initUpVector( );*/
      initCamera( );
      initSquares();
      initRotationDelta( );
      initLights( );  
      printf("Eye position, up vector and rotation delta reset.\n");
    }

    return !msglError( );
  }
    
};

int main(int argc, char* argv[]){
  CollisionDetectionApp app(argc, argv);
  return app();
}

