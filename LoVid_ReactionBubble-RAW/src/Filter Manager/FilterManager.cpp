#include "FilterManager.h"

const ofTexture & FilterManager::applyFilters(const ofTexture & input)
{
	float w = input.getWidth();
	float h = input.getHeight();
	if (w == 0 || h == 0) {
		ofLogError("Filter Manager") << "zero texture dimension, w: " << w << ", h: " << h;
		return result.getTexture();
	}

	if (!result.isAllocated() || result.getWidth() != w || result.getHeight() != h)
	{
		result.allocate(w,h,GL_RGBA); result.begin(); ofClear(0); result.end();	// allocate, clear
		ofLogNotice("FilterManager") << "allocating fbo to texture input w,h: " << w << ", " << h;
	}

	result.begin(); ofClear(0); input.draw(0,0); result.end();

	if (bTint || bKaleido || bPixelate || bBrcosa || bGlitch) {

	// 1. tint or skip
		const ofTexture& tintR = bTint ? tint.applyFilter(result.getTexture(), 1) : result.getTexture();
		// 2. kaleido or skip
		const ofTexture& kaleidoR = bKaleido ? kaleido.applyFilter(tintR, 1) : tintR;
		// 3. pixelate or skip
		const ofTexture& pixelateR = bPixelate ? pixelate.applyFilter(kaleidoR, 1) : kaleidoR;
		// 4. brocsa or skip
		const ofTexture& brcosaR = bBrcosa ? brcosa.applyFilter(pixelateR, 1) : pixelateR;
		// 5. glitch or skip
		const ofTexture& glitchR = bGlitch ? glitch.applyFilter(brcosaR, 1) : brcosaR;

		result.begin();
		glitchR.draw(0, 0, glitchR.getWidth(), glitchR.getHeight());	
		result.end();
	}

	return result.getTexture();
}

void FilterManager::draw(const ofTexture & input, float x, float y, float w, float h)
{
	const ofTexture& t = applyFilters(input);
	t.draw(x,y,w,h);
}
