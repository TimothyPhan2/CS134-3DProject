
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
	cam.setDistance(10);
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

	// set current camera;
	//
	theCam = &cam;

	// setup rudimentary lighting
	//
	initLightingAndMaterials();

	mars.loadModel("geo/moon-houdini.obj");
	// mars.loadModel("geo/mars-low-5x-v2.obj");
	mars.setScaleNormalization(false);

	gui.setup();
	gui.add(velocity.setup("Velocity", ofVec3f(0, -20, 0), ofVec3f(0, -30, 0), ofVec3f(100, 100, 100)));
	gui.add(lifespan.setup("Lifespan", 3.0, .1, 10.0));
	gui.add(rate.setup("Rate", 1.0, .5, 60.0));
	gui.add(radius.setup("Disk Radius", 0.5, .1, 10.0));
	gui.add(numLevels.setup("Number of Octree Levels", 1, 1, 10));

	ofSetVerticalSync(true);
	ofEnableSmoothing();
	ofEnableDepthTest();

	// load BG image
	//
	bBackgroundLoaded = backgroundImage.load("images/background.jpg");

	octree.create(mars.getMesh(0), 20);
	// setup rudimentary lighting
	//
	// initLightingAndMaterials();
	cout << "Number of Verts: " << mars.getMesh(0).getNumVertices() << endl;

	// load lander model
	//
	if (lander.loadModel("geo/lander.obj"))
	{
		lander.setScaleNormalization(false);
		lander.setScale(1, 1, 1);
		lander.setRotation(0, 0, 1, 0, 0);
		lander.setPosition(0, 0, 0);

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
}

void ofApp::startThruster(ParticleEmitter &emitter)
{
	emitter.start();
	emitter.oneShot = true;
}

void ofApp::update()
{

	headingVec = ofVec3f(0, 0, 0);

	// groundedEmitter.setLifespan(lifespan / 10);
	// groundedEmitter.setVelocity(ofVec3f(velocity));
	// groundedEmitter.setRate(rate);
	// groundedEmitter.setRadius(radius);

	// groundedEmitter.update();
	// thrusterEmitter.setLifespan(lifespan / 10);
	// thrusterEmitter.setVelocity(ofVec3f(velocity));
	// thrusterEmitter.setRate(rate);
	// thrusterEmitter.setRadius(radius);
	if (!bInDrag)
	{
		spaceCraft.setPosition(lander.getPosition());
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

		// Prevents the lander from going below the grid
		// if (spaceCraft.position.y < 0)
		// {
		// 	spaceCraft.position.y = 0;
		// }

		lander.setPosition(spaceCraft.position.x, spaceCraft.position.y, spaceCraft.position.z);
	}

	thrusterEmitter.setPosition(spaceCraft.position);

	thrusterEmitter.update();

	if (bLanderLoaded)
	{
		currentPosition = lander.getPosition();

		if (currentPosition != lastPosition)
		{
			lastMovementDirection = currentPosition - lastPosition;
			lastMovementDirection = glm::normalize(lastMovementDirection);
		}

		lastPosition = currentPosition;

		checkCollision();
		if (isCollided)
		{
			reverseLanderMovement();
		}
	}
}

//--------------------------------------------------------------
void ofApp::draw()
{

	// draw background image
	//
	if (bBackgroundLoaded)
	{
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
	// ofPushMatrix();
	// ofRotate(90, 0, 0, 1);
	// ofSetLineWidth(1);
	// ofSetColor(ofColor::dimGray);
	// ofDisableLighting();
	// ofDrawGridPlane();
	// ofPopMatrix();

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
		ofMesh mesh;
		if (bLanderLoaded)
		{
			// lander.setRotation(0, angle, 0, 1, 0);
			lander.drawFaces();
			if (!bTerrainSelected)
				drawAxis(lander.getPosition());
			// if (bDisplayBBoxes)
			// {
			// 	ofNoFill();
			// 	ofSetColor(ofColor::white);
			// 	cout<<"here????"<<endl;
			// 	for (int i = 0; i < lander.getNumMeshes(); i++)
			// 	{
			// 		cout<<"here------"<<endl;
			// 		ofPushMatrix();
			// 		cout<<"here????"<<endl;
			// 		ofMultMatrix(lander.getModelMatrix());
			// 		ofRotate(-90, 1, 0, 0);
			// 		Octree::drawBox(bboxList[i]);
			// 		ofPopMatrix();
			// 	}
			// }

			// ofVec3f min = lander.getSceneMin() + lander.getPosition();
			// ofVec3f max = lander.getSceneMax() + lander.getPosition();

			// Box bounds = Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));
			// ofNoFill();
			// ofSetColor(ofColor::white);
			// Octree::drawBox(bounds);

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

	ofPopMatrix();

	// groundedEmitter.draw();
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
		break;
	case 'O':
	case 'o':
		bDisplayOctree = !bDisplayOctree;
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
		// groundedEmitter.start();
		break;
	default:
		break;
	}
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
		break;
	case OF_KEY_DOWN: // move backward
		moveDown = false;
		break;
	case OF_KEY_LEFT: // move left
		moveLeft = false;
		break;
	case OF_KEY_RIGHT: // move right
		moveRight = false;
		break;
	case 'd': // rotate spacecraft clockwise (about Y (UP) axis)
		clockwiseRot = false;
		break;
	case 'a': // rotate spacecraft counter-clockwise (about Y (UP) axis)
		counterClockwiseRot = false;
		break;
	case 'w': // spacecraft thrust UP
		thrustUp = false;
		break;
	case 's': // spacefraft thrust DOWN
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

	if (bLanderLoaded)
	{
		// glm::vec3 origin = cam.getPosition();
		// glm::vec3 mouseWorld = cam.screenToWorld(glm::vec3(mouseX, mouseY, 0));
		// glm::vec3 mouseDir = glm::normalize(mouseWorld - origin);

		// ofVec3f min = lander.getSceneMin() + lander.getPosition();
		// ofVec3f max = lander.getSceneMax() + lander.getPosition();
		// Ray ray(Vector3(origin.x, origin.y, origin.z), Vector3(mouseDir.x, mouseDir.y, mouseDir.z));

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
	else
	{
		// 	float startTime;
		// 	if (timingInfoToggle)
		// 	{
		// 		startTime = ofGetElapsedTimeMillis();
		// 	}
		ofVec3f p;
		raySelectWithOctree(p);

		// 	// Ray ray(Vector3(origin.x, origin.y, origin.z), Vector3(mouseDir.x, mouseDir.y, mouseDir.z));
		// 	// TreeNode selectedNode;
		// 	// float startTime = ofGetElapsedTimeMillis();
		// 	// if (octree.intersect(ray, octree.root, selectedNode))
		// 	if (pointSelected && numLevels==5)
		// 	{
		// 		bPointSelected = true;
		// 		selectedPoint = getClosestPointToRay(selectedNode, ray);
		// 		// drawSphereAtPoint(selectedPoint);
		// 	}
		// 	else
		// 	{
		// 		bPointSelected = false;
		// 	}
		// 	if (timingInfoToggle)
		// 	{
		// 		float endTime = ofGetElapsedTimeMillis();
		// 		cout << "Time for ray intersection: " << endTime - startTime << " ms" << endl;
		// 	}
	}
}

bool ofApp::raySelectWithOctree(ofVec3f &pointRet)
{
	ofVec3f mouse(mouseX, mouseY);
	ofVec3f rayPoint = cam.screenToWorld(mouse);
	ofVec3f rayDir = rayPoint - cam.getPosition();
	rayDir.normalize();
	// if (timingInfoToggle)
	// {
	// 	float startTime = ofGetElapsedTimeMillis();
	// 	Ray ray = Ray(Vector3(rayPoint.x, rayPoint.y, rayPoint.z),
	// 				  Vector3(rayDir.x, rayDir.y, rayDir.z));

	// 	pointSelected = octree.intersect(ray, octree.root, selectedNode);
	// 	float endTime = ofGetElapsedTimeMillis();
	// 	cout << "Time for ray intersection: " << endTime - startTime << " ms" << endl;
	// }
	// else
	// {
	Ray ray = Ray(Vector3(rayPoint.x, rayPoint.y, rayPoint.z),
				  Vector3(rayDir.x, rayDir.y, rayDir.z));

	pointSelected = octree.intersect(ray, octree.root, selectedNode);
	// }

	if (pointSelected)
	{
		pointRet = octree.mesh.getVertex(selectedNode.points[0]);
	}
	return pointSelected;
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
		cout << "drag: x:" << delta.x << " y:" << delta.y << " z:" << delta.z << endl;

		landerPos += delta;
		cout << "lander: x:" << landerPos.x << " y:" << landerPos.y << " z:" << landerPos.z << endl;
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

ofVec3f ofApp::getClosestPointToRay(const TreeNode &node, const Ray &ray)
{
	float minDistance = FLT_MAX;
	ofVec3f closestPoint;
	// glm::vec3 rayOrigin(ray.origin.x(), ray.origin.y(), ray.origin.z());
	for (int idx : node.points)
	{
		ofVec3f point = mars.getMesh(0).getVertex(idx);
		glm::vec3 glmPoint(point.x, point.y, point.z);
		float distance = pointToRayDistance(glmPoint, ray); // Simple distance calculation from ray origin to point
		if (distance < minDistance)
		{
			minDistance = distance;
			closestPoint = point;
		}
	}
	return closestPoint;
}

float ofApp::pointToRayDistance(const glm::vec3 &point, const Ray &ray)
{
	glm::vec3 p1 = glm::vec3(ray.origin.x(), ray.origin.y(), ray.origin.z());
	glm::vec3 p2 = p1 + glm::vec3(ray.direction.x(), ray.direction.y(), ray.direction.z()); // Assuming ray.direction is normalized
	glm::vec3 pVec = point - p1;
	glm::vec3 rayVec = p2 - p1;

	// Project point onto the ray
	float t = glm::dot(pVec, rayVec) / glm::dot(rayVec, rayVec);
	glm::vec3 projection = p1 + rayVec * t;
	return glm::length(projection - point);
}

void ofApp::reverseLanderMovement()
{
	float stepBackSize = 0.1;
	glm::vec3 reverseStep = lastMovementDirection * stepBackSize*-1;

	// lander.setPosition(lander.getPosition().x + reverseStep);
	glm::vec3 newPosition = lander.getPosition() + reverseStep;
	lander.setPosition(newPosition.x, newPosition.y, newPosition.z);

	currentPosition = lander.getPosition();
	lastPosition = currentPosition;
	checkCollision();

	if (!isCollided)
	{
		isReverseAnimationActive = false;
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
