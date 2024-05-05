#pragma once
#include "../Common.h"
#include "../shader.h"
#include "camera.h"
#include "precompile.h"
#include <unordered_set>
#include <exception>
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
			float data_matrix4x4[16]{};
			vec4 data_color4;
			vec3 data_color3;
			texture data_tex;
			GLImage data_image;
		};
		VarType type{};
		std::wstring name{};
		union {
			vec4 default_4;
			vec3 default_3;
			vec2 default_2;
			float default_1;
			int default_int;
			float default_matrix2x2[4];
			float default_matrix3x3[9];
			float default_matrix4x4[16]{};
			vec4 default_color4;
			vec3 default_color3;
			texture default_tex;
			GLImage default_image;
		};
		var() {
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
			switch (v.type)
			{
			case VEC4:
				data_4 = v.data_4;
				break;
			case VEC3:
				data_3 = v.data_3;
				break;
			case VEC2:
				data_2 = v.data_2;
				break;
			case FLOAT:
				data_1 = v.data_1;
				break;
			case MATRIX2x2:
				memcpy(data_matrix2x2, v.data_matrix2x2, sizeof(data_matrix2x2));
				break;
			case MATRIX3x3:
				memcpy(data_matrix3x3, v.data_matrix3x3, sizeof(data_matrix3x3));
				break;
			case MATRIX4x4:
				memcpy(data_matrix4x4, v.data_matrix4x4, sizeof(data_matrix4x4));
				break;
			case INT:
				data_int = v.data_int;
				break;
			case COLOR3:
				data_color3 = v.data_color3;
				break;
			case COLOR4:
				data_color4 = v.data_color4;
				break;
			case TEXTURE1D:
			case TEXTURE2D:
			case TEXTURE3D:
				data_tex = v.data_tex;
				break;
			case IMAGE1D:
			case IMAGE2D:
			case IMAGE3D:
				data_image = v.data_image;
				break;
			
			default:
				break;
			}
			type = v.type;
			name = v.name;
		}

	};
	struct Project {
		std::wstring MLang_file{};
		std::vector<std::wstring> Shaders{};
		std::vector<std::wstring> ShaderName{};
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
	void (*frameStart) () = nullptr;
	unsigned int (*frameEnd) () = nullptr;
	bool (*frameUpdate) () = nullptr;
	void (*programStart) () = nullptr;
	void (*programEnd) () = nullptr;
	void (*varUpdate) (const wchar_t* name) = nullptr;
private:
	std::vector<var> vars;
	std::vector<int> ComputeShaders;
	std::vector<int> ComputePrograms;
	std::vector<std::wstring> ComputeShadersName;
	x86Runner runner{};
public:
	unsigned int computeShader_workgroup_size = 16;
	Camera camera{};
	int Render_Width = 512;
	int Render_Height = 512;
	double Render_Time = 0;

	int Render_KeyBoard[256];
	int Render_Mouse[3];
	double Render_MousePos[2];


	bool build(std::wstring ProjectFile);
	void enterUpdate();
	void update();
	unsigned int leaveUpdate();
	void create();
	void release();
	void reset();
	void updateVar(const wchar_t* name);
	void info(const wchar_t* info);
	bool Error{};
	std::wstring getShaderWithoutInclude(std::wstring cs, std::unordered_set<std::wstring>& map);
	static unsigned int _stdcall functions(unsigned int this_, unsigned int func);
	static void _stdcall callbacks(unsigned int this_, unsigned int name, unsigned int func);
	static unsigned int _stdcall image1D(unsigned int this_, unsigned int width);
	static unsigned int _stdcall image2D(unsigned int this_, unsigned int width, unsigned int height);
	static unsigned int _stdcall image3D(unsigned int this_, unsigned int width, unsigned int height, unsigned int depth);
	static void _stdcall bindImage(unsigned int this_, unsigned int img, unsigned int binding);
	static void _stdcall bindTex(unsigned int this_, unsigned int tex , unsigned int bind, unsigned int dim);
	static void _stdcall releaseImage(unsigned int this_, unsigned int img);
	static void _stdcall uniform1f(unsigned int this_, unsigned int program, unsigned int name, double value);
	static void _stdcall uniform1i(unsigned int this_, unsigned int program, unsigned int name, int value);
	static void _stdcall uniform2f(unsigned int this_, unsigned int program, unsigned int name, double x, double y);
	static void _stdcall uniform3f(unsigned int this_, unsigned int program, unsigned int name, double x, double y, double z);
	static void _stdcall uniform4f(unsigned int this_, unsigned int program, unsigned int name, double x, double y, double z, double w);
	static void _stdcall uniformMatrix2x2f(unsigned int this_, unsigned int program, unsigned int name, double* value);
	static void _stdcall uniformMatrix3x3f(unsigned int this_, unsigned int program, unsigned int name, double* value);
	static void _stdcall uniformMatrix4x4f(unsigned int this_, unsigned int program, unsigned int name, double* value);
	static void _stdcall compute(unsigned int this_, unsigned int program, unsigned int width, unsigned int height, unsigned int depth, unsigned int g_w, unsigned int g_h, unsigned int g_d);
	static void _stdcall bindProgram(unsigned int this_, unsigned int program);
	static void _stdcall getUniform1f(unsigned int this_, unsigned int name, double* value);
	static int _stdcall getUniform1i(unsigned int this_, unsigned int name);
	static void _stdcall getUniformArrayI(unsigned int this_, unsigned int name, int* Array);
	static void _stdcall getUniform2f(unsigned int this_, unsigned int name, double* value);
	static void _stdcall getUniform3f(unsigned int this_, unsigned int name, double* value);
	static void _stdcall getUniform4f(unsigned int this_, unsigned int name, double* value);
	static void _stdcall getUniformMatrix2x2f(unsigned int this_, unsigned int name, double* value);
	static void _stdcall getUniformMatrix3x3f(unsigned int this_, unsigned int name, double* value);
	static void _stdcall getUniformMatrix4x4f(unsigned int this_, unsigned int name, double* value);
	static unsigned int _stdcall getShader(unsigned int this_, unsigned int name);
};