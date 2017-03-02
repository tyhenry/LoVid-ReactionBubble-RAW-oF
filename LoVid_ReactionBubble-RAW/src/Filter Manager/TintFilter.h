#pragma once

#include "TextureFilter.h"

using namespace TextureFilter;

namespace Filter {

class Tint : public TextureFilterBase {

public:

	Tint()	{}
	~Tint() {}

	void reset()
	{
		hue = ofVec3f(1.,0.,0.);
		threshold = 0.;
		power = 1.;
		mixIn = 0.;
	}

	void setHue(ofVec3f rgb)		{ hue = rgb; }
	void setHue(ofFloatColor c)		{ hue.x = c.r; hue.y = c.g; hue.z = c.b; }
	void setThreshold(float pct)	{ threshold = pct; }
	void setPower(float pow)		{ power = pow; }
	void setMixIn(float mix)		{ mixIn = mix; }

	ofVec3f getHue()		{ return hue; }
	float getThreshold()	{ return threshold; }
	float getPower()		{ return power; }

	bool filterSrcToDst(ofFbo* src, ofFbo* dst) override
	{

		if (!src || !dst) return false;

		dst->begin();
		ofClear(0);
		shader.begin();
		ofTexture& tex0 = src->getTexture();
		shader.setUniformTexture("u_tex0", tex0, 0);
		shader.setUniform2f("u_tex0Resolution", tex0.getWidth(), tex0.getHeight());
		shader.setUniform3f("u_color", hue);
		shader.setUniform1f("u_mixIn", mixIn);
		shader.setUniform1f("u_threshold", threshold);
		shader.setUniform1f("u_power", power);
		plane.draw();
		shader.end();
		dst->end();

		return true;
	}

private:

	ofVec3f hue		= ofVec3f(1.,0.,0.);
	float threshold	= 0.;
	float power		= 1.;
	float mixIn		= 0.; // no mix

	string fragmentShader() override
	{
		return versionSrc + fragShaderSrc;
	}

	// TINT FILTER SHADER SOURCE

	// fragment shader

	const string fragShaderSrc = STRINGIFY(

		uniform sampler2D u_tex0;
		uniform vec2 u_tex0Resolution;

		uniform float u_threshold = 0.0;
		uniform float u_power = 1.;
		uniform vec3 u_color = vec3(1.,0.,0.);
		uniform float u_mixIn = 0.;

		varying vec2 texCoordVarying;

		float map(float val, float lo, float hi, float newLo, float newHi, bool bClamp){

			if (bClamp) {
				if (val < lo){ return newLo; }
				if (val > hi){ return newHi; }
			}

			float pct = (val-lo)/(hi-lo);					// % of val in lo-hi range \n
			return ((newHi-newLo) * pct) + newLo;			// calc + shift \n
		}


		void main () {
			vec2 st = texCoordVarying;
			vec3 color = texture2D(u_tex0,st).rgb;

			float threshold = u_threshold;
			float power = u_power;
			float mixIn = u_mixIn;

			float l = (color.r + color.g + color.b) / 3.;		// luma \n

			float pct = map(l, threshold, 1., 0., 1., true);	// map luma from threshold to 0 \n
			pct = pow(pct,u_power);

			vec3 tint = u_color;

			//vec3 color2 = mix(color,tint,0.5); //\n
			//color = mix(color,color2,pct); //\n

			color = mix(color,tint,0.6*mixIn); //\n

			gl_FragColor = vec4(color, 1.);
		}
	);

};

}