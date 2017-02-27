#pragma once

#include "ofMain.h"

class Worm {

public:

	Worm();
	Worm(ofRectangle b, ofVec2f p = ofVec2f(), float r = 3., float maxspeed = 1.5, float maxforce = .05);
	~Worm() {}

	bool loadSkinVid(string vidFile);
	bool loadSkinVid(ofVideoPlayer vid);

	bool setSkinTex(ofTexture& tex) { skinTex = &tex; }

	bool update(); // move, wriggle
	void draw(); //
	void drawSpine();
	void drawMesh(bool wireframe = false);

	void avoidEdges();
	void wrapEdges();

	void resetForces();
	void applyForce(ofVec2f force);

	ofVec2f seek(ofVec2f target);
	ofVec2f avoid(ofVec2f target);
	ofVec2f seek(ofVec2f target, float maxDist);
	ofVec2f avoid(ofVec2f target, float maxDist);

	void flock(vector<Worm>& worms, float sepW = 1.5f, float aliW = 0.1f, float cohW = 1.f); // weights
	ofVec2f separate (vector<Worm>& worms, float desiredSeparation = 40.f);
	ofVec2f align (vector<Worm>& worms, float neighborDist = 50.f);
	ofVec2f cohesion (vector<Worm>& worms, float neighborDist = 50.f);

	void makeSpine();
	void makeMesh();

	ofVec2f pos, vel, acc; ofVec3f pos3d;
	float radius, maxForce, maxSpeed;

	ofRectangle bounds;

	ofVideoPlayer skinVid;
	ofTexture* skinTex;

	ofPolyline spine;
	deque<ofVec3f> spinePts;
	int maxPts = 200;

	ofMesh mesh;

	float timeOffset = 0;
	float wriggle = 5.;
	//float zOffset = 0;

	const float sqrt3 = 1.73205;
	// 30,60,90 triangle sides = x*sqrt3, x, 2x (hypotenuse)
};