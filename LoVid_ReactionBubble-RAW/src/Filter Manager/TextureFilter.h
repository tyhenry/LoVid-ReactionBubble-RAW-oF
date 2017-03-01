#pragma once

#include "ofMain.h"

#define STRINGIFY(A) #A

namespace TextureFilter{

	class TextureFilterBase {

	public:

		TextureFilterBase() {}
		~TextureFilterBase() {}

		void applyFilter(const ofTexture& input, ofFbo& output, int nPasses);	// runs filterSrcToDst() for n passes
		
		const ofTexture& applyFilter(const ofTexture& input, int nPasses) {		// stores and returns result
			applyFilter(input, result, nPasses);
			return result.getTexture();
		}

		const ofFbo& getResult() { return result; }

		virtual void draw(const ofTexture& input, float x, float y, float w, float h, int nPasses) {
			applyFilter(input, nPasses);
			result.draw(x,y,w,h);
		}
		virtual void draw(const ofTexture& input, float x, float y, int nPasses) {
			applyFilter(input, nPasses);
			result.draw(x,y);
		}

		// links the shader sources
		virtual bool linkShader() {
			return
				(
					shader.setupShaderFromSource(GL_VERTEX_SHADER, expandLines(vertexShader())) &&
					shader.setupShaderFromSource(GL_FRAGMENT_SHADER, expandLines(fragmentShader())) &&
					shader.linkProgram()
				);
		}

		ofShader shader;

	protected:

		ofFbo pingpong[2], result;
		ofPlanePrimitive plane;

		bool allocatePingpongs(const ofTexture& input);
		bool static allocateFbo(ofFbo& fbo, const ofTexture& texture);
		bool setupPlane(const ofTexture& input);
		static string expandLines(const string& src); // expands STRINGIFY shader source, adding new lines

		// mutable functions:

		// runs the shader
		virtual bool filterSrcToDst(ofFbo* src, ofFbo* dst) 
		{
			if (!src || !dst) return false;

			dst->begin();
			ofClear(0);
			shader.begin();
			ofTexture& tex0 = src->getTexture();
			shader.setUniformTexture("u_tex0", tex0, 0);
			shader.setUniform2f("u_tex0Resolution", tex0.getWidth(), tex0.getHeight());
			plane.draw();
			shader.end();
			dst->end();

			return true;
		}

		// prepare shader code
		virtual string vertexShader() {
			return versionSrc + vertShaderSrc;
		}
		virtual string fragmentShader() {
			return versionSrc + expandLines(fragShaderSrc);
		}

		// DEFAULT SHADER CODE

		// GLSL version declaration
		const string versionSrc = "#version 120\n";

		// vertex shader
		const string vertShaderSrc = STRINGIFY(
			varying vec2 texCoordVarying;
			void main() {
				texCoordVarying = gl_MultiTexCoord0.xy;
				gl_Position = ftransform();
			}
		);

		// fragment shader
		const string fragShaderSrc = STRINGIFY(
			uniform sampler2D u_tex0;
			uniform vec2 u_tex0Resolution;
			varying vec2 texCoordVarying;
			void main() {
				gl_FragColor = texture2D(u_tex0, texCoordVarying);
			}
		);
	};

}