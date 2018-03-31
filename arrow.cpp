/* arrow.cpp
 */


#include "headers.h"

#include <math.h>
#include "arrow.h"


// Render with openGL

#define PI          3.1415926
#define PHI_STEP    PI/16.0

#define BODY_LENGTH 0.94
#define BODY_RADIUS 0.007

#define HEAD_LENGTH 0.06
#define HEAD_RADIUS 0.015

void Arrow::draw( GPUProgram *gpuProg, mat4 &OCS_to_WCS, mat4 &WCS_to_VCS, mat4 &VCS_to_CCS )

{
  if (VAO == 0) { // create VBO

    // Count the quads
    //
    // Do it this way to avoid discrepences with roundoff

    numTriangles = 0;
    for (float phi=0; phi<1.99*PI; phi+=PHI_STEP)
      numTriangles += 5;

    vec3 attribs[2*3*numTriangles]; // 2 attributes x 3 vertices x numTriangles

    vec3 *pos  = &attribs[0];
    vec3 *norm = &attribs[3*numTriangles];

    vec3 tip(0,0,BODY_LENGTH+HEAD_LENGTH);
    vec3 bodyTip(0,0,BODY_LENGTH);
    vec3 tail(0,0,0);
    vec3 nDown(0,0,-1);

    float tailRadius = (BODY_LENGTH + HEAD_LENGTH) / HEAD_LENGTH * HEAD_RADIUS;
    float headAngle = atan2( tailRadius, BODY_LENGTH + HEAD_LENGTH );
    
    for (float phi=0; phi<1.99*PI; phi+=PHI_STEP) {

      // body normals

      vec3 N0( cos(phi),          sin(phi),          0 );
      vec3 N1( cos(phi+PHI_STEP), sin(phi+PHI_STEP), 0 );

      // body points
      
      vec3 B0t = BODY_RADIUS * N0;
      vec3 B1t = BODY_RADIUS * N1;
      vec3 B0h = B0t + vec3(0,0,BODY_LENGTH);
      vec3 B1h = B1t + vec3(0,0,BODY_LENGTH);

      // head points

      vec3 H0 = HEAD_RADIUS * N0 + vec3(0,0,BODY_LENGTH);
      vec3 H1 = HEAD_RADIUS * N1 + vec3(0,0,BODY_LENGTH);

      vec3 N0h = cos(headAngle) * N0 + sin(headAngle) * vec3(0,0,1);
      vec3 N1h = cos(headAngle) * N1 + sin(headAngle) * vec3(0,0,1);

      // bottom cap

      *(pos++) = tail;
      *(pos++) = B1t;
      *(pos++) = B0t;

      *(norm++) = nDown;
      *(norm++) = nDown;
      *(norm++) = nDown;

      // edge quads

      *(pos++) = B0t;
      *(pos++) = B1t;
      *(pos++) = B1h;

      *(norm++) = N0;
      *(norm++) = N1;
      *(norm++) = N1;

      *(pos++) = B0t;
      *(pos++) = B1h;
      *(pos++) = B0h;

      *(norm++) = N0;
      *(norm++) = N1;
      *(norm++) = N0;

      // underside of arrowhead

      *(pos++) = bodyTip;
      *(pos++) = H1;
      *(pos++) = H0;

      *(norm++) = nDown;
      *(norm++) = nDown;
      *(norm++) = nDown;

      // arrowhead

      *(pos++) = tip;
      *(pos++) = H0;
      *(pos++) = H1;

      *(norm++) = N0h;
      *(norm++) = N0h;
      *(norm++) = N1h;
    }

    glGenVertexArrays( 1, &VAO );
    glBindVertexArray( VAO );

    GLuint VBO;
    glGenBuffers( 1, &VBO );
    glBindBuffer( GL_ARRAY_BUFFER, VBO );
    glBufferData( GL_ARRAY_BUFFER, sizeof(attribs), attribs, GL_STATIC_DRAW );

    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, 0 );
    glEnableVertexAttribArray( 0 );
    
    glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 0, (void*) (sizeof(vec3)*3*numTriangles) );
    glEnableVertexAttribArray( 1 );

    glBindBuffer( GL_ARRAY_BUFFER, 0 );
    glBindVertexArray( 0 );
  }

  // Draw

  mat->setMaterialForOpenGL( gpuProg );

  mat4 MV = WCS_to_VCS * OCS_to_WCS;
  gpuProg->setMat4( "MV", MV );

  mat4 MVP = VCS_to_CCS * MV;
  gpuProg->setMat4( "MVP", MVP );

  glBindVertexArray( VAO );
  glDrawArrays( GL_TRIANGLES, 0, 3*numTriangles );
  glBindVertexArray( 0 );
}
