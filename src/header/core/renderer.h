#pragma once
#include "../Common.h"
#include "../shader.h"
#include "precompile.h"
#include <unordered_set>
using namespace MLang;

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
	}
};

class GLImage {
	GLuint type{};
	GLuint tex{};
	Shader* shader{};
	GLuint width{};
	GLuint height{};
	GLuint depth{};
	GLuint binding{};
	public:
		GLImage(Shader* s) {
			shader = s;
		}
		void release() {
			if (tex) {
				glDeleteTextures(1, &tex);
				tex = 0;
			}
			width = 0;
			height = 0;
			depth = 0;
		}
		void create1D(unsigned int w) {
			release();
			type = GL_TEXTURE_1D;
			width = w;
			tex = shader->Create1DImageTex32F(width);
		}
		void create2D(unsigned int w, unsigned int h) {
			release();
			type = GL_TEXTURE_2D;
			width = w;
			height = h;
			tex = shader->Create2DImageTex32F(width, height);
		}
		void create3D(unsigned int w, unsigned int h, unsigned int d) {
			release();
			type = GL_TEXTURE_3D;
			width = w;
			height = h;
			depth = d;
			tex = shader->Create3DImageTex32F(width, height, depth);
		}
		void bindTex() {
			glBindTexture(type, tex);
		}
		void bindImage() {
			shader->ex.glBindImageTexture(binding, tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
		}
		void setBinding(GLuint b) {
			binding = b;
		}
		~GLImage() {}
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
		TEXTURE1D,
		TEXTURE2D,
		TEXTURE3D,
		IMAGE1D,
		IMAGE2D,
		IMAGE3D,
	};
	class var {
	public:
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
			texture data_tex;
			GLImage data_image;
		};
		VarType type;
		std::wstring name;
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
			texture default_tex;
			GLImage default_image;
		};
		var() {
			memset(this, 0, sizeof(var));
		
		}
		~var() {}
		void release() {
			if (type == TEXTURE1D || type == TEXTURE2D || type == TEXTURE3D) {
				data_tex.release();
			}
			if (type == IMAGE1D || type == IMAGE2D || type == IMAGE3D) {
				data_image.release();
			}
		
		}
		void operator=(const var& v) {
			memcpy(this, &v, sizeof(var));
			if (type == TEXTURE1D || type == TEXTURE2D || type == TEXTURE3D) {
				data_tex = v.data_tex;
			}
			if (type == IMAGE1D || type == IMAGE2D || type == IMAGE3D) {
				data_image = v.data_image;
			}
		}
		var(const var& v) {
			memcpy(this, &v, sizeof(var));
			if (type == TEXTURE1D || type == TEXTURE2D || type == TEXTURE3D) {
				data_tex = v.data_tex;
			}
			if (type == IMAGE1D || type == IMAGE2D || type == IMAGE3D) {
				data_image = v.data_image;
			}
		}

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
	x86Runner runner{};
public:
	unsigned int computeShader_workgroup_size = 16;
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
	static unsigned int callbacks(unsigned int this_, const wchar_t* name, const wchar_t* func);
};