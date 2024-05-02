
//--------------------------------------------------------------
//
//  10.15.19
//
//  CS 134 Midterm - Fall 2019 - Startup Files
// 
//   author:  Kevin M. Smith   

#include "ofApp.h"

//--------------------------------------------------------------
// setup scene, lighting, state and load geometry
//
void ofApp::setup() {


	bWireframe = false;
	bDisplayPoints = false;
	bAltKeyDown = false;
	bCtrlKeyDown = false;
	bLanderLoaded = false;
	cam.setDistance(10);
	cam.setNearClip(.1);
	cam.setFov(65.5);   // approx equivalent to 28mm in 35mm format
	cam.disableMouseInput();

	topCam.setNearClip(.1);
	topCam.setFov(65.5);
	topCam.setPosition(0, 15, 0);
	topCam.lookAt(glm::vec3(0, 0, 0));

	// set current camera;
	//
	theCam = &cam;

	gui.setup();
	gui.add(velocity.setup("Velocity", ofVec3f(0, -20, 0), ofVec3f(0, -1, 0), ofVec3f(100, 100, 100)));
	gui.add(lifespan.setup("Lifespan", 2.0, .1, 10.0));
	gui.add(rate.setup("Rate", 1.0, .5, 60.0));
	gui.add(radius.setup("Disk Radius", 1.0, .1, 10.0));

	ofSetVerticalSync(true);
	ofEnableSmoothing();
	ofEnableDepthTest();

	// load BG image
	//
	bBackgroundLoaded = backgroundImage.load("images/starfield-plain.jpg");


	// setup rudimentary lighting 
	//
	initLightingAndMaterials();


	// load lander model
	//
	if (lander.loadModel("geo/lander.obj")) {
		lander.setScaleNormalization(false);
		lander.setScale(.5, .5, .5);
		lander.setRotation(0, -180, 1, 0, 0);
		lander.setPosition(0, 0, 0);

		bLanderLoaded = true;
	}
	else {
		cout << "Error: Can't load model" << "geo/lander.obj" << endl;
		ofExit(0);
	}

	// setup LEM
	//

	thrusterEmitter.setEmitterType(DiskEmitter);
	

}



void ofApp::startThruster(ParticleEmitter& emitter) {
	emitter.start();
	emitter.oneShot = true;
}

void ofApp::update() {

	headingVec = ofVec3f(0, 0, 0);

	groundedEmitter.setLifespan(lifespan);
	groundedEmitter.setVelocity(ofVec3f(velocity));
	groundedEmitter.setRate(rate);
	groundedEmitter.setRadius(radius);

	

	groundedEmitter.update();


	if (moveUp) {
		headingVec.z = -headingSpeed;
		startThruster(thrusterEmitter);
	}
	else if (moveDown) {
		headingVec.z = headingSpeed;
		startThruster(thrusterEmitter);
	}
	else if (moveLeft) {
		headingVec.x = -headingSpeed;
		startThruster(thrusterEmitter);
	}
	else if (moveRight) {
		headingVec.x = headingSpeed;
		startThruster(thrusterEmitter);
	}
	else if (thrustUp) {
		headingVec.y = headingSpeed;
		startThruster(thrusterEmitter);
	}
	else if (thrustDown) {
		headingVec.y = -headingSpeed;
		startThruster(thrusterEmitter);
	}


	if (counterClockwiseRot) {
		angularAcceleration = 1;

	}
	else if (clockwiseRot) {
		angularAcceleration = -1;
	}
	else {
		angularAcceleration = 0;
	}

	// Slows down the accelration of the rotation
	if (angularVelocity > 0) {
		angularVelocity -= 0.1;
	}
	else if (angularVelocity < 0) {
		angularVelocity += 0.1;
	}


	angularVelocity += angularAcceleration;
	angle += angularVelocity;

	headingVec = headingVec.getRotated(angle, ofVec3f(0, 1, 0));
	lander.setRotation(1, angle, 0, 1, 0);
	spaceCraft.acceleration = headingVec;

	spaceCraft.integrate();

	// Prevents the lander from going below the grid
	if (spaceCraft.position.y < 0) {
		spaceCraft.position.y = 0;
	}

	lander.setPosition(spaceCraft.position.x, spaceCraft.position.y, spaceCraft.position.z);

	thrusterEmitter.setPosition(spaceCraft.position);

	thrusterEmitter.update();
}

//--------------------------------------------------------------
void ofApp::draw() {


	// draw background image
	//
	if (bBackgroundLoaded) {
		ofPushMatrix();
		ofDisableDepthTest();
		ofSetColor(50, 50, 50);
		ofScale(2, 2);
		backgroundImage.draw(-200, -100);
		ofEnableDepthTest();
		ofPopMatrix();
	}

	
	

	theCam->begin();

	ofPushMatrix();
	// draw a reference grid
	//
	ofPushMatrix();
	ofRotate(90, 0, 0, 1);
	ofSetLineWidth(1);
	ofSetColor(ofColor::dimGray);
	ofDisableLighting();
	ofDrawGridPlane();
	ofPopMatrix();

	if (bWireframe) {                    // wireframe mode  (include axis)
		ofDisableLighting();
		ofSetColor(ofColor::slateGray);
		if (bLanderLoaded) {

			//  Note this is how to rotate LEM model about the "Y" or "UP" axis
			//
			//lander.setRotation(0, angle, 0, 1, 0);    
			lander.drawWireframe();
		}
	}
	else {
		ofEnableLighting();              // shaded mode
		if (bLanderLoaded) {
			//lander.setRotation(0, angle, 0, 1, 0);
			lander.drawFaces();

		}
	}


	if (bDisplayPoints) {                // display points as an option    
		glPointSize(3);
		ofSetColor(ofColor::green);
	}


	ofPopMatrix();

	groundedEmitter.draw();
	thrusterEmitter.draw();
	theCam->end();
	
	ofDisableDepthTest();
	ofDisableLighting();
	gui.draw();
	ofEnableDepthTest();
	

	// draw screen data
	//
	string str;
	str += "Frame Rate: " + std::to_string(ofGetFrameRate());
	ofSetColor(ofColor::white);
	ofDrawBitmapString(str, ofGetWindowWidth() - 170, 15);

	string str2;
	str2 += "Altitide (AGL): " + std::to_string(lander.getPosition().y);
	ofSetColor(ofColor::white);
	ofDrawBitmapString(str2, 5, 15);

}


// Draw an XYZ axis in RGB at world (0,0,0) for reference.
//
void ofApp::drawAxis(ofVec3f location) {

	ofPushMatrix();
	ofTranslate(location);

	ofSetLineWidth(1.0);

	// X Axis
	ofSetColor(ofColor(255, 0, 0));
	ofDrawLine(ofPoint(0, 0, 0), ofPoint(1, 0, 0));


	// Y Axis
	ofSetColor(ofColor(0, 255, 0));
	ofDrawLine(ofPoint(0, 0, 0), ofPoint(0, 1, 0));

	// Z Axis
	ofSetColor(ofColor(0, 0, 255));
	ofDrawLine(ofPoint(0, 0, 0), ofPoint(0, 0, 1));

	ofPopMatrix();
}


void ofApp::keyPressed(int key) {

	switch (key) {
	case 'C':
	case 'c':
		if (cam.getMouseInputEnabled()) cam.disableMouseInput();
		else cam.enableMouseInput();
		break;
	case 'F':
	case 'f':
		ofToggleFullscreen();
		break;
	case 'H':
	case 'h':
		break;
	case 'P':
	case 'p':
		break;
	case 'r':
		cam.reset();
		break;
	case 'g':
		savePicture();
		break;
	case 't':
		break;
	case 'u':
		break;
	case 'v':
		togglePointsDisplay();
		break;
	case 'V':
		break;
	case 'm':
		toggleWireframeMode();
		break;
	case 'd':     // rotate spacecraft clockwise (about Y (UP) axis)
		clockwiseRot = true;
		break;
	case 'a':     // rotate spacecraft counter-clockwise (about Y (UP) axis)
		counterClockwiseRot = true;
		break;
	case 'w':     // spacecraft thrust UP
		thrustUp = true;
		break;
	case 's':     // spacefraft thrust DOWN
		thrustDown = true;
		break;
	case OF_KEY_F1:
		theCam = &cam;
		break;
	case OF_KEY_F3:
		theCam = &topCam;
		break;
	case OF_KEY_ALT:
		cam.enableMouseInput();
		bAltKeyDown = true;
		break;
	case OF_KEY_CONTROL:
		bCtrlKeyDown = true;
		break;
	case OF_KEY_SHIFT:
		break;
	case OF_KEY_DEL:
		break;
	case OF_KEY_UP:    // move forward
		moveUp = true;
		break;
	case OF_KEY_DOWN:   // move backward
		moveDown = true;
		break;
	case OF_KEY_LEFT:   // move left
		moveLeft = true;
		break;
	case OF_KEY_RIGHT:   // move right
		moveRight = true;
		break;
	case ' ':
		groundedEmitter.start();
		break;
	default:
		break;
	}
}


void ofApp::toggleWireframeMode() {
	bWireframe = !bWireframe;
}


void ofApp::togglePointsDisplay() {
	bDisplayPoints = !bDisplayPoints;
}

void ofApp::keyReleased(int key) {

	switch (key) {

	case OF_KEY_ALT:
		cam.disableMouseInput();
		bAltKeyDown = false;
		break;
	case OF_KEY_CONTROL:
		bCtrlKeyDown = false;
		break;
	case OF_KEY_SHIFT:
		break;
	case OF_KEY_UP:    // move forward
		moveUp = false;
		break;
	case OF_KEY_DOWN:   // move backward
		moveDown = false;
		break;
	case OF_KEY_LEFT:   // move left
		moveLeft = false;
		break;
	case OF_KEY_RIGHT:   // move right
		moveRight = false;
		break;
	case 'd':     // rotate spacecraft clockwise (about Y (UP) axis)
		clockwiseRot = false;
		break;
	case 'a':     // rotate spacecraft counter-clockwise (about Y (UP) axis)
		counterClockwiseRot = false;
		break;
	case 'w':     // spacecraft thrust UP
		thrustUp = false;
		break;
	case 's':     // spacefraft thrust DOWN
		thrustDown = false;
		break;
	case 'x':
		groundedEmitter.stop();
		break;
	default:
		break;

	}

}


//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {
}


//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {


}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {


}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {

}





//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {

}



//--------------------------------------------------------------
// setup basic ambient lighting in GL  (for now, enable just 1 light)
//
void ofApp::initLightingAndMaterials() {

	static float ambient[] =
	{ .5f, .5f, .5, 1.0f };
	static float diffuse[] =
	{ .7f, .7f, .7f, 1.0f };

	static float position[] =
	{ 20.0, 20.0, 20.0, 0.0 };

	static float lmodel_ambient[] =
	{ 1.0f, 1.0f, 1.0f, 1.0f };

	static float lmodel_twoside[] =
	{ GL_TRUE };


	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	//	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	//	glLightfv(GL_LIGHT0, GL_POSITION, position);
	glLightfv(GL_LIGHT1, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT1, GL_POSITION, position);


	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
	//	glLightModelfv(GL_LIGHT_MODEL_TWO_SIDE, lmodel_twoside);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glShadeModel(GL_SMOOTH);
}

void ofApp::savePicture() {
	ofImage picture;
	picture.grabScreen(0, 0, ofGetWidth(), ofGetHeight());
	picture.save("screenshot.png");
	cout << "picture saved" << endl;
}

//--------------------------------------------------------------
//
// support drag-and-drop of model (.obj) file loading.  when
// model is dropped in viewport, place origin under cursor
//
void ofApp::dragEvent(ofDragInfo dragInfo) {

}
