

#include "../header/shader.h"
int Shader::CompileShader(const char* vertex_shader_text, const char* fragment_shader_text, const char* geometry_shader_text, GLuint* vertex_shader, GLuint* fragment_shader, GLuint* geometry_shader) {
	int program; bool err = false;

	if (vertex_shader_text != nullptr) {
		*vertex_shader = gl->glCreateShader(GL_VERTEX_SHADER);
		gl->glShaderSource(*vertex_shader, 1, &vertex_shader_text, NULL);
		gl->glCompileShader(*vertex_shader);
		GLint a = false;
		gl->glGetShaderiv(*vertex_shader, 0x8B81, &a);
		if (!a)
		{
			char buf[2048]; int err_size;
			gl->glGetShaderInfoLog(*vertex_shader, 2047, &err_size, (char*)buf);
			std::cout << buf << std::endl; err |= true;
		}
	}
	if (geometry_shader_text != nullptr) {
		*geometry_shader = gl->glCreateShader(GL_GEOMETRY_SHADER);
		gl->glShaderSource(*geometry_shader, 1, &geometry_shader_text, NULL);
		gl->glCompileShader(*geometry_shader);
		GLint a = false;
		gl->glGetShaderiv(*geometry_shader, 0x8B81, &a);
		if (!a)
		{
			char buf[2048]; int err_size;
			gl->glGetShaderInfoLog(*geometry_shader, 2047, &err_size, (char*)buf);
			std::cout << buf << std::endl; err |= true;
		}
	}
	if (vertex_shader_text != nullptr) {
		*fragment_shader = gl->glCreateShader(GL_FRAGMENT_SHADER);
		gl->glShaderSource(*fragment_shader, 1, &fragment_shader_text, NULL);
		gl->glCompileShader(*fragment_shader);
		GLint a = false;
		gl->glGetShaderiv(*fragment_shader, 0x8B81, &a);
		if (!a)
		{
			char buf[2048]; int err_size;
			gl->glGetShaderInfoLog(*fragment_shader, 2047, &err_size, (char*)buf);
			std::cout << buf << std::endl; err |= true;
		}
	}
	program = gl->glCreateProgram();
	if (vertex_shader_text != nullptr) gl->glAttachShader(program, *vertex_shader);
	if (fragment_shader_text != nullptr) gl->glAttachShader(program, *fragment_shader);
	if (geometry_shader_text != nullptr) gl->glAttachShader(program, *geometry_shader);
	gl->glLinkProgram(program);
	GLint a = false;
	gl->glGetShaderiv(program, GL_COMPILE_STATUS, &a);
	//if (!a)
	//{
	//	char buf[2048]; int err_size;
	//	glGetShaderInfoLog(program, 2047, &err_size, (char*)buf);
	//	println(buf); err |= true;
	//}

	return err ? -1 : program;

}
int Shader::CompileComputeShader(const char* compute_shader_text, GLuint* compute_shader) {
	int program; bool err = false;
	if (compute_shader_text == nullptr) return -1;

	if (compute_shader_text != nullptr) {
		*compute_shader = gl->glCreateShader(GL_COMPUTE_SHADER);
		gl->glShaderSource(*compute_shader, 1, &compute_shader_text, NULL);
		gl->glCompileShader(*compute_shader);
		GLint a = false;
		gl->glGetShaderiv(*compute_shader, 0x8B81, &a);
		if (!a)
		{
			char buf[2048]; int err_size;
			gl->glGetShaderInfoLog(*compute_shader, 2047, &err_size, (char*)buf);
			std::cout << buf << std::endl; err |= true;
			OutputDebugStringA(buf);
		}
	}
	program = gl->glCreateProgram();
	gl->glAttachShader(program, *compute_shader);
	gl->glLinkProgram(program);
	return err ? -1 : program;

}

int Shader::CompileComputeShader(const char* compute_shader_text, GLuint* compute_shader, char* errbuf) {
	int program; bool err = false;
	if (compute_shader_text == nullptr) return -1;

	if (compute_shader_text != nullptr) {
		*compute_shader = gl->glCreateShader(GL_COMPUTE_SHADER);
		gl->glShaderSource(*compute_shader, 1, &compute_shader_text, NULL);
		gl->glCompileShader(*compute_shader);
		GLint a = false;
		gl->glGetShaderiv(*compute_shader, 0x8B81, &a);
		if (!a)
		{
			int err_size;
			gl->glGetShaderInfoLog(*compute_shader, 2047, &err_size, errbuf);
			err |= true;
			OutputDebugStringA(errbuf);
		}
	}
	program = gl->glCreateProgram();
	gl->glAttachShader(program, *compute_shader);
	gl->glLinkProgram(program);
	return err ? -1 : program;

}

int Shader::Create1DImageTex32F(int width) {
	GLuint tex;
	gl->glGenTextures(1, &tex);
	gl->glBindTexture(GL_TEXTURE_1D, tex);
	gl->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	gl->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	gl->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	ex.glTexStorage1D(GL_TEXTURE_1D, 1, width, GL_RGBA32F);
	gl->glBindTexture(GL_TEXTURE_1D, 0);
	return tex;
}
int Shader::Create2DImageTex32F(int width, int height) {
	GLuint tex;
	gl->glGenTextures(1, &tex);
	gl->glBindTexture(GL_TEXTURE_2D, tex);
	ex.glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, width, height);
	gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);	
	gl->glBindTexture(GL_TEXTURE_2D, 0);
	return tex;
}
int Shader::Create3DImageTex32F(int width, int height, int depth) {
	GLuint tex;
	gl->glGenTextures(1, &tex);
	gl->glBindTexture(GL_TEXTURE_3D, tex);
	gl->glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	gl->glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	gl->glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	gl->glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	gl->glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	ex.glTexStorage3D(GL_TEXTURE_3D, 1, GL_RGBA32F, width, height, depth);
	gl->glBindTexture(GL_TEXTURE_3D, 0);
	return tex;
}
