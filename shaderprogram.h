#ifndef SHADERPROGRAM_H
#define SHADERPROGRAM_H

#include <GL/glew.h>

typedef GLuint ShaderProgram;

ShaderProgram* shaderprogram_ctor(
	ShaderProgram* self, 
	const char* vshaderpath,
	const char* fshaderpath
);
void shaderprogram_dtor(ShaderProgram* self);

#endif
