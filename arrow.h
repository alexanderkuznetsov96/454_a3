/* arrow.h
 * 
 * A generic arrow pointing up the z axis with length 1.
 */


#ifndef ARROW_H
#define ARROW_H


#include "object.h"
#include "material.h"


class Arrow {

  GLuint   VAO;
  int      numTriangles;

 public:

  Material *mat;

  Arrow() {
    VAO = 0;
    mat = new Material();
    mat->setDefault();
  }

  void draw( GPUProgram *prog, mat4 &OCS_to_WCS, mat4 &WCS_to_VCS, mat4 &VCS_to_CCS );
};

#endif
