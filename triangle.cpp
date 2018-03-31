/* triangle.C
 */


#include "headers.h"
#include "triangle.h"
#include "main.h"
#include "texture.h"


// Compute plane/ray intersection, and then the local coordinates to
// see whether the intersection point is inside.

bool Triangle::rayInt( vec3 rayStart, vec3 rayDir, int objPartIndex,
		       vec3 & intPoint, vec3 & intNorm, float & intParam, Material * &mat, int &intPartIndex )

{
  float param;
  vec3 point, lc;

  // Compute ray/plane intersection

  float dn = rayDir * faceNormal;

  if (fabs(dn) < 0.0001) 	// *** CHANGED TO ALLOW INTERSECTION FROM BEHIND TRIANGLE ***
    return false;		// ray is parallel to plane

  param = (dist - rayStart*faceNormal) / dn;
  if (param < 0)
    return false;		// plane is behind starting point

  point = rayStart + param * rayDir;

  // Compute barycentric coords

  lc = localCoords( point );

  if (lc.x < 0 || lc.y < 0 || lc.x + lc.y > 1)
    return false;		// outside of triangle

  intParam = param;
  intPoint = point;
  mat      = this->mat;

  // Find the normal with bump mapping

  if (mat->bumpMap != NULL) {
    intNorm = faceNormal;	// NOT YET IMPLEMENTED!
    return true;
  }

  // No bump mapping: Find the normal as interpolated

  if (verts[0].normal.x == 0 && verts[0].normal.y == 0 && verts[0].normal.z == 0)
    intNorm = faceNormal;
  else {
    vec3 bc = barycentricCoords( point );
    intNorm = bc.x * verts[0].normal + bc.y * verts[1].normal + bc.z * verts[2].normal;
    intNorm.normalize();
  }

  return true;
}


// Determine the texture colour at a point


vec3 Triangle::textureColour( vec3 p, int objPartIndex, float &alpha )

{
  // No texture map?

  if (mat->texture == NULL) {
    alpha = 1;
    return vec3(1,1,1);
  }

  // Texture map:

  vec3 bc = barycentricCoords( p );
  vec3 texCoords = bc.x * verts[0].texCoords + bc.y * verts[1].texCoords + bc.z * verts[2].texCoords;

  return mat->texture->texel( texCoords.x, texCoords.y, alpha );
}

// Compute the barycentric coordinates of a point which lies on the
// plane of the triangle: p = ua + vb + wc where a,b,c are the first,
// second, and third vertices of the triangle.
//
// See http://research.microsoft.com/~hollasch/cgindex/math/barycentric.html

vec3 Triangle::barycentricCoords( vec3 p )

{
  float w = barycentricFactor * (((verts[1].position-verts[0].position) ^ (p - verts[0].position)) * faceNormal);
  float v = barycentricFactor * (((p - verts[0].position) ^ (verts[2].position - verts[0].position)) * faceNormal);
  float u = 1.0 - v - w;

  return vec3( u, v, w );
}


// Compute the local coordinates of a point which lies on the plane of
// the triangle: p = s (b-a) + t (c-a) where a,b,c are the first,
// second, and third vertices of the triangle.  A vec3 (s,t,0) is
// returned (you can ignore the 0).

vec3 Triangle::localCoords( vec3 p )

{
  vec3 bc = barycentricCoords( p );
  return vec3( bc.y, bc.z, 0 );
}


// Output a triangle

void Triangle::output( ostream &stream ) const

{
  stream << "triangle" << endl
	 << "  " << verts[0] << endl
	 << "  " << verts[1] << endl
	 << "  " << verts[2] << endl;
}


// Input a triangle

void Triangle::input( istream &stream )

{
  skipComments( stream );  stream >> verts[0];
  skipComments( stream );  stream >> verts[1];
  skipComments( stream );  stream >> verts[2];

  // Compute the triangle normal and the barycentric factor which is
  // used in computing the barycentric coordinates of a point.

  faceNormal = (verts[1].position - verts[0].position) ^ (verts[2].position - verts[0].position);
  barycentricFactor = 1.0/faceNormal.length();
  faceNormal = barycentricFactor * faceNormal;
  dist   = verts[0].position * faceNormal;
}


// Render with openGL

void Triangle::renderGL( GPUProgram *prog, mat4 &WCS_to_VCS, mat4 &VCS_to_CCS )

{
  if (VAO == 0) { // create VBO
    
    vec3 attribs[3*3];

    for (int i=0; i<3; i++) {
      attribs[i]   = verts[i].position;  // position
      attribs[i+3] = faceNormal;	 // normal
      attribs[i+6] = verts[i].texCoords; // texcoords (3D !!!)
    }

    glGenVertexArrays( 1, &VAO );
    glBindVertexArray( VAO );

    GLuint VBO;
    glGenBuffers( 1, &VBO );
    glBindBuffer( GL_ARRAY_BUFFER, VBO );
    glBufferData( GL_ARRAY_BUFFER, sizeof(attribs), attribs, GL_STATIC_DRAW );

    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, 0 );
    glEnableVertexAttribArray( 0 );
    
    glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 0, (void*) (sizeof(vec3)*3) );
    glEnableVertexAttribArray( 1 );

    glVertexAttribPointer( 2, 3, GL_FLOAT, GL_FALSE, 0, (void*) (sizeof(vec3)*6) );
    glEnableVertexAttribArray( 2 );

    glBindBuffer( GL_ARRAY_BUFFER, 0 );
    glBindVertexArray( 0 );
  }

  // Draw

  mat->setMaterialForOpenGL( prog );

  gpuProg->setMat4( "MV",  WCS_to_VCS );

  mat4 MVP = VCS_to_CCS * WCS_to_VCS;
  gpuProg->setMat4( "MVP", MVP );

  glBindVertexArray( VAO );
  glDrawArrays( GL_TRIANGLES, 0, 3 );
  glBindVertexArray( 0 );
}


vec3 Triangle::pointFromBarycentricCoords( float a, float b, float c )

{
  return a * verts[0].position + b * verts[1].position + c * verts[2].position;
}
