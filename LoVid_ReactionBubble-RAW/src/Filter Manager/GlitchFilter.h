#pragma once

#include "TextureFilter.h"

using namespace TextureFilter;

namespace Filter {

class Glitch : public TextureFilterBase {

public:

	Glitch()	{}
	~Glitch()	{}

	void reset()
	{
		glitch = 1.; // range 0-1
	}

	void setGlitch(float pct)	{ glitch = pct; }
	float getGlitch()			{ return glitch; }


	bool filterSrcToDst(ofFbo* src, ofFbo* dst) override
	{

		if (!src || !dst) return false;

		dst->begin();
		ofClear(0);
		shader.begin();
		ofTexture& tex0 = src->getTexture();
		shader.setUniformTexture("u_tex0", tex0, 0);
		shader.setUniform2f("u_tex0Resolution", tex0.getWidth(), tex0.getHeight());
		shader.setUniform1f("u_glitch", glitch);
		shader.setUniform1f("u_time", ofGetElapsedTimef());
		plane.draw();
		shader.end();
		dst->end();

		return true;
	}

private:

    float glitch = 0.;

	string fragmentShader() override
	{
		return versionSrc + fragShaderSrc;
	}

	// fragment shader

	const string fragShaderSrc = STRINGIFY(

        uniform float u_glitch; // 0-1
		uniform float u_time;
		
		uniform sampler2D u_tex0;
		uniform vec2 u_tex0Resolution;

		varying vec2 texCoordVarying;

                  
        void main (void)
        {
            vec2 st = texCoordVarying;
                      
            //Glitch \n
            //vec2 st_glitch = st; \n
            //vec2 res = vec2(textureW, textureH); \n
            //st_glitch = st/(res*1.2); //normalize (w/ offset?) \n
			vec2 st_glitch = st;
            st_glitch.y = 1.-st_glitch.y; //invert y \n
                      
            st_glitch *= 400.; //multiply space \n
                      
            vec2 st_glitchIndex = floor(st_glitch);
                      
            //st_glitch = fract(st_glitch); //repeat space \n
                      
            if (mod(st_glitchIndex.x,2.) == 0.) { //if even \n
                st_glitch.y -= sin(u_time+5.*u_glitch*10.)*10.*u_glitch;
            } else {
                st_glitch.y += cos(u_time+7.*u_glitch*10.)*10.*u_glitch;
            }
                      
            if (mod(st_glitchIndex.y,floor(abs(sin(u_glitch*10.)*10.))) == 0.) { //if even \n
                st_glitch.x -= 100.*abs(sin(st_glitch.y*mod(u_time,3.+u_glitch)*0.01))*u_glitch;
                          
            } else {
                st_glitch.x += 100.*abs(cos(st_glitch.y*mod(u_time,2.+u_glitch)*0.01))*u_glitch;
                          
            }
                      
            st_glitch /= 400.;
                      
            st_glitch.y = 1. - st_glitch.y;
            //st_glitch *= res*1.2; \n
                      
            vec3 color = vec3(texture2D(u_tex0,st_glitch));

			//color = vec3(1.,0.,0.); \n
			gl_FragColor = vec4(color, 1.);
		}
	);

};

}