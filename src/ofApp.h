#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxAssimpModelLoader.h"
#include "Particle.h"
#include "ParticleEmitter.h"
#include <glm/gtx/intersect.hpp>
#include "Octree.h"

class ofApp : public ofBaseApp
{

public:
	void setup();
	void update();
	void draw();

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);
	void drawAxis(ofVec3f);
	void initLightingAndMaterials();
	void savePicture();
	void toggleWireframeMode();
	void togglePointsDisplay();

	ofEasyCam cam;
	ofxAssimpModelLoader lander, mars;
	ofLight light;
	ofImage backgroundImage;
	ofCamera *theCam = NULL;
	ofCamera topCam;

	ofVec3f headingVec;
	float headingSpeed = 5.0f;
	bool bAltKeyDown;
	bool bCtrlKeyDown;
	bool bWireframe;
	bool bDisplayPoints;

	bool bBackgroundLoaded = false;
	bool bLanderLoaded = false;

	bool moveUp = false;
	bool moveDown = false;
	bool moveLeft = false;
	bool moveRight = false;
	bool thrustUp = false;
	bool thrustDown = false;
	bool clockwiseRot = false;
	bool counterClockwiseRot = false;

	float angle = 0.0f;
	float angularVelocity = 0.0f;
	float angularAcceleration = 0.0f;
	Particle spaceCraft;

	ParticleEmitter groundedEmitter;
	ParticleEmitter thrusterEmitter;

	ofxPanel gui;
	ofxFloatSlider radius;
	ofxVec3Slider velocity;
	ofxFloatSlider lifespan;
	ofxFloatSlider rate;
	ofxIntSlider numLevels;

	void startThruster(ParticleEmitter &emitter);
	// ofShader shader;

	bool bLanderSelected = false;
	bool bTerrainSelected;
	bool bInDrag = false;
	glm::vec3 mouseDownPos, mouseLastPos;
	bool bDisplayOctree = false;
	bool bDisplayBBoxes = true;
	float pointToRayDistance(const glm::vec3 &point, const Ray &ray);
	ofVec3f getClosestPointToRay(const TreeNode &node, const Ray &ray);

	bool isCollided = false;
	bool isReverseAnimationActive = false;
	glm::vec3 lastMovementDirection;
	int collisionCount = 0;
	void checkCollision();
	void reverseLanderMovement();
	void toggleSelectTerrain();
	glm::vec3 getMousePointOnPlane(glm::vec3 p, glm::vec3 n);
	// bool mouseIntersectPlane(ofVec3f planePoint, ofVec3f planeNorm, ofVec3f &point);
	bool raySelectWithOctree(ofVec3f &pointRet);
	// void dragEvent2(ofDragInfo dragInfo);
	glm::vec3 currentPosition;
	glm::vec3 lastPosition;
	ofVec3f selectedPoint;
	ofVec3f intersectPoint;

	vector<Box> bboxList;
	Box boundingBox, landerBounds;
	vector<Box> colBoxList;
	TreeNode selectedNode;
	Octree octree;
	bool pointSelected = false;
};
