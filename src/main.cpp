#include "ofMain.h"
#include "ofApp.h"

// #version 120
// attribute vec4 position;
// attribute vec4 color;
// varying vec4 vColor;
// varying vec4 vColor;
//========================================================================
int main( ){
	ofSetupOpenGL(1280, 1024,OF_WINDOW);			// <-------- setup the GL context

	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	// vColor = color;
    // gl_Position = position;
	// gl_FragColor = vColor;
	ofRunApp(new ofApp());

}
