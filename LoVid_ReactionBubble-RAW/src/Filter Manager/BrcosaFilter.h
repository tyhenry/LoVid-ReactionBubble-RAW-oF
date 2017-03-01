#pragma once

#include "TextureFilter.h"

using namespace TextureFilter;

namespace Filter {

class Brcosa : public TextureFilterBase {

public:

	Brcosa()	{}
	~Brcosa() {}

	void reset()
	{
		saturation = 1.; // range 0-2
		brightness = 1.; // 0-2
		contrast = 1.;   // 0.25-4
	}

	void setBrt(float brt)	{ brightness = brt; }
	void setSat(float sat)	{ saturation = sat; }
	void setCon(float con)	{ contrast = con; }

	float getBrt()			{ return brightness; }
	float getSat()			{ return saturation; }
	float getCon()			{ return contrast; }

	bool filterSrcToDst(ofFbo* src, ofFbo* dst) override
	{

		if (!src || !dst) return false;

		dst->begin();
		ofClear(0);
		shader.begin();
		ofTexture& tex0 = src->getTexture();
		shader.setUniformTexture("u_tex0", tex0, 0);
		shader.setUniform2f("u_tex0Resolution", tex0.getWidth(), tex0.getHeight());
		shader.setUniform1f("u_saturation", saturation);
		shader.setUniform1f("u_contrast", contrast);
		shader.setUniform1f("u_brightness", brightness);
		plane.draw();
		shader.end();
		dst->end();

		return true;
	}

private:

    ofVec3f avgluma = ofVec3f(0.62); //default vec3(0.62f)
    float saturation = 1.;
    float contrast = 1.;
    float brightness = 1.;

	string fragmentShader() override
	{
		return versionSrc + fragShaderSrc;
	}

	// TINT FILTER SHADER SOURCE

	// fragment shader

	const string fragShaderSrc = STRINGIFY(

		const vec3 LumCoeff = vec3(0.2125, 0.7154, 0.0721); //BrCoSa const \n
        const vec3 avgluma = vec3(0.62); //default vec3(0.62f)

        uniform float u_saturation;
        uniform float u_contrast;
        uniform float u_brightness;
		
		uniform sampler2D u_tex0;
		uniform vec2 u_tex0Resolution;

		varying vec2 texCoordVarying;

		void main (void)
        {
			vec2 st = texCoordVarying;
			vec4 c = texture2D(u_tex0,st);
			vec3 color = c.rgb;

			vec3 intensity  = vec3(dot(color, LumCoeff));
			color			= mix(intensity, color, u_saturation);
			color           = mix(avgluma, color, u_contrast);
			color          *= u_brightness;

			//color = vec3(1.,0.,0.);
			gl_FragColor = vec4(color, 1.);
		}
	);

};

}