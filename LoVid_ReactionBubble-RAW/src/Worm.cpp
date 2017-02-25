#include "Worm.h"

Worm::Worm()
{
	acc.set(0, 0);
	vel.set(ofRandom(-1, 1), ofRandom(-1, 1));
	bounds.set(ofGetCurrentViewport());
	pos.set(bounds.getCenter());

	radius = 3.;

	maxSpeed = 1.5;
	maxForce = 0.05;

	timeOffset = ofRandom(100);
	
	cout << "created worm, bounds: " << bounds << ", pos: " << pos << ", vel: " << vel << endl;
}

Worm::Worm(ofRectangle b, ofVec2f p, float r, float maxspeed, float maxforce)
{
	acc.set(0,0);
	vel.set(ofRandom(-1,1),ofRandom(-1,1));
	bounds.set(b);
	if (!bounds.inside(p)) p = bounds.getCenter();
	pos.set(p);

	radius = r;
	
	maxSpeed = maxspeed;
	maxForce = maxforce;

	timeOffset = ofRandom(100);

	cout << "created worm, bounds: " << bounds << "\n\tpos: " << pos << ", vel: " << vel << endl;

}

bool Worm::update() 
{
	skinVid.update();

	vel += acc;
	vel.limit(maxSpeed);
	pos += vel;

	avoidEdges();

	ofVec3f pos3d = pos;
	pos3d.z = sin(ofGetElapsedTimef()*.5 + timeOffset) * radius*wriggle;

	spinePts.push_front(pos3d);
	if (spinePts.size() > maxPts) spinePts.pop_back();

	makeSpine();
	makeMesh();

	return true;
}

void Worm::draw()
{
	ofPushMatrix();
	ofTranslate(pos);
	float rot = atan2(vel.y, vel.x);
	ofRotate(ofRadToDeg(rot));

	ofDrawBox(radius);
	//ofDrawAxis(100);

	//ofTriangle(-40, 10, -40, -10, 0, 0);
	ofPopMatrix();

}

void Worm::drawSpine()
{
	spine.draw();
}

void Worm::drawMesh(bool wireframe)
{
	if (wireframe) mesh.drawWireframe();
	else mesh.drawFaces();
}

void Worm::avoidEdges() {

	// wrap
	if (pos.x < bounds.getLeft()) {
		pos.x = bounds.getLeft();
		vel.x *= -1;
	}
	if (pos.y < bounds.getTop()) {
		pos.y = bounds.getTop();
		vel.y *= -1;
	}
	if (pos.x > bounds.getRight()) {
		pos.x = bounds.getRight();
		vel.x *= -1;
	}
	if (pos.y > bounds.getBottom()) {
		pos.y = bounds.getBottom();
		vel.y *= -1;
	}
}

void Worm::wrapEdges() {

	// wrap
	if (pos.x < bounds.getLeft() - radius) pos.x = bounds.getRight() + radius;
	if (pos.y < bounds.getTop() - radius) pos.y = bounds.getBottom() + radius;
	if (pos.x > bounds.getRight() + radius) pos.x = bounds.getLeft() - radius;
	if (pos.y > bounds.getBottom() + radius) pos.y = bounds.getTop() - radius;
}

void Worm::flock(vector<Worm>& worms, float sepW, float aliW, float cohW) {

	ofVec2f sep = separate(worms);   // Separation
	ofVec2f ali = align(worms);      // Alignment
	ofVec2f coh = cohesion(worms);   // Cohesion

	// Arbitrarily weight these forces
	sep *= sepW;
	ali *= aliW;
	coh *= cohW;

	// Add the force vectors to acceleration
	applyForce(sep);
	applyForce(ali);
	applyForce(coh);
}

void Worm::resetForces()
{
	acc *= 0;
}

void Worm::applyForce(ofVec2f force)
{
	acc += force;
}

ofVec2f Worm::seek(ofVec2f target)
{
	ofVec2f desired = target - pos;
	desired.normalize();
	desired *= maxSpeed;

	ofVec2f steer = desired - vel;
	steer.limit(maxForce);
	return steer;
}

ofVec2f Worm::avoid(ofVec2f target) 
{
	ofVec2f desired = target - pos; // opposite direction as seek
	desired.normalize();
	desired *= maxSpeed;

	ofVec2f steer = desired - vel;
	steer.limit(maxForce);
	return steer;
}

ofVec2f Worm::seek(ofVec2f target, float maxDist) {
	ofVec2f steer = seek(target);
	float d = target.distance(pos);
	steer *= ofMap(d, 0, maxDist, 1, 0, true);
	return steer;
}

ofVec2f Worm::avoid(ofVec2f target, float maxDist) {
	ofVec2f steer = avoid(target);
	float d = target.distance(pos);
	steer *= ofMap(d, 0, maxDist, 1, 0, true);
	return steer;
}

ofVec2f Worm::separate(vector<Worm>& worms, float desiredSeparation) {

	ofVec2f steer = ofVec2f(0, 0);
	int count = 0;

	// For every Worm in the system, check if it's too close
	for (auto& other : worms) {
		float d = pos.distance(other.pos);

		if ((d > 0) && (d < desiredSeparation)) { // needs to separate

			ofVec2f diff = pos - other.pos;
			diff.normalize();
			diff /= d; //stronger if closer
			steer += diff;
			count++; // Keep track of how many
		}
	}

	// Average all steering forces
	if (count > 0) {
		steer /= (float)count;
	}

	// calculate final separate steering force
	if (steer.length() > 0) {
		steer.normalize();  // ???
		steer *= maxSpeed; 
		steer -= vel;       // ???
		steer.limit(maxForce);
	}

	return steer;
}


ofVec2f Worm::align(vector<Worm>& worms, float neighborDist) {

	ofVec2f sum = ofVec2f(0, 0);
	int count = 0;

	for (auto& other : worms) {
		float d = ofDist(pos.x, pos.y, other.pos.x, other.pos.y);

		if ((d > 0) && (d < neighborDist)) {
			sum += other.vel;
			count++;
		}
	}

	if (count > 0) {
		sum /= (float)count;
		sum.normalize();
		sum *= maxSpeed;
		ofVec2f steer = sum - vel;
		steer.limit(maxForce);
		return steer;

	}
	else {
		return ofVec2f(0, 0);
	}
}

ofVec2f Worm::cohesion(vector<Worm>& worms, float neighborDist) {

	ofVec2f sum = ofVec2f(0, 0);
	int count = 0;

	for (auto& other : worms) {
		float d = ofDist(pos.x, pos.y, other.pos.x, other.pos.y);
		if ((d > 0) && (d < neighborDist)) {
			sum += other.pos; // Add location
			count++;
		}
	}
	if (count > 0) {
		sum /= count;
		return seek(sum);  // Steer towards the location
	}
	else {
		return ofVec2f(0, 0);
	}
}

void Worm::makeSpine()
{
	spine.clear();
	for (auto& pt : spinePts) {
		spine.addVertex(pt);
	}

}

void Worm::makeMesh()
{
	mesh.clear();
	mesh.setMode(OF_PRIMITIVE_TRIANGLES);

	float rotation = sin(ofGetElapsedTimef() + timeOffset) * 180.;

	ofPolyline smoothSpine = spine.getResampledByCount(30);
	smoothSpine.getSmoothed(5);
	for (int i = 0; i < smoothSpine.size()-1; i++) {

		// get vector from next pt to this one
		ofNode helper;
		helper.setPosition(smoothSpine[i+1]);
		helper.lookAt(smoothSpine[i]);
		helper.setPosition(smoothSpine[i]);
		ofVec3f pos, pt1, pt2, pt3;
		pos = helper.getGlobalPosition();
		pt1 = pos + helper.getYAxis() * radius;
		float shorty = radius / sqrt3;
		pt2 = pos + helper.getXAxis() * shorty;
		pt3 = pos - helper.getXAxis() * shorty;

		float rot = i==0 ? rotation : rotation / i;

		ofVec3f pivot = pos;
		ofVec3f axis = helper.getZAxis();
		pt1.rotate(rot, pivot, axis);
		pt2.rotate(rot, pivot, axis);
		pt3.rotate(rot, pivot, axis);

		//mesh.addVertex(pos);
		mesh.addVertex(pt1);
		mesh.addVertex(pt2);
		mesh.addVertex(pt3);

	}

	// tex coords
	int nVerts = mesh.getNumVertices();
	for (int i = 0; i < nVerts; i += 3) {

		ofVec2f t1, t2, t3;
		float x = (float)i / (float)nVerts;
		t1.set(x, 0.0); // y?
		t2.set(x, 0.05);
		t3.set(x, 0.1);
		mesh.addTexCoord(t1);
		mesh.addTexCoord(t2);
		mesh.addTexCoord(t3);
	}

	// indices

	for (int i = 0; i+2 < nVerts; i += 3) {
		// face
		//mesh.addTriangle(i, i+1, i+2);
		// this to prev face
		if (i > 3) {
			// 6 triangles

			// side 1
			mesh.addTriangle(i, i-3, i-2);
			mesh.addTriangle(i, i+1, i-2);

			// side 2
			mesh.addTriangle(i+1, i-1, i-2);
			mesh.addTriangle(i+1, i+2, i-1);

			// side 3
			mesh.addTriangle(i+2, i, i-3);
			mesh.addTriangle(i+2, i-1, i-3);
		}
	}

	//mesh.setupIndicesAuto();
}

bool Worm::loadSkinVid(string vidFile)
{
	return false;
}

bool Worm::loadSkinVid(ofVideoPlayer vid)
{
	return false;
}
