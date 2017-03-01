#pragma once

#include "TextureFilter.h"

using namespace TextureFilter;

namespace Filter {

class Pixelate : public TextureFilterBase {

public:

	Pixelate() {}
	~Pixelate() {}

	void reset()					{ nDivs = ofVec2f(100); }

	void setNumDivs(ofVec2f num)	{ nDivs = num; }
	ofVec2f getNumDivs()			{ return nDivs; }

	bool filterSrcToDst(ofFbo* src, ofFbo* dst) override
	{

		if (!src || !dst) return false;

		dst->begin();
		ofClear(0);
		shader.begin();
		ofTexture& tex0 = src->getTexture();
		shader.setUniformTexture("u_tex0", tex0, 0);
		shader.setUniform2f("u_tex0Resolution", tex0.getWidth(), tex0.getHeight());
		shader.setUniform2f("u_divs", nDivs);
		plane.draw();
		shader.end();
		dst->end();

		return true;
	}

private:

	ofVec2f nDivs = ofVec2f(100);

	string fragmentShader() override
	{
		return versionSrc + fragShaderSrc;
	}

	// PIXEL FILTER SHADER SOURCE

	// fragment shader

	const string fragShaderSrc = STRINGIFY(

		uniform sampler2D u_tex0;

		uniform vec2 u_tex0Resolution;
		uniform vec2 u_divs = vec2(100.,100.);

		varying vec2 texCoordVarying;

		void main(void){
			vec2 st = texCoordVarying * u_tex0Resolution;

			// # divs \n
			vec2 divs = vec2(max(u_divs.x,1.), max(u_divs.y,1.)); // min 1 div \n
			// div size \n
			vec2 divPx = u_tex0Resolution/divs;

			vec2 uv;
			uv.x = divPx.x*floor(st.x/divPx.x);		// left edge of pixel block \n
			uv.y = divPx.y*floor(st.y/divPx.y);		// top edge \n
			uv += divPx * vec2(0.5);				// shift to center \n

			vec3 color = texture2D(u_tex0, uv/u_tex0Resolution).rgb;

			gl_FragColor = vec4(color,1.);
		}
	);

};

}