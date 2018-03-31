/* wavefrontobj.h
 */


#ifndef WAVEFRONTOBJ_H
#define WAVEFRONTOBJ_H


#include "object.h"
#include "wavefront.h"


class WavefrontObj : public Object {

 public:

  wfModel *obj;

  seq<Material*> mats;		/* one material for each wavefront group */

  WavefrontObj() {}

  WavefrontObj( const char *filename ) {

    // Read the object

    obj = new wfModel( filename, MIPMAP_LINEAR );

    // Create a copy of the material library

    copyWfMaterialsToObject();
  }

  void input( istream &stream ) {
    cerr << "No streaming input method for Wavefront objects" << endl;
    exit(1);
  }

  void output( ostream &stream ) {
    stream << "wavefront" << endl
	   << "  " << obj->pathname << endl;
  }

  void renderGL( GPUProgram * gpuProg, mat4 &WCS_to_VCS, mat4 &VCS_to_CCS ) {
    obj->draw( gpuProg, WCS_to_VCS, VCS_to_CCS );
  }
  
  void renderGL() {
    cerr << "No renderGL method without gpuProgram" << endl;
    exit(1);
  }

  bool rayInt( vec3 rayStart, vec3 rayDir, int objPartIndex,
	       vec3 & intPoint, vec3 & intNorm, float & intParam, Material * &mat, int &intPartIndex );
  
  vec3 textureColour( vec3 p, int objPartIndex, float &alpha );
  void copyWfMaterialsToObject();
};

#endif
