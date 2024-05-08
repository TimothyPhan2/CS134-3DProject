#pragma once

#include "ofMain.h"

class ParticleForceField;

class Particle {
public:
	Particle();

	ofVec3f position;
	ofVec3f velocity;
	ofVec3f acceleration;
	float	damping;
	float   mass;
	float   lifespan;
	float   radius;
	// ofVec3f forces;
	float   birthtime;
	void    integrate();
	void    draw();
	float   age();        // sec
	ofColor color;
	void setPosition(glm::vec3 newPos);
};


