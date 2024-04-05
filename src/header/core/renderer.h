#pragma once
#include "../Common.h"
#include "../shader.h"
#define _IN_MAIN_
#include "precompile.h"
#undef _IN_MAIN_
#include <unordered_set>


struct vec4
{
	float x{}, y{}, z{}, w{};
};
struct vec3
{
	float x{}, y{}, z{};
};
struct vec2
{
	float x{}, y{};
};
class texture
{
private:
	std::wstring file{};
	GLuint id{};
	enum {
		_1D = GL_TEXTURE_1D,
		_2D = GL_TEXTURE_2D,
		_3D = GL_TEXTURE_3D,
	} dim;
public:
	bool loadBmp2D(std::wstring file);
	GLuint getID() { return id; }
	void bind() { glBindTexture(dim, id); }
	texture() {}
	void release() {
		if (file == L"" || !id) return;
		glDeleteTextures(1, &id);
		file = L"";
	}
	~texture() {
		release();
	}
};

class RenderSystem
{
public:
	enum VarType {
		VEC4,
		VEC3,
		VEC2,
		FLOAT,
		MATRIX2x2,
		MATRIX3x3,
		MATRIX4x4,
		INT,
		COLOR3,
		COLOR4,
	};
	struct var {
		union {
			vec4 data_4;
			vec3 data_3;
			vec2 data_2;
			float data_1;
			int data_int;
			float data_matrix2x2[4];
			float data_matrix3x3[9];
			float data_matrix4x4[16];
			vec4 data_color4;
			vec3 data_color3;
		};
		VarType type;
		union {
			vec4 default_4;
			vec3 default_3;
			vec2 default_2;
			float default_1;
			int default_int;
			float default_matrix2x2[4];
			float default_matrix3x3[9];
			float default_matrix4x4[16];
			vec4 default_color4;
			vec3 default_color3;
		};
	};
	struct Project {
		std::wstring MLang_file{};
		std::vector<std::wstring> Shaders{};
		std::wstring Var_file{};
	} project;
	double time{};
	std::wstring _ProjectFile{};
	std::wstring _ProjectDir{};
	enum {
		TEX,
		MAP,
		MESH
	} RenderMode;
	QOpenGLFunctions_4_5_Compatibility gl{};
	Shader shader{};
	void (*info_callback)(const wchar_t* info) {};
private:
	std::vector<var> vars;
	std::vector<int> ComputeShaders;
	std::vector<int> ComputePrograms;
public:
	bool build(std::wstring ProjectFile);
	void enterUpdate();
	void update();
	void leaveUpdate();
	void create();
	void release();
	void reset();
	void info(const wchar_t* info);
	std::wstring getShaderWithoutInclude(std::wstring cs, std::unordered_set<std::wstring>& map);
	static unsigned int functions(unsigned int this_, const wchar_t* func);
};