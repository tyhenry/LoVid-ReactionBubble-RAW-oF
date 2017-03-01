#pragma once

#include "ofMain.h"
#include "KaleidoFilter.h"
#include "PixelateFilter.h"
#include "TintFilter.h"
#include "BrcosaFilter.h"
#include "GlitchFilter.h"

class FilterManager {

public:

	FilterManager() {}
	~FilterManager() {}

	bool setup() {
		ofLogVerbose("FilterManager") << "linking shaders:";
		bool b = true; bool bL = true;
		bL = kaleido.linkShader(); 
		b = b && bL;
		ofLogVerbose("FilterManager") << "\t - kaleido: " << bL;
		bL = pixelate.linkShader(); 
		b = b && bL;
		ofLogVerbose("FilterManager") << "\t - pixelate: " << bL;
		bL = tint.linkShader(); 
		b = b && bL;
		ofLogVerbose("FilterManager") << "\t - tint: " << bL;
		bL = brcosa.linkShader(); 
		b = b && bL;
		ofLogVerbose("FilterManager") << "\t - brcosa: " << bL;
		bL = glitch.linkShader(); 
		b = b && bL;
		ofLogVerbose("FilterManager") << "\t - glitch: " << bL;

		result.allocate(1,1,GL_RGBA);
		return b;
	}

	void reset()							{	bTint = false; bKaleido = false; bPixelate = false;	bBrcosa = false; bGlitch = false;
												resetUniforms(); 
												resetInteraction();
											}
	void resetUniforms()					{ tint.reset(); kaleido.reset(); pixelate.reset(); brcosa.reset(); glitch.reset();}

	// drawing

	const ofTexture& applyFilters(const ofTexture& input);
	void draw(const ofTexture& input, float x, float y, float w, float h);
	void draw(const ofTexture& input, float x, float y)	{ draw(input,x,y,input.getWidth(),input.getHeight()); }
	void draw(const ofTexture& input) { draw(input,0,0); }

	// interaction

	void setXBounds(float x, float w)		{ bounds[0] = ofVec2f(x,w); }
	void setYBounds(float y, float h)		{ bounds[1] = ofVec2f(y,h); }
	void setXYBounds(ofRectangle area)		{ setXBounds(area.x,area.width); setYBounds(area.y,area.height); }
	void setZBounds(float z, float depth)	{ bounds[2] = ofVec2f(z,depth); }


	void setPos(ofVec3f pt)					{ posPrev = pos; pos = pt; }
	void setPosX(float x)					{ pos.x = x; }
	void setPosY(float y)					{ pos.y = y; }
	void setPosZ(float z)					{ pos.z = z; }

	void resetInteraction()					{ resetBounds(); resetPos(); }
	void resetBounds()						{ for (auto& b : bounds) b = ofVec2f(); }
	void resetPos()							{ pos = ofVec3f(); posPrev = ofVec3f(); }

	ofVec3f& getPos()						{ return pos; }
	ofVec3f& getPosPrev()					{ return posPrev; }

	ofVec2f& xBounds()						{ return bounds[0]; }
	ofVec2f& yBounds()						{ return bounds[1]; }
	ofVec2f& zBounds()						{ return bounds[2]; }

	float getXPct()							{ return getPct(pos.x, bounds[0]); }
	float getYPct()							{ return getPct(pos.y, bounds[1]); }
	float getZPct()							{ return getPct(pos.z, bounds[2]); }

	// filters
	
	Filter::Kaleido kaleido;
	Filter::Pixelate pixelate;
	Filter::Tint tint;
	Filter::Brcosa brcosa;
	Filter::Glitch glitch;
	bool bTint, bKaleido, bPixelate, bBrcosa, bGlitch = false;

	// texture output

	ofFbo result;

protected:

	float getPct(float val, ofVec2f bound) {
		return ofMap(val, bound.x, bound.x + bound.y, 0., 1., false);
	}

	ofVec3f pos, posPrev = ofVec3f();
	vector<ofVec2f> bounds = { ofVec2f(), ofVec2f(), ofVec2f() };

};