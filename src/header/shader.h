#pragma once
#include "Common.h"

class Shader
{
public:
	QOpenGLFunctions_4_5_Compatibility ex;
	QOpenGLFunctions* gl{};
	int CompileShader(const char* vertex_shader_text, const char* fragment_shader_text, const char* geometry_shader_text, GLuint* vertex_shader, GLuint* fragment_shader, GLuint* geometry_shader);
	int CompileComputeShader(const char* compute_shader_text, GLuint* compute_shader);
	int CompileComputeShader(const char* compute_shader_text, GLuint* compute_shader, char* errbuf);
	int Create1DImageTex32F(int width);
	int Create2DImageTex32F(int width, int height);
	int Create3DImageTex32F(int width, int height, int depth);
	void init() {
		ex.initializeOpenGLFunctions();
	}
};

