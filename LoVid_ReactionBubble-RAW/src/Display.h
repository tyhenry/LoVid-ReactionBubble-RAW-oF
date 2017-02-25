#pragma once

#include "ofMain.h"

class Display {

public:

	Display(ofRectangle area = ofRectangle()) 
	{
		setBounds(area);
	}
	Display(float w, float h)
	{
		setBounds(ofRectangle(0,0,w,h));
	}
	Display(float x, float y, float w, float h)
	{
		setBounds(ofRectangle(x,y,w,h));
	}
	~Display() {}


	virtual void setBounds(ofRectangle area) { 
		bounds = area;
		allocate(bounds);
	}
	ofRectangle getBounds()		{ return bounds; }

	virtual void begin()		{ fbo.begin(); }
	virtual void end()			{ fbo.end(); }
	virtual void draw()			{ fbo.draw(bounds); }
	virtual void draw(float x, float y, float w, float h)
	{
		fbo.draw(x,y,w,h);
	}
	virtual void draw(float x, float y)
	{
		fbo.draw(x,y,bounds.width, bounds.height);
	}

protected:

	virtual bool allocate(ofRectangle size) {
		if (!fbo.isAllocated() || fbo.getWidth() != size.width || fbo.getHeight() != size.height) {
			fbo.allocate(size.width, size.height, GL_RGBA);
		}
		return fbo.isAllocated();
	}
	
	ofRectangle bounds;
	ofFbo fbo;

};