/* scene.C
 */


#include "headers.h"
#ifndef WIN32
  #include <unistd.h>
#endif

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "scene.h"
#include "rtWindow.h"
#include "sphere.h"
#include "triangle.h"
#include "wavefrontobj.h"
#include "light.h"
#include "font.h"
#include "main.h"
#include "material.h"
#include "arrow.h"


#ifndef MAXFLOAT
  #define MAXFLOAT 9999999
#endif


vec3 backgroundColour(1,1,1);
vec3 blackColour(0,0,0);


#define NUM_SOFT_SHADOW_RAYS 50


// Find the first object intersected

bool Scene::findFirstObjectInt( vec3 rayStart, vec3 rayDir, int thisObjIndex, int thisObjPartIndex,
				vec3 &P, vec3 &N, float &param, int &objIndex, int &objPartIndex, Material *&mat )

{
  if (storingRays)
    storedRays.add( rayStart );

  param = MAXFLOAT;

  for (int i=0; i<objects.size(); i++) {

    WavefrontObj* wfo = dynamic_cast<WavefrontObj*>( objects[i] );

    if (wfo || i != thisObjIndex) {	// don't check for int with the originating object
      vec3 point, normal;
      Material *intMat;
      float t;
      int intPartIndex;

      if (objects[i]->rayInt( rayStart, rayDir, ((i != thisObjIndex) ? -1 : thisObjPartIndex), point, normal, t, intMat, intPartIndex ))
	if (0 < t && t < param) {
	  param = t;
	  P = point;
	  N = normal;
	  objIndex = i;
	  objPartIndex = intPartIndex;
	  mat = intMat;
	}
    }
  }
    
  if (storingRays) {

    // Is this ray aiming for a light?

    bool foundLight = false;
    vec3 lightPos;
    
    for (int j=0; j<lights.size(); j++) {
      float lightToRayDist = ((lights[j]->position - rayStart) - ((lights[j]->position - rayStart) * rayDir) * rayDir).length();
      if (lightToRayDist < 0.001) {
	foundLight = true;
	lightPos = lights[j]->position;
	break;
      }
    }

    if (param != MAXFLOAT) {
      storedRays.add( P );
      if (foundLight)
	storedRayColours.add( vec3(.9,.3,.1) ); // RED: shadow ray toward a light
      else
	storedRayColours.add( vec3(.1,.7,.7) ); // CYAN: normal ray that hits something
    } else {
      if (foundLight) {
	storedRays.add( lightPos );
	storedRayColours.add( vec3(.9,.3,.1) ); // RED: shadow ray toward a light
      } else {
	storedRays.add( rayStart+2*rayDir );
	storedRayColours.add( vec3(.3,.3,.3) ); // GREY: normal ray that misses
      }
    }
  }

  return (param != MAXFLOAT);
}

// Raytrace: This is the main raytracing routine which finds the first
// object intersected, performs the lighting calculation, and does
// recursive calls.
//
// This returns the colour received on the ray.

vec3 Scene::raytrace( vec3 &rayStart, vec3 &rayDir, int depth, int thisObjIndex, int thisObjPartIndex )

{
  // Terminate the ray?

  depth++;

  if (depth > maxDepth)
    return blackColour;

  // Find the closest object intersected

  vec3 P, N;
  float  t;
  int    objIndex, objPartIndex;
  Material *mat;

  findFirstObjectInt( rayStart, rayDir, thisObjIndex, thisObjPartIndex, P, N, t, objIndex, objPartIndex, mat );

  // No intersection: Return background colour

  if (t == MAXFLOAT)
    if (depth == 1)
      return backgroundColour;
    else
      return blackColour;

  // Find reflection direction & incoming light from that direction

  Object &obj = *objects[objIndex];

  vec3 E = (-1 * rayDir).normalize();
  vec3 R = (2 * (E * N)) * N - E;

  float  alpha;
  vec3 colour = obj.textureColour( P, objPartIndex, alpha );
  vec3 kd = vec3( colour.x*mat->kd.x, colour.y*mat->kd.y, colour.z*mat->kd.z );

  vec3 Iout = mat->Ie + vec3( mat->ka.x * Ia.x, mat->ka.y * Ia.y, mat->ka.z * Ia.z );

  // Compute glossy reflection

  if (mat->g < 0 || mat->g > 1) {
    cerr << "Material glossiness is outside the range [0,1]" << endl;
    exit(1);
  }

  float g = mat->g;

  if (g == 1 || glossyIterations == 1) {

    vec3 Iin = raytrace( P, R, depth, objIndex, objPartIndex );

    Iout = Iout + calcIout( N, R, E, E, kd, mat->ks, mat->n, Iin );

  } else if (g > 0) {

    // Send a number of rays equal to 'glossyIterations' in the cone
    // defined by direction 'R' with glossiness 'g'.  Note that the
    // cone's halfangle psi = acos(g)

    // YOUR CODE HERE
    
    vec3 TotalGlossyIout = vec3(0.0,0.0,0.0);

    float psi = acos(g);
    vec3 u = R.perp1();
    vec3 v = R.perp2();
    float l = 1.0/tan(psi);
    //std::cout << "R: " << R.x << " " << R.y << " " << R.z << std::endl;
    //std::cout << "u: " << u.x << " " << u.y << " " << u.z << std::endl;
    //std::cout << "v: " << v.x << " " << v.y << " " << v.z << std::endl;
    
    for(int i = 0; i < glossyIterations; i++){
	// Generate random vectors within the cone defined by direction 'R' with glossiness 'g'
        float a;
        float b;
	do {
	  a = static_cast <float>( rand() ) / RAND_MAX;
  	  b = static_cast <float>( rand() ) / RAND_MAX;
	} while(a*a + b*b > 1.0);
	//std::cout << "a: " << a << " b: " << b << std::endl;
	vec3 Rprime = (l*R + a*u + b*v).normalize();

    	vec3 Iin = raytrace( P, Rprime, depth, objIndex, objPartIndex );
	
	// We pass kd = (0,0,0) because glossy reflection contributes only to the specular component of illumation
        TotalGlossyIout = TotalGlossyIout + calcIout( N, Rprime, E, E, vec3(0.0, 0.0, 0.0) , mat->ks, mat->n, Iin );
    }
    
    Iout = Iout + (1.0f / glossyIterations) * TotalGlossyIout; // We add the average Iout from glossy reflection

  }


  // Add direct contributions from lights

  for (int i=0; i<lights.size(); i++) {
    Light &light = *lights[i];

    vec3 L = light.position - P;

    if (N*L > 0) {
      float  Ldist = L.length();
      L = (1.0/Ldist) * L;

      vec3 intP, intN;
      float intT;
      int intObjIndex, intObjPartIndex;
      Material *intMat;

      // Is there an object between P and the light?

      bool found = findFirstObjectInt( P, L, objIndex, objPartIndex, intP, intN, intT, intObjIndex, intObjPartIndex, intMat );

      if (!found || intT > Ldist) { // no object: Add contribution from this light
	vec3 Lr = (2 * (L * N)) * N - L;
	Iout = Iout + calcIout( N, L, E, Lr, kd, mat->ks, mat->n, light.colour);
      }
    }
  }

  // Add contributions from emitting triangles

  for (int i=0; i<objects.size(); i++)
    if (i != thisObjIndex && objects[i]->mat->Ie.squaredLength() > 0) {

      Triangle *triangle = (Triangle*) objects[i];

      // Send NUM_SOFT_SHADOW_RAYS shadow rays toward random points on
      // the light.  Accumulate in Iout the contributions of all the
      // shadow rays

      // YOUR CODE HERE
        vec3 TotalShadowIout = vec3(0.0, 0.0, 0.0);

        // Generate random vectors towards light
        for(int i = 0; i < NUM_SOFT_SHADOW_RAYS; i++){

          // Generate vector
          float a;
          float b;
	  do {
	    a = static_cast <float>( rand() ) / RAND_MAX;
  	    b = static_cast <float>( rand() ) / RAND_MAX;
	  } while(a < 0 | b < 0 | (a + b > 1.0) );
          float c = 1 - a - b;
          // Compute this point on the triangle 
          vec3 PointOnTriangle = triangle->pointFromBarycentricCoords(a, b, c);
          vec3 Lprime = PointOnTriangle - P;
	  float  Ldist = Lprime.length();
	  Lprime = Lprime.normalize();

          // Check if blocked
      	  vec3 intP, intN;
      	  float intT;
      	  int intObjIndex, intObjPartIndex;
      	  Material *intMat;

          bool found = findFirstObjectInt( P, Lprime, objIndex, objPartIndex, intP, intN, intT, intObjIndex, intObjPartIndex, intMat );
	  if (found && abs(intT - Ldist) < 0.0001) { // no object between light and point
 	    // Add contribution from light intensity
	    // std::cout << "intMat->Ie: " << intMat->Ie << std::endl;
	    TotalShadowIout = TotalShadowIout + intMat->Ie; 
	  }

        }
        // Each shadow ray contributes 1/k of the intensity, where we sample k rays.   
	Iout = Iout + (1.0 / NUM_SOFT_SHADOW_RAYS) * TotalShadowIout;
    }

  return Iout;
}


// Calculate the outgoing intensity due to light Iin entering from
// direction L and exiting to direction E, with normal N.  Reflection
// direction R is provided, along with the material properties Kd, 
// Ks, and n.
//
//       Iout = Iin * ( Kd (N.L) + Ks (R.V)^n )

vec3 Scene::calcIout( vec3 N, vec3 L, vec3 E, vec3 R,
			vec3 Kd, vec3 Ks, float ns,
			vec3 In )

{
  // Don't illuminate from the back of the surface

  if (N * L <= 0)
    return blackColour;

  // Both E and L are in front:

  vec3 Id = (L*N) * In;

  vec3 Is;

  if (R*E < 0)
    Is = blackColour;		// Is = 0 from behind the
  else				// reflection direction
    Is = pow( R*E, ns ) * In;

  return vec3( Is.x*Ks.x+Id.x*Kd.x, Is.y*Ks.y+Id.y*Kd.y, Is.z*Ks.z+Id.z*Kd.z );
}


// Determine the colour of one pixel.  This is where
// the raytracing actually starts.

vec3 Scene::pixelColour( int x, int y )

{
  vec3 result;

  int count = 0;
  vec3 sum(0,0,0);

  // Send numPixelSamples x numPixelSamples rays through the pixel.
  // If 'jitter' is true, jitter them in their own regions.  If
  // 'jitter' is false, send them through the centres of their own
  // regions.

  // YOUR CODE HERE





  // Here's some code that chooses a direction through the pixel and
  // traces one ray in that direction, without antialiasing.  Replace
  // this code with your own.

  vec3 dir = (llCorner + x * right + y * up).normalize();
  result = raytrace( eye->position, dir, 0, -1, -1 );





  
  if (storingRays)
    storingRays = false;

  return result;
}


// Read the scene from an input stream

void Scene::read( const char *basename, istream &in )

{
  char command[1000];

  while (in) {

    skipComments( in );
    in >> command;
    if (!in || command[0] == '\0')
      break;
    
    skipComments( in );

    if (strcmp(command,"sphere") == 0) {

      Sphere *o = new Sphere();
      in >> *o;
      objects.add( o );
      
    } else if (strcmp(command,"triangle") == 0) {

      Triangle *o = new Triangle();
      in >> *o;
      objects.add( o );
      
    } else if (strcmp(command,"material") == 0) {

      Material *m = new Material();
      in >> *m;
      materials.add( m );
      
    } else if (strcmp(command,"wavefront") == 0) {

      // Rely on the wavefront.cpp code to read this

      string filename;
      in >> filename;

      char pathname[1000];
      sprintf( pathname, "%s/%s", basename, filename.c_str() );

      WavefrontObj *o = new WavefrontObj( pathname );
      objects.add( o );
      
    } else if (strcmp(command,"light") == 0) {

      Light *o = new Light();
      in >> *o;
      lights.add( o );
      
    } else if (strcmp(command,"eye") == 0) {

      eye = new Eye();
      in >> *eye;

      win->eye = eye->position;
      win->lookAt = eye->lookAt;
      win->upDir = eye->upDir;
      win->fovy = eye->fovy;
      
    } else {
      
      cerr << "Command '" << command << "' not recognized" << endl;
      exit(-1);
    }
  }
}



// Output the whole scene (mainly for debugging the reader)


void Scene::write( ostream &out )

{
  out << *eye << endl;

  for (int i=0; i<lights.size(); i++)
    out << *lights[i] << endl;

  for (int i=0; i<materials.size(); i++)
    out << *materials[i] << endl;

  for (int i=0; i<objects.size(); i++)
    out << *objects[i] << endl;
}


char *Scene::statusMessage()

{
  static char buffer[1000];

  sprintf( buffer, "depth %d, glossy %d", maxDepth, glossyIterations );

  return buffer;
}



// Draw the scene.  This sets things up and simply
// calls pixelColour() for each pixel.

void Scene::renderRT( bool restart )

{
  static float nextDot;
  static int nextx, nexty;

  mat4 WCS_to_VCS = lookat( win->eye, win->lookAt, win->upDir );
  mat4 VCS_to_CCS = perspective( win->fovy, win->windowWidth / win->windowHeight, win->zNear, win->zFar );

  if (restart) {

    // Copy the window eye into the scene eye

    eye->position = win->eye;
    eye->lookAt = win->lookAt;
    eye->upDir = win->upDir;
    eye->fovy = win->fovy;

    // Compute the image plane coordinate system

    up = (2 * tan( eye->fovy / 2.0 )) * eye->upDir.normalize();

    right = (2 * tan( eye->fovy / 2.0 ) * (float) win->windowWidth / (float) win->windowHeight)
      * ((eye->lookAt - eye->position) ^ eye->upDir).normalize();
  
    llCorner = (eye->lookAt - eye->position).normalize()
      - 0.5 * up - 0.5 * right;

    up = (1.0 / (float) (win->windowHeight-1)) * up;
    right = (1.0 / (float) (win->windowWidth-1)) * right;

    if (nextDot != 0) {
      cout << "\r           \r";
      cout.flush();
    }

    nextx = 0;
    nexty = 0;
    nextDot = 0;

    stop = false;

    // Clear the RT image
    
    if (rtImage != NULL)
      delete [] rtImage;

    rtImage = NULL;
  }

  // Set up a new RT image

  if (rtImage == NULL) {
    rtImage = new vec4[ (int) (win->windowWidth * win->windowHeight) ];
    for (int i=0; i<win->windowWidth * win->windowHeight; i++)
      rtImage[i] = vec4(0,0,0,0); // transparent
  }

  if (stop)
    return;

  // Draw the next pixel

  vec3 colour = pixelColour( nextx, nexty );

  rtImage[ nextx + nexty * (int) win->windowWidth ] = vec4( colour.x, colour.y, colour.z, 1 ); // opaque

  // Move (nextx,nexty) to the next pixel

  nexty++;
  if (nexty >= win->windowHeight) {
    nexty = 0;
    nextx++;
    if ((float)nextx/(float)win->windowWidth >= nextDot) {

      cout << "."; cout.flush();
      nextDot += 0.1;

      draw_RT_and_GL( gpuProg, WCS_to_VCS, VCS_to_CCS ); // gpuProg is from main.cpp
    }
    
    if (nextx >= win->windowWidth) { // finished

      draw_RT_and_GL( gpuProg, WCS_to_VCS, VCS_to_CCS );

      nextx = 0;
      stop = true;
      cout << "\r           \r";
      cout.flush();
    }
  }
}


// Render the scene with OpenGL


void Scene::renderGL( mat4 &WCS_to_VCS, mat4 &VCS_to_CCS )

{
  // Set up the framebuffer
 
  glEnable( GL_DEPTH_TEST );
  glClearColor( backgroundColour.x, backgroundColour.y, backgroundColour.z, 0 );
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

  // Copy the window eye into the scene eye

  eye->position = win->eye;
  eye->lookAt = win->lookAt;
  eye->upDir = win->upDir;
  eye->fovy = win->fovy;

  // Draw the objects

  gpuProg->activate();

  // Light direction

  vec3 lightDir(0,0,1);
  lightDir = lightDir.normalize();
  gpuProg->setVec3( "lightDir", lightDir );

  // Ambient lighting

  gpuProg->setVec3( "Ia", vec3(1,1,1) );

  // Axes

  if (showAxes) {
    if (axes == NULL)
      axes = new Axes();	// create axes here so that they are not created before the window is initialized
    mat4 MVP = VCS_to_CCS * WCS_to_VCS;
    axes->draw( MVP );
  }

  // Show the lights

  for (int i=0; i<lights.size(); i++)
    lights[i]->draw( gpuProg, WCS_to_VCS, VCS_to_CCS );
      
  // Draw any stored rays (for debugging)

  drawStoredRays( gpuProg, WCS_to_VCS, VCS_to_CCS );

  // Draw everything

  for (int i=0; i<objects.size(); i++)
    objects[i]->renderGL( gpuProg, WCS_to_VCS, VCS_to_CCS );

  // Show status message
      
#ifdef USE_FREETYPE
  render_text( statusMessage(), 10, 10, win->window );
#endif

  // Done

  gpuProg->deactivate();
}



// Draw any stored rays (for debugging)

void Scene::drawStoredRays( GPUProgram *gpuProg, mat4 &WCS_to_VCS, mat4 &VCS_to_CCS )

{
  if (storedRays.size() > 0) {
	
    if (arrow == NULL)
      arrow = new Arrow();

    for (int i=0; i<storedRays.size(); i+=2) {

      vec3 dir = storedRays[i] - storedRays[i+1];
      mat4 OCS_to_WCS = translate( storedRays[i+1] ) * rotate( vec3(0,0,1), dir ) * scale( 1, 1, dir.length() );

      arrow->mat->kd = storedRayColours[i/2];

      arrow->draw( gpuProg, OCS_to_WCS, WCS_to_VCS, VCS_to_CCS );
    }
  }
}



// Draw the scene, then draw the RT image on top of it.  Transparent
// pixels in the RT image are those that have not been calculated yet.

void Scene::draw_RT_and_GL( GPUProgram *gpuProg, mat4 &WCS_to_VCS, mat4 &VCS_to_CCS )

{
  win->display(); // GL rendering
  drawRTImage();  // RT image on top

  // Redraw the stored rays over top of the RT image

  gpuProg->activate();
  mat4 VP = translate(0,0,-0.1) * VCS_to_CCS; // move a little forward in the CCS so the it overdraws the previously-drawn arrow at this location
  drawStoredRays( gpuProg, WCS_to_VCS, VP );
  gpuProg->deactivate();

  // Perhaps show the pixel zoom

  if (buttonDown == GLFW_MOUSE_BUTTON_MIDDLE)
    showPixelZoom( mouse );

  // Status message

#ifdef USE_FREETYPE
  render_text( statusMessage(), 10, 10, win->window );
#endif

  // Done

  glfwSwapBuffers( win->window );
}



// Draw a quad at the mouse position.  Texture the quad with zoomed-pixels near the mouse position

#define ZOOM_FACTOR 13		// scale applied to one pixel
#define ZOOM_RADIUS 15		// radius of window, in original pixels

void Scene::showPixelZoom( vec2 mouse )

{
  if (rtImage == NULL)
    return;

  // Draw texture on a small quad

  vec2 texPos( mouse.x / win->windowWidth, (win->windowHeight - mouse.y) / win->windowHeight ); // mouse position in texture
  vec2 winPos = 2 * texPos - vec2(1,1); // mouse position in window
    
  vec2 texRadius = ZOOM_RADIUS * vec2( 1 / win->windowWidth, 1 / win->windowHeight ); // radius in texture coordinates
  vec2 winRadius = ZOOM_FACTOR * 2 * texRadius; // radius in window coordinates

  vec2 verts[8] = {
    winPos + vec2( -winRadius.x, -winRadius.y ), // positions
    winPos + vec2( -winRadius.x,  winRadius.y ),
    winPos + vec2(  winRadius.x, -winRadius.y ),
    winPos + vec2(  winRadius.x,  winRadius.y ),

    texPos + vec2( -texRadius.x, -texRadius.y ), // texture coordinates
    texPos + vec2( -texRadius.x,  texRadius.y ),
    texPos + vec2(  texRadius.x, -texRadius.y ),
    texPos + vec2(  texRadius.x,  texRadius.y )
  };
    
  GLuint VAO, VBO;

  glGenVertexArrays( 1, &VAO );
  glBindVertexArray( VAO );

  glGenBuffers( 1, &VBO );
  glBindBuffer( GL_ARRAY_BUFFER, VBO );

  glBufferData( GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW );

  glEnableVertexAttribArray( 0 );
  glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, 0, 0 );

  glEnableVertexAttribArray( 1 );
  glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, 0, (void*) (sizeof(vec2)*4) );

  // Set up GPU

  glActiveTexture( GL_TEXTURE1 );
  glBindTexture( GL_TEXTURE_2D, rtImageTexID );

  glDisable( GL_DEPTH_TEST );
  gpu->activate();
  gpu->setInt( "texUnitID", 1 );

  // Draw texture

  gpu->setInt( "texturing", 1 );
  glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );

  // Draw boundary

  vec2 lineverts[4] = {
    winPos + vec2( -winRadius.x, -winRadius.y ), // positions
    winPos + vec2(  winRadius.x, -winRadius.y ),
    winPos + vec2(  winRadius.x,  winRadius.y ),
    winPos + vec2( -winRadius.x,  winRadius.y )
  };
    
  glBufferData( GL_ARRAY_BUFFER, sizeof(lineverts), lineverts, GL_STATIC_DRAW );

  glEnableVertexAttribArray( 0 );
  glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, 0, 0 );

  glDisableVertexAttribArray( 1 );

  gpu->setInt( "texturing", 0 );

  glLineWidth( 5 );
  glDrawArrays( GL_LINE_LOOP, 0, 4 );

  // Done

  gpu->deactivate();
  glEnable( GL_DEPTH_TEST );

  glDeleteBuffers( 1, &VBO );
  glDeleteVertexArrays( 1, &VAO );

  glBindTexture( GL_TEXTURE_2D, 0 );
}



void Scene::drawRTImage()

{
  if (rtImage == NULL)
    return;

  if (gpu == NULL) {
    gpu = new GPUProgram();
    gpu->init( vertShader, fragShader );
  }

  // Send texture to GPU

  if (rtImageTexID == 0)
    glGenTextures( 1, &rtImageTexID );

  glActiveTexture( GL_TEXTURE1 );
  glBindTexture( GL_TEXTURE_2D, rtImageTexID );

  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );

  glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, win->windowWidth, win->windowHeight, 0, GL_RGBA, GL_FLOAT, rtImage );

  // Draw texture on a full-screen quad

  vec2 verts[8] = {
    vec2( -1, -1 ), vec2( -1, 1 ), vec2( 1, -1 ), vec2( 1, 1 ), // positions
    vec2(  0,  0 ), vec2(  0, 1 ), vec2( 1,  0 ), vec2( 1, 1 )	// texture coordinates
  };
    
  GLuint VAO, VBO;

  glGenVertexArrays( 1, &VAO );
  glBindVertexArray( VAO );

  glGenBuffers( 1, &VBO );
  glBindBuffer( GL_ARRAY_BUFFER, VBO );

  glBufferData( GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW );

  glEnableVertexAttribArray( 0 );
  glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, 0, 0 );

  glEnableVertexAttribArray( 1 );
  glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, 0, (void*) (sizeof(vec2)*4) );

  glDisable( GL_DEPTH_TEST );
  glEnable( GL_BLEND );
  glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

  gpu->activate();
  gpu->setInt( "texUnitID", 1 );
  gpu->setInt( "texturing", 1 );
  glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
  gpu->deactivate();

  glDisable( GL_BLEND );
  glEnable( GL_DEPTH_TEST );

  glDeleteBuffers( 1, &VBO );
  glDeleteVertexArrays( 1, &VAO );

  glBindTexture( GL_TEXTURE_2D, 0 );
}



char *Scene::vertShader = "\n\
\n\
  #version 300 es\n\
\n\
  layout (location = 0) in mediump vec2 posIn;\n\
  layout (location = 1) in mediump vec2 texCoordsIn;\n\
\n\
  smooth out mediump vec2 pos;\n\
  smooth out mediump vec2 texCoords;\n\
\n\
  void main() {\n\
    gl_Position = vec4( posIn, -1, 1 );\n\
    pos = posIn;\n\
    texCoords = texCoordsIn;\n\
  }\n\
";


char *Scene::fragShader = "\n\
\n\
  #version 300 es\n\
\n\
  uniform sampler2D texUnitID;\n\
  uniform bool texturing;\n\
\n\
  smooth in mediump vec2 pos;\n\
  smooth in mediump vec2 texCoords;\n\
  out mediump vec4 outputColour;\n\
\n\
  void main() {\n\
\n\
    if (texturing) {\n\
      mediump vec4 c = texture( texUnitID, texCoords );\n \
      outputColour = c;\n\
    } else\n\
      outputColour = vec4(0.5,0.5,0.5,1);\n\
  }\n\
";



