
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
void ofApp::setup()
{
	bWireframe = false;
	bDisplayPoints = false;
	bAltKeyDown = false;
	bCtrlKeyDown = false;
	bLanderLoaded = false;
	bTerrainSelected = true;
	landPlace1.set(-55, 11.3, 28);
	landPlace2.set(48, 24.5, 39.2);
	landPlace3.set(7.4, -7.3, -44);
	cam.setDistance(30);
	cam.setNearClip(.1);
	cam.setFov(65.5); // approx equivalent to 28mm in 35mm format
	// ofSetVerticalSync(true);
	cam.disableMouseInput();
	// ofEnableSmoothing();
	// ofEnableDepthTest();

	topCam.setNearClip(.1);
	topCam.setFov(65.5);
	topCam.setPosition(0, 15, 0);
	topCam.lookAt(glm::vec3(0, 0, 0));

	// attempt at onboarding cam lol
	onBoardCam.setNearClip(.1);
	onBoardCam.setFov(65.5);
	onBoardCam.disableMouseInput();
	onBoardCam.setPosition(lander.getPosition().x, lander.getPosition().y, lander.getPosition().z);
	// onBoardCam.lookAt(lander.getPosition());
	onBoardCam.lookAt(glm::vec3(lander.getPosition().x, -100, lander.getPosition().z));

	// tracking cam good??
	trackingCam.setNearClip(.1);
	trackingCam.setFov(65.5);
	trackingCam.setDistance(70);
	trackingCam.disableMouseInput();
	trackingCam.setPosition(70, 40, 0);
	trackingCam.lookAt(lander.getPosition());

	// set current camera;
	//
	theCam = &cam;

	mars.loadModel("geo/desert5.obj");
	mars.setScaleNormalization(false);

	gui.setup();
	gui.add(numLevels.setup("Number of Octree Levels", 1, 1, 10));
	gui.add(groundHeight.setup("Ground Height", false));

	ofSetVerticalSync(true);
	ofEnableSmoothing();
	ofEnableDepthTest();

	// load BG image
	//
	bBackgroundLoaded = backgroundImage.load("images/background.jpg");
	if (winImg.load("images/HappyFace.png"))
	{
		winImgLoaded = true;
	}
	else
	{
		cout << "Can't open player image file" << endl;
	}

	octree.create(mars.getMesh(1), 20);
	cout << "Number of Verts: " << mars.getMesh(1).getNumVertices() << endl;

	// load lander model
	//
	if (lander.loadModel("geo/seat.obj"))
	{
		lander.setScaleNormalization(false);
		lander.setScale(1, 1, 1);
		lander.setRotation(0, 90, 0, 1, 0);
		lander.setPosition(0, 0.5, 0);

		bLanderLoaded = true;
		cout << "geo/lander.obj" << endl;
	}
	else
	{
		cout << "Error: Can't load model"
			 << "geo/lander.obj" << endl;
		ofExit(0);
	}

	// setup LEM
	//

	thrusterEmitter.setEmitterType(DiskEmitter);
	// collisionEmitter.setEmitterType(RadialEmitter);
	// collisionEmitter.color = ofColor::red;
	turbForce = new TurbulenceForce(ofVec3f(-1, -1, -1), ofVec3f(1, 1, 1));
	gravityForce = new GravityForce(ofVec3f(0, -10, 0));
	shipGravity.set(0, -0.05, 0);
	radialForce = new ImpulseRadialForce(1.0);

	thrusterEmitter.sys->addForce(gravityForce);
	collisionEmitter.sys->addForce(turbForce);
	collisionEmitter.sys->addForce(gravityForce);
	collisionEmitter.sys->addForce(radialForce);
	collisionEmitter.setOneShot(true);
	collisionEmitter.setEmitterType(RadialEmitter);
	collisionEmitter.setGroupSize(500);
	collisionEmitter.radius = 100;
	collisionEmitter.particleRadius = 0.05;
	collisionEmitter.color = ofColor::red;
	// spaceCraft.forces=shipGravity;

	// texture loading
	//
	ofDisableArbTex(); // disable rectangular textures

	// load textures
	//
	if (!ofLoadImage(particleTex1, "images/red.jpg"))
	{
		cout << "Particle Texture File1: images/thrusterFire.jpg not found" << endl;
		ofExit();
	}
	if (!ofLoadImage(particleTex2, "images/1.jpg"))
	{
		// if (!ofLoadImage(particleTex2, "images/red_background.jpg")) {
		cout << "Particle Texture File2: images/thrusterFire.jpg not found" << endl;
		ofExit();
	}

	// load the shader
	//
#ifdef TARGET_OPENGLES
	shader.load("shaders_gles/shader");
#else
	shader.load("shaders/shader");
#endif

	// setup lights

	keyLight.setup();
	keyLight.enable();
	keyLight.setAreaLight(1, 1);
	keyLight.setAmbientColor(ofFloatColor(0.1, 0.1, 0.1));
	keyLight.setDiffuseColor(ofFloatColor(1, 1, 1));
	keyLight.setSpecularColor(ofFloatColor(1, 1, 1));

	keyLight.rotate(45, ofVec3f(0, 1, 0));
	keyLight.rotate(-45, ofVec3f(1, 0, 0));
	// keyLight.setPosition(5, 5, 5);
	keyLight.setPosition(20, 70, 30);

	fillLight.setup();
	fillLight.enable();
	fillLight.setSpotlight();
	fillLight.setScale(.05);
	fillLight.setSpotlightCutOff(15);
	fillLight.setAttenuation(2, .001, .001);
	fillLight.setAmbientColor(ofFloatColor(0.1, 0.1, 0.1));
	fillLight.setDiffuseColor(ofFloatColor(1, 1, 1));
	fillLight.setSpecularColor(ofFloatColor(1, 1, 1));
	fillLight.rotate(-10, ofVec3f(1, 0, 0));
	fillLight.rotate(-45, ofVec3f(0, 1, 0));
	// fillLight.setPosition(-5, 5, 5);
	fillLight.setPosition(-50, 20, 40);

	rimLight.setup();
	rimLight.enable();
	rimLight.setSpotlight();
	rimLight.setScale(.05);
	rimLight.setSpotlightCutOff(30);
	rimLight.setAttenuation(.2, .001, .001);
	rimLight.setAmbientColor(ofFloatColor(0.1, 0.1, 0.1));
	rimLight.setDiffuseColor(ofFloatColor(1, 1, 1));
	rimLight.setSpecularColor(ofFloatColor(1, 1, 1));
	rimLight.rotate(135, ofVec3f(0, 1, 0));
	rimLight.rotate(-20, ofVec3f(1, 0, 0));
	// fillLight.rotate(-45, ofVec3f(0, 1, 0));
	// rimLight.setPosition(0, 5, -7);
	rimLight.setPosition(20, 40, -40);

	explosionSound.load("sounds/explosion.mp3");
	thrusterSound.load("sounds/thruster2.mp3");
	gameOverSound.load("sounds/gameOver.wav");
	gameWinSound.load("sounds/gameWin.wav");
	backgroudSound.load("sounds/background.mp3");
	backgroudSound.setLoop(true);

	fuelTime = maxFuelTime;
	restart();
}

void ofApp::loadThrusterVbo()
{
	if (thrusterEmitter.sys->particles.size() < 1)
		return;

	vector<ofVec3f> sizes;
	vector<ofVec3f> points;

	for (int i = 0; i < thrusterEmitter.sys->particles.size(); i++)
	{
		points.push_back(thrusterEmitter.sys->particles[i].position);
		sizes.push_back(ofVec3f(10));
	}

	// upload the data to the vbo
	//
	int total = (int)points.size();
	thrusterVBO.clear();
	thrusterVBO.setVertexData(&points[0], total, GL_STATIC_DRAW);
	thrusterVBO.setNormalData(&sizes[0], total, GL_STATIC_DRAW);
	// explosionVBO.setColorData(&particleColors1[0], total, GL_STATIC_DRAW);
}

void ofApp::loadExplosionVbo()
{
	if (collisionEmitter.sys->particles.size() < 1)
		return;

	vector<ofVec3f> sizes;
	vector<ofVec3f> points;

	for (int i = 0; i < collisionEmitter.sys->particles.size(); i++)
	{
		points.push_back(collisionEmitter.sys->particles[i].position);
		sizes.push_back(ofVec3f(10));
	}

	// upload the data to the vbo
	int total = (int)points.size();
	explosionVBO.clear();
	explosionVBO.setVertexData(&points[0], total, GL_STATIC_DRAW);
	explosionVBO.setNormalData(&sizes[0], total, GL_STATIC_DRAW);
}

void ofApp::startThruster(ParticleEmitter &emitter)
{
	float fuelConsumptionRate = 1; // each sec consum 1
	emitter.start();
	emitter.oneShot = true;
	fuelTime -= fuelConsumptionRate * ofGetLastFrameTime();
	if (!thrusterSound.isPlaying())
	{
		thrusterSound.play();
	}
}

void ofApp::update()
{

	headingVec = ofVec3f(0, 0, 0);
	spaceCraft.forces = shipGravity;
	spaceCraft.setPosition(lander.getPosition());
	// cout<< "pos: x:"<<lander.getPosition().x<<" y:"<<lander.getPosition().y<<" z:"<<lander.getPosition().z<<endl;
	checkCollision();
	if (!bInDrag && !catapultShip && !isCollided && restartBool)
	{
		// spaceCraft.setPosition(lander.getPosition());
		if (moveUp)
		{
			headingVec.z = -headingSpeed;
			startThruster(thrusterEmitter);
		}
		else if (moveDown)
		{
			headingVec.z = headingSpeed;
			startThruster(thrusterEmitter);
		}
		else if (moveLeft)
		{
			headingVec.x = -headingSpeed;
			startThruster(thrusterEmitter);
		}
		else if (moveRight)
		{
			headingVec.x = headingSpeed;
			startThruster(thrusterEmitter);
		}
		else if (thrustUp)
		{
			headingVec.y = headingSpeed;
			startThruster(thrusterEmitter);
		}
		else if (thrustDown)
		{
			headingVec.y = -headingSpeed;
			startThruster(thrusterEmitter);
		}

		if (counterClockwiseRot)
		{
			angularAcceleration = 1;
		}
		else if (clockwiseRot)
		{
			angularAcceleration = -1;
		}
		else
		{
			angularAcceleration = 0;
		}

		angularVelocity += angularAcceleration;
		angle += angularVelocity;

		headingVec = headingVec.getRotated(angle, ofVec3f(0, 1, 0));
		lander.setRotation(1, angle, 0, 1, 0);
		angularVelocity *= 0.94;
		spaceCraft.acceleration = headingVec;
		spaceCraft.integrate();
	}
	if (!catapultShip)
	{
		collisionEmitter.setPosition(spaceCraft.position);
	}

	lander.setPosition(spaceCraft.position.x, spaceCraft.position.y, spaceCraft.position.z);

	thrusterEmitter.setPosition(spaceCraft.position);

	thrusterEmitter.update();

	currentPosition = lander.getPosition();

	if (currentPosition != lastPosition)
	{
		lastMovementDirection = currentPosition - lastPosition;
		lastMovementDirection = glm::normalize(lastMovementDirection);
	}
	// cout <<"p:"<<spaceCraft.position.y<<endl;
	lastPosition = currentPosition;
	if (spaceCraft.position.y < getGroundHeight)
	{
		spaceCraft.setPosition(glm::vec3(spaceCraft.position.x, getGroundHeight, spaceCraft.position.z));
	}
	if (isCollided)
	{
		float landingForce = getLandingForce(spaceCraft.velocity);
		// cout << "landing force: " << landingForce << endl;s
		if (landingForce > baseLandingForce)
		{
			catapultShip = true;
		}
	}
	// cout <<"v:"<<spaceCraft.velocity.y<<endl;
	reverseLanderMovement();
	collisionEmitter.start();
	collisionEmitter.update();

	onBoardCam.setPosition(spaceCraft.position.x, spaceCraft.position.y, spaceCraft.position.z);
	onBoardCam.lookAt(spaceCraft.position);

	trackingCam.lookAt(ofVec3f(spaceCraft.position.x + 10, spaceCraft.position.y, spaceCraft.position.z));
}

//--------------------------------------------------------------
void ofApp::draw()
{
	loadExplosionVbo();
	loadThrusterVbo();
	// draw background image
	//

	glDepthMask(false);
	ofSetColor(255, 255, 255);
	backgroundImage.draw(0, 0, ofGetWindowWidth(), ofGetWindowHeight());
	if (!bHide)
		gui.draw();
	glDepthMask(true);

	theCam->begin();

	ofPushMatrix();
	// keyLight.draw();
	// fillLight.draw();
	// rimLight.draw();
	if (bWireframe)
	{ // wireframe mode  (include axis)
		ofDisableLighting();
		ofSetColor(ofColor::slateGray);
		mars.drawWireframe();
		if (bLanderLoaded)
		{

			//  Note this is how to rotate LEM model about the "Y" or "UP" axis
			//
			// lander.setRotation(0, angle, 0, 1, 0);
			lander.drawWireframe();
			if (!bTerrainSelected)
				drawAxis(lander.getPosition());
		}
		if (bTerrainSelected)
			drawAxis(ofVec3f(0, 0, 0));
	}
	else
	{
		ofEnableLighting(); // shaded mode
		mars.drawFaces();
		// ofMesh mesh;
		if (bLanderLoaded)
		{
			// lander.setRotation(0, angle, 0, 1, 0);
			lander.drawFaces();
			if (!bTerrainSelected)
				drawAxis(lander.getPosition());

			if (bLanderSelected || isCollided)
			{
				ofVec3f min = lander.getSceneMin() + lander.getPosition();
				ofVec3f max = lander.getSceneMax() + lander.getPosition();

				Box bounds = Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));
				ofNoFill();
				ofSetColor(ofColor::white);
				Octree::drawBox(bounds);
			}

			// draw colliding boxes
			//
			// ofSetColor(ofColor::green);
			for (int i = 0; i < colBoxList.size(); i++)
			{
				if (isCollided)
				{
					// ofPushStyle();
					ofSetColor(ofColor::red);
					Octree::drawBox(colBoxList[i]);
					// ofPopStyle();
				}
				else
				{
					ofSetColor(ofColor::green);
					Octree::drawBox(colBoxList[i]);
				}
			}
		}
	}
	if (bTerrainSelected)
		drawAxis(ofVec3f(0, 0, 0));

	if (bDisplayPoints)
	{ // display points as an option
		glPointSize(3);
		ofSetColor(ofColor::green);
		mars.drawVertices();
	}

	if (bDisplayOctree)
	{
		ofNoFill();
		// ofSetColor(ofColor::white);
		octree.draw(numLevels, 0);
	}
	ofDisableLighting();
	ofPopMatrix();
	theCam->end();

	glDepthMask(GL_FALSE);

	// this makes everything look glowy :)
	//
	ofEnableBlendMode(OF_BLENDMODE_ADD);
	ofEnablePointSprites();

	// begin drawing in the camera
	//
	shader.begin();
	theCam->begin();

	particleTex2.bind();
	// explosionVBO.enableColors();
	thrusterVBO.draw(GL_POINTS, 0, (int)thrusterEmitter.sys->particles.size());
	// explosionVBO.disableColors();
	particleTex2.unbind();

	if (catapultShip)
	{
		particleTex1.bind();
		explosionVBO.draw(GL_POINTS, 0, (int)collisionEmitter.sys->particles.size());
		particleTex1.unbind();
	}

	theCam->end();
	shader.end();

	ofDisablePointSprites();
	ofDisableBlendMode();
	ofEnableAlphaBlending();

	// set back the depth mask
	//
	glDepthMask(GL_TRUE);

	// draw screen data
	//
	string str;
	str += "Frame Rate: " + std::to_string(ofGetFrameRate());
	ofSetColor(ofColor::white);
	ofDrawBitmapString(str, ofGetWindowWidth() - 170, 15);

	// string str2;
	string str2;
	str2 = "Fuel time remaining: " + std::to_string(fuelTime) + "/";
	ofSetColor(ofColor::white);
	ofDrawBitmapString(str2, 5, 15);

	if (groundHeight)
	{
		string str3;
		gHeight = lander.getPosition().y - raySelectWithOctree();
		str3 += "Ground Height: " + std::to_string(gHeight);
		ofSetColor(ofColor::white);
		ofDrawBitmapString(str3, ofGetWindowWidth() - 170, 30);
	}
	string str41 = "Press space to restart!";
	ofSetColor(ofColor::red);
	ofDrawBitmapString(str41, ofGetWindowWidth() / 2 - 15, 30);
	float sp = spaceCraft.velocity.length();
	// if ((sp < 0.05 && gHeight < 0.4 && gHeight > 0.1))
	isInLandArea();
	if ((sp < 0.5 && inLandArea && !isCollided))
	// if (isCollided && !catapultShip)
	{
		backgroudSound.stop();
		// playingWinSound = true;
		if (!gameWinSound.isPlaying())
		{
			gameWinSound.play();
		}
		string str4 = "Game Over! Win!";
		ofSetColor(ofColor::red);
		ofDrawBitmapString(str4, ofGetWindowWidth() / 2 - 15, 15);
		// restart();
		spaceCraft.velocity.set(0, 0, 0);
		spaceCraft.acceleration.set(0, 0, 0);
		restartBool = false;
		float windowWidth = ofGetWidth();
		float windowHeight = ofGetHeight();
		float imgWidth = winImg.getWidth() * 8;
		float imgHeight = winImg.getHeight() * 8;

		float xPos = (windowWidth - imgWidth) / 2;
		float yPos = (windowHeight - imgHeight) / 2;
		winImg.draw(xPos, yPos, imgWidth, imgHeight);
		// cout<< "win: pos: x:"<<lander.getPosition().x<<" y:"<<lander.getPosition().y<<" z:"<<lander.getPosition().z<<endl;

		// ofDrawBitmapString(str41, ofGetWindowWidth()/2 - 15, 30);
	}
	else if (catapultShip || fuelTime <= 0)
	{
		backgroudSound.stop();
		if (catapultShip && playingExplosionSound)
		{
			explosionSound.setVolume(0.3);
			explosionSound.play();
			playingExplosionSound = false;
		}
		string str5 = "Game Over! Lose!";
		ofSetColor(ofColor::red);
		ofDrawBitmapString(str5, ofGetWindowWidth() / 2 - 15, 15);
		if (sp > 2500 || fuelTime <= 0)
		{
			fuelTime = 0;
			catapultShip = false;
			collisionEmitter.stop();
			if (playingGamenOverSound)
			{
				gameOverSound.play();
				playingGamenOverSound = false;
			}
			lander.setPosition(0, 0.5, 0);
			spaceCraft.velocity.set(0, 0, 0);
			spaceCraft.acceleration.set(0, 0, 0);
			restartBool = false;
		}
	}
}

// Draw an XYZ axis in RGB at world (0,0,0) for reference.
//
void ofApp::drawAxis(ofVec3f location)
{

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

float ofApp::getLandingForce(ofVec3f vel)
{
	ofVec3f collisionVelocity = vel;
	float landingForce = collisionVelocity.length();

	return landingForce;
}

void ofApp::keyPressed(int key)
{
	switch (key)
	{
	case 'B':
	case 'b':
		bDisplayBBoxes = !bDisplayBBoxes;
		break;
	case 'C':
	case 'c':
		if (cam.getMouseInputEnabled())
			cam.disableMouseInput();
		else
			cam.enableMouseInput();
		break;
	case 'F':
	case 'f':
		ofToggleFullscreen();
		break;
	case 'H':
	case 'h':
		theCam = &onBoardCam;
		break;
	case 'O':
	case 'o':
		bDisplayOctree = !bDisplayOctree;
		break;
	case 'P':
	case 'p':
		theCam = &trackingCam;
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
	case 'd': // rotate spacecraft clockwise (about Y (UP) axis)
		clockwiseRot = true;
		break;
	case 'a': // rotate spacecraft counter-clockwise (about Y (UP) axis)
		counterClockwiseRot = true;
		break;
	case 'w': // spacecraft thrust UP
		thrustUp = true;
		break;
	case 's': // spacefraft thrust DOWN
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
	case OF_KEY_UP: // move forward
		moveUp = true;
		break;
	case OF_KEY_DOWN: // move backward
		moveDown = true;
		break;
	case OF_KEY_LEFT: // move left
		moveLeft = true;
		break;
	case OF_KEY_RIGHT: // move right
		moveRight = true;
		break;
	case ' ':
		restart();
		// groundedEmitter.start();
		break;
	default:
		break;
	}
}

void ofApp::restart()
{
	lander.setRotation(0, 90, 0, 1, 0);
	lander.setPosition(0, 0.5, 0);

	catapultShip = false;
	spaceCraft.velocity.set(0, 0, 0);
	spaceCraft.acceleration.set(0, 0, 0);
	// spaceCraft.forces=shipGravity;
	getFlyDir = true;
	flySpeed = 0;
	collisionEmitter.setPosition(spaceCraft.position);
	collisionEmitter.stop();
	restartBool = true;
	explosionSound.stop();
	backgroudSound.play();
	gameOverSound.stop();
	gameWinSound.stop();
	playingGamenOverSound = true;
	playingExplosionSound = true;
}

void ofApp::toggleWireframeMode()
{
	bWireframe = !bWireframe;
}
void ofApp::toggleSelectTerrain()
{
	bTerrainSelected = !bTerrainSelected;
}

void ofApp::togglePointsDisplay()
{
	bDisplayPoints = !bDisplayPoints;
}

void ofApp::keyReleased(int key)
{

	switch (key)
	{

	case OF_KEY_ALT:
		cam.disableMouseInput();
		bAltKeyDown = false;
		break;
	case OF_KEY_CONTROL:
		bCtrlKeyDown = false;
		break;
	case OF_KEY_SHIFT:
		break;
	case OF_KEY_UP: // move forward
		moveUp = false;
		thrusterSound.stop();
		break;
	case OF_KEY_DOWN: // move backward
		moveDown = false;
		thrusterSound.stop();
		break;
	case OF_KEY_LEFT: // move left
		moveLeft = false;
		thrusterSound.stop();
		break;
	case OF_KEY_RIGHT: // move right
		moveRight = false;
		thrusterSound.stop();
		break;
	case 'd': // rotate spacecraft clockwise (about Y (UP) axis)
		clockwiseRot = false;
		break;
	case 'a': // rotate spacecraft counter-clockwise (about Y (UP) axis)
		counterClockwiseRot = false;
		break;
	case 'w': // spacecraft thrust UP
		thrustUp = false;
		thrusterSound.stop();
		break;
	case 's': // spacefraft thrust DOWN
		thrustDown = false;
		thrusterSound.stop();
		break;
	case 'x':
		// groundedEmitter.stop();
		break;
	default:
		break;
	}
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y)
{
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button)
{
	if (cam.getMouseInputEnabled())
		return;

	glm::vec3 origin = cam.getPosition();
	glm::vec3 mouseWorld = cam.screenToWorld(glm::vec3(mouseX, mouseY, 0));
	glm::vec3 mouseDir = glm::normalize(mouseWorld - origin);

	ofVec3f min = lander.getSceneMin() + lander.getPosition();
	ofVec3f max = lander.getSceneMax() + lander.getPosition();
	Ray ray(Vector3(origin.x, origin.y, origin.z), Vector3(mouseDir.x, mouseDir.y, mouseDir.z));

	Box bounds = Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));
	bool hit = bounds.intersect(ray, 0, 10000);
	if (hit)
	{
		bLanderSelected = true;
		mouseDownPos = getMousePointOnPlane(lander.getPosition(), cam.getZAxis());
		mouseLastPos = mouseDownPos;
		bInDrag = true;
	}
	else
	{
		bLanderSelected = false;
	}
}

float ofApp::raySelectWithOctree()
{
	ofVec3f pointRet(0, 0, 0);
	ofVec3f rayPoint = lander.getPosition();
	Ray ray = Ray(Vector3(rayPoint.x, rayPoint.y, rayPoint.z),
				  Vector3(0, -1, 0));
	// Ray ray = Ray(Vector3(rayPoint.x, 0, rayPoint.z), Vector3(rayPoint.x, rayPoint.y, rayPoint.z));

	pointSelected = octree.intersect(ray, octree.root, selectedNode);
	// }

	if (pointSelected)
	{
		pointRet = octree.mesh.getVertex(selectedNode.points[0]);
	}
	return pointRet.y;
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button)
{
	// if moving camera, don't allow mouse interaction
	//
	if (cam.getMouseInputEnabled())
	{
		return;
	}
	if (!bInDrag)
	{
		return;
	}

	if (bInDrag)
	{
		glm::vec3 landerPos = lander.getPosition();

		glm::vec3 mousePos = getMousePointOnPlane(landerPos, cam.getZAxis());
		// float offset = 2;
		// lander.setPosition(mousePos.x, mousePos.y - offset, mousePos.z);
		glm::vec3 delta = mousePos - mouseLastPos;

		landerPos += delta;
		lander.setPosition(landerPos.x, landerPos.y, landerPos.z);
		mouseLastPos = mousePos;

		// ofVec3f min = lander.getSceneMin() + lander.getPosition();
		// ofVec3f max = lander.getSceneMax() + lander.getPosition();

		// Box bounds = Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));

		// checkCollision();
	}
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button)
{
	bInDrag = false;
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y)
{
}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y)
{
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h)
{
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg)
{
}

//--------------------------------------------------------------
// setup basic ambient lighting in GL  (for now, enable just 1 light)
//
void ofApp::initLightingAndMaterials()
{

	static float ambient[] =
		{.5f, .5f, .5, 1.0f};
	static float diffuse[] =
		{1.0f, 1.0f, 1.0f, 1.0f};

	static float position[] =
		{5.0, 5.0, 5.0, 0.0};

	static float lmodel_ambient[] =
		{1.0f, 1.0f, 1.0f, 1.0f};

	static float lmodel_twoside[] =
		{GL_TRUE};

	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, position);

	glLightfv(GL_LIGHT1, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT1, GL_POSITION, position);

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
	glLightModelfv(GL_LIGHT_MODEL_TWO_SIDE, lmodel_twoside);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	//	glEnable(GL_LIGHT1);
	glShadeModel(GL_SMOOTH);
}

void ofApp::savePicture()
{
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
void ofApp::dragEvent(ofDragInfo dragInfo)
{
}

//  intersect the mouse ray with the plane normal to the camera
//  return intersection point.   (package code above into function)
//
glm::vec3 ofApp::getMousePointOnPlane(glm::vec3 planePt, glm::vec3 planeNorm)
{
	// Setup our rays
	//
	glm::vec3 origin = cam.getPosition();
	glm::vec3 camAxis = cam.getZAxis();
	glm::vec3 mouseWorld = cam.screenToWorld(glm::vec3(mouseX, mouseY, 0));
	glm::vec3 mouseDir = glm::normalize(mouseWorld - origin);
	float distance;

	bool hit = glm::intersectRayPlane(origin, mouseDir, planePt, planeNorm, distance);

	if (hit)
	{
		// find the point of intersection on the plane using the distance
		// We use the parameteric line or vector representation of a line to compute
		//
		// p' = p + s * dir;
		//
		glm::vec3 intersectPoint = origin + distance * mouseDir;

		return intersectPoint;
	}
	else
		return glm::vec3(0, 0, 0);
}

void ofApp::reverseLanderMovement()
{
	if (catapultShip)
	{
		// cout<< "pos: x:"<<lander.getPosition().x<<" y:"<<lander.getPosition().y<<" z:"<<lander.getPosition().z<<endl;
		flySpeed += 3;
		if (getFlyDir)
		{
			flyDir = lastMovementDirection;
			getFlyDir = false;
		}
		glm::vec3 reverseStep = flyDir * flySpeed * -1;
		// spaceCraft.velocity.set(0, 0, 0);
		spaceCraft.acceleration = reverseStep;
		spaceCraft.forces.set(0, 500, 0);
		spaceCraft.integrate();
		lander.setPosition(spaceCraft.position.x, spaceCraft.position.y, spaceCraft.position.z);
		currentPosition = lander.getPosition();
		lastPosition = currentPosition;
	}
	else if (isCollided)
	{
		// cout<< "collieded: pos: x:"<<lander.getPosition().x<<" y:"<<lander.getPosition().y<<" z:"<<lander.getPosition().z<<endl;
		float sp = spaceCraft.velocity.length();
		float stepBackSize = 100;
		glm::vec3 reverseStep = lastMovementDirection * stepBackSize * -1;
		spaceCraft.acceleration = reverseStep;
		spaceCraft.integrate();
		lander.setPosition(spaceCraft.position.x, spaceCraft.position.y, spaceCraft.position.z);
		currentPosition = lander.getPosition();
		lastPosition = currentPosition;
	}
	if (!isCollided)
	{
		getGroundHeight = raySelectWithOctree();
	}
}

void ofApp::checkCollision()
{
	ofVec3f min = lander.getSceneMin() + lander.getPosition();
	ofVec3f max = lander.getSceneMax() + lander.getPosition();

	Box bounds = Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));
	colBoxList.clear();
	octree.intersect(bounds, octree.root, colBoxList);
	collisionCount = colBoxList.size();
	isCollided = collisionCount >= 10;
}

void ofApp::isInLandArea()
{
	float landerX = lander.getPosition().x;
	float landerY = lander.getPosition().y;
	float landerZ = lander.getPosition().z;
	double distance1 = sqrt(pow(landPlace1.x - landerX, 2) + pow(landPlace1.y - landerY, 2) + pow(landPlace1.z - landerZ, 2));
	double distance2 = sqrt(pow(landPlace2.x - landerX, 2) + pow(landPlace2.y - landerY, 2) + pow(landPlace2.z - landerZ, 2));
	double distance3 = sqrt(pow(landPlace3.x - landerX, 2) + pow(landPlace3.y - landerY, 2) + pow(landPlace3.z - landerZ, 2));
	if ((distance1 < radius1) && (landerY - landPlace1.y < 1.5))
	{
		inLandArea = true;
	}
	else if ((distance2 < radius2) && (landerY - landPlace2.y < 1.5))
	{
		inLandArea = true;
	}
	else if ((distance3 < radius3) && (landerY - landPlace3.y < 1.5))
	{
		inLandArea = true;
	}
	else
	{
		inLandArea = false;
	}
}