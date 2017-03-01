#include "TextureFilter.h"

namespace TextureFilter {

void TextureFilterBase::applyFilter(const ofTexture & input, ofFbo & output, int nPasses)
{
	allocatePingpongs(input);
	setupPlane(input);

	ofFbo* src = &pingpong[0];
	ofFbo* dst = &pingpong[1];

	// init src
	src->begin();
	ofClear(0);
	input.draw(0, 0, src->getWidth(), src->getHeight());
	src->end();

	// ping pong
	for (int i = 0; i < nPasses; i++) {

		if (!filterSrcToDst(src, dst)) {	// do the shader
			ofLogError("TextureFilter") << "unable to run filter at pass: " << i;
			break;						// break if error
		}

		if (i == nPasses - 1) break; // last pass don't flip flop 

		// flipflop
		ofFbo* tmp = src;
		src = dst;
		dst = tmp;
	}

	// draw to output
	allocateFbo(output, input);
	output.begin();
	ofClear(0);
	dst->draw(0, 0, output.getWidth(), output.getHeight());
	output.end();
}

bool TextureFilterBase::allocatePingpongs(const ofTexture & input)
{
	if (!input.isAllocated()) return false;
	bool didIt = false;
	for (int i = 0; i < 2; i++) {
		didIt = allocateFbo(pingpong[i], input);
	}
	return didIt;
}

// static function allocates an fbo with texture dims and GL_RGBA
// ---------------------------------------------------------------------------

bool TextureFilterBase::allocateFbo(ofFbo& fbo, const ofTexture& texture) {
	if (!texture.isAllocated()) return false;
	float w = texture.getWidth();
	float h = texture.getHeight();
	if (fbo.getWidth() != w || fbo.getHeight() != h || fbo.getTexture().getTextureData().glInternalFormat != GL_RGBA) {
		fbo.allocate(w, h, GL_RGBA);
		fbo.begin();
		ofClear(0);
		fbo.end();
		return true;
	}
	return fbo.isAllocated();
}

// set up plane dims + pos to match texture dims
// ---------------------------------------------------------------------------

bool TextureFilterBase::setupPlane(const ofTexture & input)
{
	if (!input.isAllocated()) return false;
	float w = input.getWidth();
	float h = input.getHeight();
	if (plane.getWidth() != w || plane.getHeight() != h) {	// avoid unnecessary mesh reconstruction
		plane.set(w, h, 2, 2);								// texture dims and 2x2 grid
	}
	plane.resetTransform();
	plane.setPosition(w * 0.5, h * 0.5, 0);					// make sure centered on texture dims
	return true;
}


// expands stringified source, making new lines after ';' and '{' and '}'
// ---------------------------------------------------------------------------
string TextureFilterBase::expandLines(const string& src) {

	string s = src;
	ofStringReplace(s, ";", ";\n");
	ofStringReplace(s, "{", "{\n");
	ofStringReplace(s, "}", "}\n");
	return s;
}


} // end TextureFilter namespace