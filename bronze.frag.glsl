# version 120
/*
 * Michael Shafae
 * mshafae at fullerton.edu
 * 
 * A simple Blinn-Phong shader with two light sources.
 * This file is the fragment shader which calculates
 * the halfway vector between the viewer and light source
 * vector and then calculates the color on the surface
 * given the material properties passed in from the CPU program.
 *
 * For more information see:
 *     <http://en.wikipedia.org/wiki/Blinn–Phong_shading_model>
 *
 * $Id: blinn_phong.frag.glsl 4891 2014-04-05 08:36:23Z mshafae $
 *
 * Be aware that for this course, we are limiting ourselves to
 * GLSL v.1.2. This is not at all the contemporary shading
 * programming environment, but it offers the greatest degree
 * of compatability.
 *
 * Please do not use syntax from GLSL > 1.2 for any homework
 * submission.
 *
 */

// These are passed from the vertex shader to here, the fragment shader
// In later versions of GLSL these are 'in' variables.
varying vec3 myNormal;
varying vec4 myVertex;

// These are passed in from the CPU program, camera_control_*.cpp
uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;
uniform mat4 normalMatrix;
uniform vec4 light0_position;
uniform vec4 light0_color;
uniform vec4 light1_position;
uniform vec4 light1_color;

vec4 ComputeColor (const in vec3 eyeDirection, const in vec4 whatColor, const in vec4 pleaseColor, const in vec3 reflection) {

  float rVal, gVal, bVal;
  float vDotR = dot(eyeDirection, reflection);

  rVal = whatColor.x * pow(( (vDotR + 1.0f) / 2 ), pleaseColor.x);
  gVal = whatColor.y * pow(( (vDotR + 1.0f) / 2 ), pleaseColor.y);
  bVal = whatColor.z * pow(( (vDotR + 1.0f) / 2 ), pleaseColor.z);

  vec4 retval = vec4(rVal, gVal, bVal, 1.0f);
  return retval;
}      

void main (void){

  const vec3 eyepos = vec3(0,0,0);
  vec4 _mypos = modelViewMatrix * myVertex;
  vec3 mypos = _mypos.xyz / _mypos.w;
  vec3 eyedirn = normalize(eyepos - mypos);

  // Light 0, point
  vec3 position0 = light0_position.xyz / light0_position.w;
  vec3 direction0 = normalize (position0 - mypos);
  vec3 reflection0 = reflect(direction0, myNormal);

  // Light 1, point
  vec3 position1 = light1_position.xyz / light1_position.w;
  vec3 direction1 = normalize (position1 - mypos);
  vec3 reflection1 = reflect(direction1, myNormal);

  vec4 what = vec4(1.0f, 1.0f, 1.0f, 1.0f);   // base color?
  vec4 please = vec4(1.0f, 2.0f, 20.0f, 1.0f);    // filter determines which of the base color values live or die

  vec4 color0 = ComputeColor(eyedirn, what, please, reflection0);
  vec4 color1 = ComputeColor(eyedirn, what, please, reflection1);
  gl_FragColor = color0 + color1;
}
