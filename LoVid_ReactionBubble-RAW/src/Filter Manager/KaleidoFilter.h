#pragma once

#include "TextureFilter.h"

using namespace TextureFilter;

namespace Filter {

class Kaleido : public TextureFilterBase {

public:

	Kaleido() {}
	~Kaleido() {}

	void reset()				
	{ 	
		centerPct = ofVec2f(.5,.5);
		zoom = 10.;
		angle = 0.;
		nDivs = 10.;
	}

	void setCenter(ofVec2f pct)	{ centerPct = pct; }
	void setZoom(float scale)	{ zoom = scale; }
	void setAngle(float deg)	{ angle = deg; }
	void setNumDivs(int divs)	{ nDivs = ofClamp(divs,1,360); }

	ofVec2f getCenter()	{ return centerPct; }
	float getZoom()		{ return zoom; }
	float getAngle()	{ return angle; }
	float getNumDivs()	{ return nDivs; }


	bool filterSrcToDst(ofFbo* src, ofFbo* dst) override
	{
		if (!src || !dst) return false;


		ofTexture& tex0 = src->getTexture();
		src->getTexture().setTextureWrap(GL_MIRRORED_REPEAT, GL_MIRRORED_REPEAT);

		dst->begin();
		ofClear(0);
		shader.begin();
		shader.setUniformTexture("u_tex0", tex0, 0);
		shader.setUniform2f("u_tex0Resolution", tex0.getWidth(), tex0.getHeight());
		shader.setUniform2f("u_texCenter", centerPct.x*tex0.getWidth(), centerPct.y*tex0.getHeight());
		shader.setUniform1f("u_zoom", zoom);
		shader.setUniform1f("u_angle", angle);
		shader.setUniform1i("u_divs", nDivs);
		plane.draw();
		shader.end();
		dst->end();

		return true;
	}

private:

	ofVec2f centerPct = ofVec2f(.5,.5);
	float zoom = 1.;
	float angle = 0.;
	int nDivs = 10;

	string fragmentShader() override
	{
		static string fragShaderHead = "#define PI (3.14159265358979323846) \n#define TWO_PI (2*PI) \n";
		return versionSrc + fragShaderHead + fragShaderSrc;
	}

	// KALEIDO FILTER SHADER SOURCE

	// fragment shader

	const string fragShaderSrc = STRINGIFY(

		uniform sampler2D u_tex0;
		uniform vec2 u_tex0Resolution;

		uniform vec2 u_texCenter = vec2(0.);
		uniform float u_zoom = 1.;
		uniform float u_angle = 0.;
		uniform int u_divs = 10;

		varying vec2 texCoordVarying;


		void main(){
			vec2 st = texCoordVarying; // normalized coords \n

			// zoom \n
			//st /= u_tex0Resolution; \n
			st += u_zoom*0.5-0.5;
			st /= u_zoom;
			st *= u_tex0Resolution; // pixel coords \n

			// vector to px from texture center \n
			vec2 v = st - (u_tex0Resolution * 0.5);

			float r = length(v);  // radius to px \n
			float a = atan(v.y, v.x);  // angle to px \n

			float A = TWO_PI / float(u_divs);	// angle of divisions \n
			a = mod( a, A );					// bound px angle to div angle \n
			if ( a > A/2.0 ) a = A - a;			// invert along middle of angle \n
			a -= u_angle;						// offset resultant angle \n

			vec2 u = vec2( cos(a), sin(a) ) * r;	// vector to resultant px in texture \n
			u += u_texCenter;						// shift vector to center point \n



			st = u / u_tex0Resolution; // normalize coords \n
			vec3 color = texture2D(u_tex0, st).rgb;
			gl_FragColor = vec4(color, 1.);
		}
	);

};

}

/*
			// add spokes \n
			r = r/u_tex0Resolution.x;				// normalize \n

			float pct = smoothstep(0.01,0.05, a/A);	// spokes \n
			pct += (1.-smoothstep(0.11,0.12,r));	// remove inner spokes \n
			pct = clamp(pct,0.,1.);

			// inner circle \n
			float circle = smoothstep(0.1,0.103,r);
			circle *= 1.-smoothstep(0.103,0.106,r);
			//pct -= circle; \n
			//pct *= (1.-(1.-smoothstep(0.04,0.05, r)) * smoothstep(0.05,0.6, r)); \n

			// vignette \n
			pct -= pow(r*4.,50.);//pow(r*2.0, 50.); \n

			// lighten dark fx \n
			pct += 0.1;
			pct = clamp(pct,0.,1.);
*/