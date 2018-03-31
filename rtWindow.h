/* rtWindow.h
 */


#ifndef RTWINDOW_H
#define RTWINDOW_H

class RTwindow;


#include "arcballWindow.h"
#include "scene.h"


class RTwindow : public arcballWindow {

  Scene *scene;
  bool   viewpointChanged;

 public:

  RTwindow( int x, int y, int width, int height, const char *title, Scene *s )
    : arcballWindow( x, y, width, height, title ) {
    scene = s;
    viewpointChanged = true;
  }

  void userDraw( mat4 &WCS_to_VCS, mat4 &VCS_to_CCS ) {
    viewpointChanged = true;
    scene->renderGL( WCS_to_VCS, VCS_to_CCS );
  }

  void raytrace() {
    scene->renderRT( viewpointChanged );
    viewpointChanged = false;
  }

  bool userMouseClick( vec2 mouse, int button ) {

    if (button == GLFW_MOUSE_BUTTON_LEFT ) {
      scene->storedRays.clear();
      scene->storedRayColours.clear();
      scene->storingRays = true;
      scene->pixelColour( mouse.x, windowHeight-1-mouse.y );
      scene->storingRays = false;

      return true; // no further processing of this click
    }

    return false;
  }

  bool userMouseMotion( vec2 mouse, int button ) {

    scene->mouse = mouse;
    return false;
  }

  bool userMouseAction( vec2 mouse, int button, int action, int mods ) {

    if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
      if (action == GLFW_PRESS) {
	scene->buttonDown = button;
	scene->mouse = mouse;
      } else
	scene->buttonDown = -1;
      return true;
    }

    return false;
  }

  bool userKeyAction( int key, int scancode, int action, int mods ) {

    switch (key) {
    case GLFW_KEY_DELETE:
      scene->storedRays.clear();
      scene->storedRayColours.clear();
      break;
    case GLFW_KEY_ESCAPE:
      exit(0);
    case 'E':
      scene->outputEye();
      break;
    case '+':
    case '=':
      scene->maxDepth++;
      viewpointChanged = true;
      break;
    case '-':
    case '_':
      if (scene->maxDepth > 1) {
	scene->maxDepth--;
	viewpointChanged = true;
      }
      break;
    case '<':
    case ',':
      if (scene->glossyIterations > 1) {
	scene->glossyIterations--;
	viewpointChanged = true;
      }
      break;
    case '>':
    case '.':
      scene->glossyIterations++;
      viewpointChanged = true;
      break;
    case 'P':
      if (mods & GLFW_MOD_SHIFT)
	scene->numPixelSamples++;
      else {
	scene->numPixelSamples--;
	if (scene->numPixelSamples < 1)
	  scene->numPixelSamples = 1;
      }
      redisplay = true;
      cout << "pixel sampling " <<  scene->numPixelSamples << " x " <<  scene->numPixelSamples << endl;
      break;
    case 'A':
      scene->showAxes = !scene->showAxes;
      break;
    case 'J':
      scene->jitter = !scene->jitter;
      redisplay = true;
      cout << "jittering = " << scene->jitter << endl;
      break;
    case '/':
      cout
	<< endl
	<< "]     increase shown kd tree depth" << endl
	<< "[     decrease shown kd tree depth" << endl
	<< ">     increase glossy rays" << endl
	<< "<     decrease glossy rays" << endl
	<< "+     increase ray depth" << endl
	<< "-     decrease ray depth" << endl
	<< "P     increase pixel sampling" << endl
	<< "p     decrease pixel sampling" << endl
	<< "j     toggle pixel sample jittering" << endl
	<< "a     show/hide axes" << endl
	<< "e     output eye position" << endl
	<< "SPACE toggle rotation/translation mode" << endl
	<< "DEL   delete debugging rays" << endl
	<< "ESC   exit" << endl
	<< endl
	<< "left mouse drag          - rotate viewpoint" << endl
	<< "right mouse drag up/down - zoom" << endl
	<< "middle mouse             - pixel zoom" << endl;
      break;
    default:
      return false;  // not handled		
    }

    return true; // no further processing
  }
};


#endif
