/* material.h
 */


#ifndef MATERIAL_H
#define MATERIAL_H


#include "linalg.h"
#include "texture.h"
#include "gpuProgram.h"


class Material {

 public:

  char *  name;                 // material name
  char *  texName;              // texture filename (for debugging)
  char *  bumpMapName;          // bump map filename (for debugging)

  vec3  ka;                   // ambient reflectivity
  vec3  kd;                   // diffuse reflectivity
  vec3  ks;                   // specular reflectivity
  float   n;                    // shininess
  float   g;                    // glossiness in range 1 (a mirror) to 0 (a very rough surface)
  vec3  Ie;                   // emitted light
  float   alpha;                // opacity in [0,1] with 1 = opaque
  Texture *texture;             // texture map (= NULL if none)
  Texture *bumpMap;             // bump map (= NULL if none)

  Material() {}

  void setMaterialForOpenGL( GPUProgram *gpuProg );

  void setDefault() {
    name = "";
    texName = "";
    bumpMapName = "";
    ka = vec3(0.1,0.1,0.1);
    kd = vec3(0.7,0.7,0.7);
    ks = vec3(0.3,0.3,0.3);
    n = 200;
    g = 0.999;
    Ie = vec3(0,0,0);
    texture = NULL;
    bumpMap = NULL;
  }

  friend ostream& operator << ( ostream& stream, Material const& m );
  friend istream& operator >> ( istream& stream, Material & m );
  
};


#endif
