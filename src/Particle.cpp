#include "Particle.h"


Particle::Particle() {

	// initialize particle with some reasonable values first;
	//
	velocity.set(0, 0, 0);
	acceleration.set(0, 0, 0);
	position.set(0, 0, 0);
	lifespan = 5;
	birthtime = 0;
	radius = .1;
	damping = .99;
	mass = 1;
	color = ofColor::aquamarine;
}

void Particle::draw() {
	ofSetColor(color);
	ofDrawSphere(position, radius);
}

// write your own integrator here.. (hint: it's only 3 lines of code)
//
void Particle::integrate() {

	if (ofGetFrameRate() < 1.0) return;  // avoidng divide by zero errors

	float dt = 1.0 / ofGetFrameRate();

	
	position += (velocity * dt);

	velocity += acceleration * dt;

	
	velocity *= damping;

	
}

//  return age in seconds
//
float Particle::age() {
	return (ofGetElapsedTimeMillis() - birthtime)/30;
}
void Particle::setPosition(glm::vec3 newPos){
	position.x = newPos.x;
	position.y = newPos.y;
	position.z = newPos.z;
}


