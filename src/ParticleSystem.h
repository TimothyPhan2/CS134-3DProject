#pragma once
//  Kevin M. Smith - CS 134 SJSU

#include "ofMain.h"
#include "Particle.h"

class ParticleForce {
protected:
public:
	bool applyOnce = false;
	bool applied = false;
	virtual void updateForce(Particle *) = 0;
};

class ParticleSystem {
public:
	void add(const Particle &);
	void remove(int);
	void update();
	void setLifespan(float);
	int removeNear(const ofVec3f & point, float dist);
	void addForce(ParticleForce *f);
	void reset();

	void draw();
	vector<Particle> particles;
	vector<ParticleForce *> forces;
};

class GravityForce: public ParticleForce {
	ofVec3f gravity;
public:
	GravityForce(const ofVec3f & gravity);
	void updateForce(Particle *);
};

class TurbulenceForce : public ParticleForce {
	ofVec3f tmin, tmax;
public:
	TurbulenceForce(const ofVec3f & min, const ofVec3f &max);
	void updateForce(Particle *);
};

class ImpulseRadialForce : public ParticleForce {
	float magnitude;
	float heightLimit;
public:
	ImpulseRadialForce(float magnitude); 
	void updateForce(Particle *);
	void setMagnitude(float mag) {
        magnitude = mag;
    }

    // Setter for heightLimit to control the 'y' component
    void setHeightLimit(float limit) {
        heightLimit = limit;
    }
};
