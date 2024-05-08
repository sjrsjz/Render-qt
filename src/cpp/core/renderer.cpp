#include "../../header/core/renderer.h"

bool texture::loadBmp2D(std::wstring file)
{
// Load the bitmap
	HBITMAP hBmp = (HBITMAP)LoadImage(NULL, file.c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
	if (hBmp == NULL)
	{
		return false;
	}

	// Get the bitmap info
	BITMAP bmp;
	GetObject(hBmp, sizeof(BITMAP), &bmp);

	GLuint m_texture;
	// Create the texture
	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);

	// Set the texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Load the texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bmp.bmWidth, bmp.bmHeight, 0, GL_BGR, GL_UNSIGNED_BYTE, bmp.bmBits);
	glBindTexture(GL_TEXTURE_2D, 0);

	width = bmp.bmWidth;
	height = bmp.bmHeight;
	depth = 0;
	dim = texture::DIM::_2D;
	id = m_texture;

	// 删除原始位图
	DeleteObject(hBmp);

	return true;
}
void RenderSystem::info(const wchar_t* info) {
	if (!info_callback) {
		std::cout << "[Render System Info]" << info << std::endl;
		return;
	}
	info_callback(info);
}

std::wstring loadFileString(std::wstring file) {
	std::wifstream ifs(file);
	if (!ifs.is_open()) {
		return L"";
	}
	std::wstring str((std::istreambuf_iterator<wchar_t>(ifs)), std::istreambuf_iterator<wchar_t>());
	return str;
}

bool RenderSystem::build(std::wstring ProjectFile) {
	release();
	this->Error = false;
	//get Current Directory
	std::wstring curr_dir;
	curr_dir.resize(32767);
	GetCurrentDirectory(32767, &curr_dir[0]);
	//Get the Project Directory
	_ProjectDir = ProjectFile.substr(0, ProjectFile.find_last_of(L"\\"));
	//get full path
	std::wstring tmp;
	tmp.resize(32767);
	GetFullPathName(_ProjectDir.c_str(), 32767, &tmp[0], NULL);
	_ProjectDir = tmp;
	_ProjectDir = (std::wstring)_ProjectDir.c_str();
	//cd to the Project Directory
	SetCurrentDirectory(_ProjectDir.c_str());
	_ProjectFile = _ProjectDir + L"\\" + ProjectFile.substr(ProjectFile.find_last_of(L"\\") + 1);

	info(L"读取工程文件中... ");
	info((L"工程路径: " + _ProjectDir).c_str());
	info((L"工程文件: " + _ProjectFile).c_str());
	//Load the Project File
	std::wifstream Project(_ProjectFile);
	if (!Project.is_open()) {
		info(L"工程文件打开失败");
		this->Error = true;
		return false;
	}
	std::wstring text;
	text = std::wstring((std::istreambuf_iterator<wchar_t>(Project)), std::istreambuf_iterator<wchar_t>());
	std::vector<std::wstring> lines=split(text, L"\n");
	for (size_t i = 0; i < lines.size(); i++) {
		if (lines[i] == L"")
		{
			lines.erase(lines.begin() + i);
			i--;
		}
	}
	if (lines.size() % 3) {
		info(L"工程文件格式错误");
		this->Error = true;
		return false;
	}
	project.MLang_file = L"";
	project.Var_file = L"";
	project.Shaders.clear();
	project.ShaderName.clear();
	for (size_t i{}; i < lines.size(); i += 3) {
		std::wstring name = lines[i];
		std::wstring type = lines[i + 1];
		std::wstring file = lines[i + 2];
		std::wstring fullpath;
		if (file.find(L":") != std::wstring::npos) {
			fullpath = file;
		}
		else {
			fullpath = _ProjectDir + L"\\" + file;
		}
		if (type == L"gc") {
			std::wstring cs = loadFileString(fullpath);
			project.Shaders.push_back(cs);
			project.ShaderName.push_back(name);
		}
		else if (type == L"ml" && name == L"main") {
			project.MLang_file = fullpath;
		}
		else if (type == L"var") {
			project.Var_file = fullpath;
		}
	}
	if (project.MLang_file.empty() || project.Var_file.empty() || project.Shaders.empty()) {
		info(L"工程文件格式错误");
		this->Error = true;
		return false;
	}
	info(L"读取工程文件成功");
	info(L"编译Shader中...");
	//Compile the Shader
	ComputeShaders.clear();
	ComputePrograms.clear();
	ComputeShadersName.clear();
	for (auto & x : project.Shaders) {
		std::unordered_set<std::wstring> map{};
		x = subreplace(getShaderWithoutInclude(x, map), L"####", std::to_wstring(computeShader_workgroup_size));
		ComputeShaders.push_back(0);
		char errbuf[2048];
		ComputePrograms.push_back(shader.CompileComputeShader(MLang::to_byte_string(x).c_str(), (GLuint*)&ComputeShaders.back(), errbuf));
		std::wstring name = project.ShaderName[ComputeShadersName.size()];
		ComputeShadersName.push_back(name);
		if (ComputePrograms.back() < 0) {
			info((L"Shader编译失败:" + name + L"\n" + MLang::to_wide_string((std::string)errbuf)).c_str());
			this->Error = true;
			return false;
		}
	}
	info(L"编译Shader成功");
	info(L"编译MLang程序中...");
	Lexer lexer;
	
	workPath = _ProjectDir + R("\\");
	Lexer lex{};
	lstring src = readFileString(project.MLang_file);
	src = 
		L"Class:_RenderSystem_{"
		"	Public N:_this_;" 
		"	Public N:functions;" 
		"	Public N:callbacks;"
		"	_init_()->N:={"
		"		functions=" + std::to_wstring((unsigned int)&functions) + L";"
		"		callbacks=" + std::to_wstring((unsigned int)&callbacks) + L";"
		"		_this_=" + std::to_wstring((unsigned int)this) + L";"
		"	}"
		"	Transit \"\" _getFunction(N:this_,N:name)->N:={return(functions)}"
		"	Transit \"\" _setCallback(N:this_,N:name,N:func)->N:={return(callbacks)}"
		"	Public getFunction(N:name)->N:={return(_getFunction(_this_,name))}"
		"	Public setCallback(N:name,N:func)->N:={return(_setCallback(_this_,name,func))}"
		"}\n"
		+ src;
	bool err = lex.analyze(src);
	AST ast{};
	IRGenerator ir{};
	ByteArray<unsigned char> mexe;
	if (err) goto Error;
	PrepareLexer(lex);
	err = ast.analyze(lex.importedLibs, lex.globals, lex.functionSets, lex.structures, lex.ExternFunctions, lex.constants);
	if (err) goto Error;
	err = ir.analyze(ast.libs, ast.globalVars, ast.analyzed_functionSets, ast.sets, ast.structures, ast.ExtraFunctions, ast.constants);
	if (err) goto Error;
	err = !IR2MEXE(ir.IR, mexe);
	if (err) goto Error;
	err = !runner.LoadMEXE(mexe);
	if (err) goto Error;
	goto Success;
Error:
	info((L"编译MLang程序失败\n" + MLangError).c_str());
	this->Error = true;
	return false;
Success:
	info(L"编译MLang程序成功");
	info(L"初始化变量中...");
	//Load the Var
	std::wifstream VarFile(project.Var_file);
	if (!VarFile.is_open()) {
		info(L"变量文件打开失败");
		this->Error = true;
		return false;
	}
	text = std::wstring((std::istreambuf_iterator<wchar_t>(VarFile)), std::istreambuf_iterator<wchar_t>());
	lines = split(text, L"\n");
	for (auto& x : lines) {
		std::vector<std::wstring> tokens;
		cut_tokens(x, tokens);
		if (tokens.size() < 2) continue;
		std::wstring type = tokens[0];
		std::wstring name = tokens[1];
		if (type == L";") {
			continue;
		}
		if (type == L"float") {
			if (tokens.size() < 3) {
				info((L"变量 " + name + L" 参数过少").c_str());
				this->Error = true;
				return false;
			}
			var v{};
			v.type = VarType::FLOAT;
			v.name = name;
			v.data_1 = std::stof(tokens[2]);
			v.default_1 = v.data_1;
			vars.push_back(v);
		}

		else if (type == L"vec2") {
			if (tokens.size() < 4) {
				info((L"变量 " + name + L" 参数过少").c_str());
				this->Error = true;
				return false;
			}
			var v{};
			v.type = VarType::VEC2;
			v.name = name;
			v.data_2 = { std::stof(tokens[2]),std::stof(tokens[3]) };
			v.default_2 = v.data_2;
			vars.push_back(v);
		}

		else if (type == L"vec3") {
			if (tokens.size() < 5) {
				info((L"变量 " + name + L" 参数过少").c_str());
				this->Error = true;
				return false;
			}
			var v{};
			v.type = VarType::VEC3;
			v.name = name;
			v.data_3 = { std::stof(tokens[2]),std::stof(tokens[3]),std::stof(tokens[4]) };
			v.default_3 = v.data_3;
			vars.push_back(v);
		}

		else if (type == L"vec4") {
			if (tokens.size() < 6) {
				info((L"变量 " + name + L" 参数过少").c_str());
				this->Error = true;
				return false;
			}
			var v{};
			v.type = VarType::VEC4;
			v.name = name;
			v.data_4 = { std::stof(tokens[2]),std::stof(tokens[3]),std::stof(tokens[4]),std::stof(tokens[5]) };
			v.default_4 = v.data_4;
			vars.push_back(v);
		}

		else if (type == L"int") {
			if (tokens.size() < 3) {
				info((L"变量 " + name + L" 参数过少").c_str());
				this->Error = true;
				return false;
			}
			var v{};
			v.type = VarType::INT;
			v.name = name;
			v.data_int = std::stoi(tokens[2]);
			v.default_int = v.data_int;
			vars.push_back(v);
		}

		else if (type == L"mat2x2") {
			if (tokens.size() < 6) {
				info((L"变量 " + name + L" 参数过少").c_str());
				this->Error = true;
				return false;
			}
			var v{};
			v.type = VarType::MATRIX2x2;
			v.name = name;
			try {
				for (size_t i = 0; i < 4; i++) {
					v.data_matrix2x2[i] = std::stof(tokens[i + 2]);
					v.default_matrix2x2[i] = v.data_matrix2x2[i];
				}
			}
			catch (...) {
				info((L"变量 " + name + L" 参数错误").c_str());
				this->Error = true;
				return false;
			}
			std::copy(std::begin(v.data_matrix2x2), std::end(v.data_matrix2x2), std::begin(v.default_matrix2x2));
			vars.push_back(v);
		}

		else if (type == L"mat3x3") {
			if (tokens.size() < 10) {
				info((L"变量 " + name + L" 参数过少").c_str());
				this->Error = true;
				return false;
			}
			var v{};
			v.type = VarType::MATRIX3x3;
			v.name = name;
			try {
				for (size_t i = 0; i < 9; i++) {
					v.data_matrix3x3[i] = std::stof(tokens[i + 2]);
					v.default_matrix3x3[i] = v.data_matrix3x3[i];
				}
			}
			catch (...) {
				info((L"变量 " + name + L" 参数错误").c_str());
				this->Error = true;
				return false;
			}
			std::copy(std::begin(v.data_matrix3x3), std::end(v.data_matrix3x3), std::begin(v.default_matrix3x3));
			vars.push_back(v);
		}
		else if (type == L"mat4x4") {
			if (tokens.size() < 18) {
				info((L"变量 " + name + L" 参数过少").c_str());
				this->Error = true;
				return false;
			}
			var v{};
			v.type = VarType::MATRIX4x4;
			v.name = name;
			try {
				for (size_t i = 0; i < 16; i++) {
					v.data_matrix4x4[i] = std::stof(tokens[i + 2]);
					v.default_matrix4x4[i] = v.data_matrix4x4[i];
				}
			}
			catch (...) {
				info((L"变量 " + name + L" 参数错误").c_str());
				this->Error = true;
				return false;
			}
			std::copy(std::begin(v.data_matrix4x4), std::end(v.data_matrix4x4), std::begin(v.default_matrix4x4));
			vars.push_back(v);
		}
		else if (type == L"color") {
			if (tokens.size() < 5) {
				info((L"变量 " + name + L" 参数过少").c_str());
				this->Error = true;
				return false;
			}
			var v{};
			v.type = VarType::COLOR3;
			v.name = name;
			v.data_color3 = { std::stof(tokens[2]),std::stof(tokens[3]),std::stof(tokens[4]) };
			v.default_color3 = v.data_color3;
			vars.push_back(v);
		}
		else if (type == L"color4") {
			if (tokens.size() < 6) {
				info((L"变量 " + name + L" 参数过少").c_str());
				this->Error = true;
				return false;
			}
			var v{};
			v.type = VarType::COLOR4;
			v.name = name;
			v.data_color4 = { std::stof(tokens[2]),std::stof(tokens[3]),std::stof(tokens[4]),std::stof(tokens[5]) };
			v.default_color4 = v.data_color4;
			vars.push_back(v);
		}
		else if (type == L"tex") {
			if (tokens.size() < 3) {
				info((L"变量 " + name + L" 参数过少").c_str());
				this->Error = true;
				return false;
			}
			texture t;
			t.loadBmp2D(process_quotation_mark(tokens[2]));
			var v{};
			v.type = VarType::TEXTURE2D;
			v.name = name;
			v.data_tex = t;
			v.default_tex = t;
			vars.push_back(v);		
		}
		else if (type == L"buffer1D") {
			if (tokens.size() < 4) {
				info((L"变量 " + name + L" 参数过少").c_str());
				this->Error = true;
				return false;
			}
			GLImage img(&shader);
			img.setBinding(std::stoul(tokens[2]));
			img.create1D(std::stoul(tokens[3]));
			var v{};
			v.type = VarType::IMAGE1D;
			v.name = name;
			v.data_image = img;
			v.default_image = img;
			vars.push_back(v);
		}
		else if (type == L"buffer2D") {
			if (tokens.size() < 5) {
				info((L"变量 " + name + L" 参数过少").c_str());
				this->Error = true;
				return false;
			}
			GLImage img(&shader);
			img.setBinding(std::stoul(tokens[2]));
			img.create2D(std::stoul(tokens[3]), std::stoul(tokens[4]));
			var v{};
			v.type = VarType::IMAGE2D;
			v.name = name;
			v.data_image = img;
			v.default_image = img;
			vars.push_back(v);
		}
		else if (type == L"buffer3D") {
			if (tokens.size() < 6) {
				info((L"变量 " + name + L" 参数过少").c_str());
				this->Error = true;
				return false;
			}
			GLImage img(&shader);
			img.setBinding(std::stoul(tokens[2]));
			img.create3D(std::stoul(tokens[3]), std::stoul(tokens[4]), std::stoul(tokens[5]));
			var v{};
			v.type = VarType::IMAGE3D;
			v.name = name;
			v.data_image = img;
			v.default_image = img;
			vars.push_back(v);
		}
		else {
			info((L"不支持的变量类型:" + type).c_str());
			this->Error = true;
			return false;
		}
	}
	info(L"初始化变量成功");
	return true;
}


std::wstring RenderSystem::getShaderWithoutInclude(std::wstring cs, std::unordered_set<std::wstring>& map) {
	std::vector<std::wstring> lines = split(cs, L"\n");
	std::wstring result;
	
	for (auto& x : lines) {
		std::vector<std::wstring> tokens;
		cut_tokens(x, tokens);
		if (tokens.size() == 2 && tokens[0] == L"#include") {
			std::wstring file = process_quotation_mark(tokens[1]);
			std::wstring fullpath;
			if (file.find(L":") != std::wstring::npos) {
				fullpath = file;
			}
			else {
				fullpath = _ProjectDir + L"\\" + file;
			}
			if (map.find(fullpath) != map.end()) {
				continue;	
			}
			map.insert(fullpath);
			std::wstring cs = loadFileString(fullpath);
			result += getShaderWithoutInclude(cs, map);
		}
		else {
			result += x + L"\n";
		}

	}
	DebugOutput(result);
	return result;
}


unsigned int _stdcall RenderSystem::functions(unsigned int this_, unsigned int func_) {
	RenderSystem* _this = (RenderSystem*)this_;
	std::wstring func = (std::wstring)(wchar_t*)func_;
	if (func == L"image1D") return (unsigned int)RenderSystem::image1D;
	if (func == L"image2D") return (unsigned int)RenderSystem::image2D;
	if (func == L"image3D") return (unsigned int)RenderSystem::image3D;
	if (func == L"bindImage") return (unsigned int)RenderSystem::bindImage;
	if (func == L"bindTex") return (unsigned int)RenderSystem::bindTex;
	if (func == L"uniform1f") return (unsigned int)RenderSystem::uniform1f;
	if (func == L"uniform1i") return (unsigned int)RenderSystem::uniform1i;
	if (func == L"uniform2f") return (unsigned int)RenderSystem::uniform2f;
	if (func == L"uniform3f") return (unsigned int)RenderSystem::uniform3f;
	if (func == L"uniform4f") return (unsigned int)RenderSystem::uniform4f;
	if (func == L"uniformMatrix2x2f") return (unsigned int)RenderSystem::uniformMatrix2x2f;
	if (func == L"uniformMatrix3x3f") return (unsigned int)RenderSystem::uniformMatrix3x3f;
	if (func == L"uniformMatrix4x4f") return (unsigned int)RenderSystem::uniformMatrix4x4f;
	if (func == L"compute") return (unsigned int)RenderSystem::compute;
	if (func == L"bindProgram") return (unsigned int)RenderSystem::bindProgram;
	if (func == L"getUniform1f") return (unsigned int)RenderSystem::getUniform1f;
	if (func == L"getUniform2f") return (unsigned int)RenderSystem::getUniform2f;
	if (func == L"getUniform3f") return (unsigned int)RenderSystem::getUniform3f;
	if (func == L"getUniform4f") return (unsigned int)RenderSystem::getUniform4f;
	if (func == L"getUniform1i") return (unsigned int)RenderSystem::getUniform1i;
	if (func == L"getUniformMatrix2x2f") return (unsigned int)RenderSystem::getUniformMatrix2x2f;
	if (func == L"getUniformMatrix3x3f") return (unsigned int)RenderSystem::getUniformMatrix3x3f;
	if (func == L"getUniformMatrix4x4f") return (unsigned int)RenderSystem::getUniformMatrix4x4f;
	if (func == L"getShader") return (unsigned int)RenderSystem::getShader;
	if (func == L"getCmdLineArg") return (unsigned int)RenderSystem::getCmdLineArg;
	if (func == L"getImage") return (unsigned int)RenderSystem::getImage;
	if (func == L"getTex") return (unsigned int)RenderSystem::getTex;
	throw L"未知函数" + func;
	return 0;
}

void _stdcall RenderSystem::callbacks(unsigned int this_, unsigned int name, unsigned int func) {
	RenderSystem* _this = (RenderSystem*)this_;
	if ((std::wstring)(wchar_t*)name == L"frame_update") {
		_this->frameUpdate = (bool(*)())func;
	}
	else if ((std::wstring)(wchar_t*)name == L"frame_start") {
		_this->frameStart = (void(*)())func;
	}
	else if ((std::wstring)(wchar_t*)name == L"frame_end") {
		_this->frameEnd = (unsigned int (*)())func;
	}
	else if ((std::wstring)(wchar_t*)name == L"program_start") {
		_this->programStart = (void(*)())func;
	}
	else if ((std::wstring)(wchar_t*)name == L"program_end") {
		_this->programEnd = (void(*)())func;
	}
	else if ((std::wstring)(wchar_t*)name == L"var_update") {
		_this->varUpdate = (void(*)(const wchar_t*))func;
	}
}


unsigned int _stdcall RenderSystem::image1D(unsigned int this_, unsigned int width) {
	return ((RenderSystem*)this_)->shader.Create1DImageTex32F(width);
}
unsigned int _stdcall RenderSystem::image2D(unsigned int this_, unsigned int width, unsigned int height) {
	return ((RenderSystem*)this_)->shader.Create2DImageTex32F(width, height);
}
unsigned int _stdcall RenderSystem::image3D(unsigned int this_, unsigned int width, unsigned int height, unsigned int depth) {
	return ((RenderSystem*)this_)->shader.Create3DImageTex32F(width, height, depth);
}
void _stdcall RenderSystem::bindImage(unsigned int this_, unsigned int img, unsigned int binding) {
	((RenderSystem*)this_)->shader.ex.glBindImageTexture(binding, img, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
}
void _stdcall RenderSystem::bindTex(unsigned int this_, unsigned int tex, unsigned int bind, unsigned int dim) {
	((RenderSystem*)this_)->shader.ex.glActiveTexture(GL_TEXTURE0 + bind);
	switch (dim)
	{
	case texture::DIM::_1D:
		glBindTexture(GL_TEXTURE_1D, tex);
		break;
	case texture::DIM::_2D:
		glBindTexture(GL_TEXTURE_2D, tex);
		break;
	case texture::DIM::_3D:
		glBindTexture(GL_TEXTURE_3D, tex);
		break;
	default:
		break;
	}
}
void _stdcall RenderSystem::uniform1f(unsigned int this_, unsigned int program, unsigned int name, double value) {
	((RenderSystem*)this_)->shader.ex.glUniform1f(((RenderSystem*)this_)->shader.ex.glGetUniformLocation(program, to_byte_string((std::wstring)(wchar_t*)name).c_str()), value);
}
void _stdcall RenderSystem::uniform2f(unsigned int this_, unsigned int program, unsigned int name, double x, double y) {
	((RenderSystem*)this_)->shader.ex.glUniform2f(((RenderSystem*)this_)->shader.ex.glGetUniformLocation(program, to_byte_string((std::wstring)(wchar_t*)name).c_str()), x, y);
}
void _stdcall RenderSystem::uniform3f(unsigned int this_, unsigned int program, unsigned int name, double x, double y, double z) {
	((RenderSystem*)this_)->shader.ex.glUniform3f(((RenderSystem*)this_)->shader.ex.glGetUniformLocation(program, to_byte_string((std::wstring)(wchar_t*)name).c_str()), x, y, z);
}
void _stdcall RenderSystem::uniform4f(unsigned int this_, unsigned int program, unsigned int name, double x, double y, double z, double w) {
	((RenderSystem*)this_)->shader.ex.glUniform4f(((RenderSystem*)this_)->shader.ex.glGetUniformLocation(program, to_byte_string((std::wstring)(wchar_t*)name).c_str()), x, y, z, w);
}
void _stdcall RenderSystem::uniform1i(unsigned int this_, unsigned int program, unsigned int name, int value) {
	((RenderSystem*)this_)->shader.ex.glUniform1i(((RenderSystem*)this_)->shader.ex.glGetUniformLocation(program, to_byte_string((std::wstring)(wchar_t*)name).c_str()), value);
}
void _stdcall RenderSystem::uniformMatrix2x2f(unsigned int this_, unsigned int program, unsigned int name, double* value) {
	float v[4];
	for (size_t i = 0; i < 4; i++) {
		v[i] = (float)value[i];
	}
	((RenderSystem*)this_)->shader.ex.glUniformMatrix2fv(((RenderSystem*)this_)->shader.ex.glGetUniformLocation(program, to_byte_string((std::wstring)(wchar_t*)name).c_str()), 1, GL_FALSE, v);
}
void _stdcall RenderSystem::uniformMatrix3x3f(unsigned int this_, unsigned int program, unsigned int name, double* value) {
	float v[9];
	for (size_t i = 0; i < 9; i++) {
		v[i] = (float)value[i];
	}
	((RenderSystem*)this_)->shader.ex.glUniformMatrix3fv(((RenderSystem*)this_)->shader.ex.glGetUniformLocation(program, to_byte_string((std::wstring)(wchar_t*)name).c_str()), 1, GL_FALSE, v);
}
void _stdcall RenderSystem::uniformMatrix4x4f(unsigned int this_, unsigned int program, unsigned int name, double* value) {
	float v[16];
	for (size_t i = 0; i < 16; i++) {
		v[i] = (float)value[i];
	}
	((RenderSystem*)this_)->shader.ex.glUniformMatrix4fv(((RenderSystem*)this_)->shader.ex.glGetUniformLocation(program, to_byte_string((std::wstring)(wchar_t*)name).c_str()), 1, GL_FALSE, v);
}
void _stdcall RenderSystem::compute(unsigned int this_, unsigned int program, unsigned int width, unsigned int height, unsigned int depth, unsigned int g_w, unsigned int g_h, unsigned int g_d) {
	((RenderSystem*)this_)->shader.ex.glDispatchCompute(width / g_w + (unsigned int)(width%g_w != 0), height / g_h + (unsigned int)(height % g_h != 0), depth / g_d + (unsigned int)(depth % g_d != 0));
	((RenderSystem*)this_)->shader.ex.glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}
void _stdcall RenderSystem::bindProgram(unsigned int this_, unsigned int program) {
	((RenderSystem*)this_)->shader.ex.glUseProgram(program);
}


void RenderSystem::enterUpdate() {
	_set_se_translator(sehTranslator);
	if (frameStart && !Error) {
		try {
			frameStart();
		}
		catch (const SEHException& e) {
			info((L"帧起始函数异常" + e.getExceptionString()).c_str());
			Error = true;
		}
	}
}
void RenderSystem::update(double dtime) {
	_set_se_translator(sehTranslator);
	Render_Time += dtime;
	Render_dTime = dtime;
	if (frameUpdate && !Error) {
		try {
			while(!frameUpdate() && !Error);
		}
		catch (const SEHException& e) {
			info((L"帧更新函数异常" + e.getExceptionString()).c_str());
			info(runner.getTraceBackInfo().c_str());
			Error = true;
		}
	}

}
unsigned int RenderSystem::leaveUpdate() {
	_set_se_translator(sehTranslator);
	if (frameEnd && !Error) {
		try {
			return frameEnd();
		}
		catch (const SEHException& e) {
			info((L"帧结束函数异常\n" + e.getExceptionString()).c_str());
			info(runner.getTraceBackInfo().c_str());
			Error = true;
		}
	}
	return 0;
}
void RenderSystem::create() {
	Render_Time = 0;
	_set_se_translator(sehTranslator);
	if(Error) return;
	try {
		runner.run();
	}
	catch (const SEHException& e) {
		info((L"程序初始化异常\n" + e.getExceptionString()).c_str());
		info(runner.getTraceBackInfo().c_str());
		Error = true;
		return;
	}
	if (programStart && !Error) {
		try {
			
			programStart();
		}
		catch (const SEHException& e) {
			info((L"程序起始函数异常\n" + e.getExceptionString()).c_str());
			info(runner.getTraceBackInfo().c_str());
			Error = true;
		}
	}

}
void RenderSystem::release() {
	_set_se_translator(sehTranslator);
	if (programEnd) {
		try {
			programEnd();
		}
		catch (const SEHException& e) {
			info((L"程序结束函数异常\n" + e.getExceptionString()).c_str());
			info(runner.getTraceBackInfo().c_str());
			Error = true;
		}
	}
	programStart = nullptr;
	programEnd = nullptr;
	frameStart = nullptr;
	frameEnd = nullptr;
	frameUpdate = nullptr;
	varUpdate = nullptr;
}
void RenderSystem::reset() {}
void RenderSystem::updateVar(const wchar_t* name) {
	_set_se_translator(sehTranslator);
	if (varUpdate && !Error) {
		try {
			varUpdate(name);
		}
		catch (const SEHException& e) {
			info((L"变量更新函数异常\n" + e.getExceptionString()).c_str());
			info(runner.getTraceBackInfo().c_str());
			Error = true;
		}
	}
}

void _stdcall RenderSystem::getUniform1f(unsigned int this_, unsigned int name, double* value) {
	if ((std::wstring)(wchar_t*)name == R("System.Renderer.Time")) {
		*value = ((RenderSystem*)this_)->Render_Time;
		return;
	}
	if ((std::wstring)(wchar_t*)name == R("System.Renderer.dTime")) {
		*value = ((RenderSystem*)this_)->Render_dTime;
		return;
	}
	if ((std::wstring)(wchar_t*)name == R("System.Renderer.Scale")) {
		*value = ((RenderSystem*)this_)->Render_SmoothScale.X();
		return;
	}
	for (auto& x : ((RenderSystem*)this_)->vars) {
		if (x.name == (std::wstring)(wchar_t*)name) {
			*value = x.data_1;
			return;
		}
	}
	((RenderSystem*)this_)->info((L"未知变量:" + (std::wstring)(wchar_t*)name).c_str());
	((RenderSystem*)this_)->Error = true;
}
void _stdcall RenderSystem::getUniform2f(unsigned int this_, unsigned int name, double* value) {
	for (auto& x : ((RenderSystem*)this_)->vars) {
		if (x.name == (std::wstring)(wchar_t*)name) {
			value[0] = x.data_2.x;
			value[1] = x.data_2.y;
			return;
		}
	}
	((RenderSystem*)this_)->info((L"未知变量:" + (std::wstring)(wchar_t*)name).c_str());
	((RenderSystem*)this_)->Error = true;
}
void _stdcall RenderSystem::getUniform3f(unsigned int this_, unsigned int name, double* value) {
	for (auto& x : ((RenderSystem*)this_)->vars) {
		if (x.name == (std::wstring)(wchar_t*)name) {
			value[0] = x.data_3.x;
			value[1] = x.data_3.y;
			value[2] = x.data_3.z;
			return;
		}
	}
	((RenderSystem*)this_)->info((L"未知变量:" + (std::wstring)(wchar_t*)name).c_str());
	((RenderSystem*)this_)->Error = true;
}
void _stdcall RenderSystem::getUniform4f(unsigned int this_, unsigned int name, double* value) {
	if ((std::wstring)(wchar_t*)name == R("System.Renderer.Camera.YPR")) {
		value[0] = ((RenderSystem*)this_)->camera.yaw;
		value[1] = ((RenderSystem*)this_)->camera.pitch;
		value[2] = ((RenderSystem*)this_)->camera.roll;
		return;
	
	}
	if ((std::wstring)(wchar_t*)name == R("System.Renderer.Camera.XY2D")) {
		value[0] = ((RenderSystem*)this_)->camera.x_2D;
		value[1] = ((RenderSystem*)this_)->camera.y_2D;
		return;
	}
	if ((std::wstring)(wchar_t*)name == R("System.Renderer.Mouse.Position")) {
		value[0] = ((RenderSystem*)this_)->Render_MousePos[0];
		value[1] = ((RenderSystem*)this_)->Render_MousePos[1];
		value[2] = ((RenderSystem*)this_)->Render_MousePosInt[0];
		value[3] = ((RenderSystem*)this_)->Render_MousePosInt[1];
		return;
	}
	if ((std::wstring)(wchar_t*)name == R("System.Renderer.Mouse.Button"))
	{
		value[0] = ((RenderSystem*)this_)->Render_Mouse[0];
		value[1] = ((RenderSystem*)this_)->Render_Mouse[1];
		value[2] = ((RenderSystem*)this_)->Render_Mouse[2];
		return;
	}
	for (auto& x : ((RenderSystem*)this_)->vars) {
		if (x.name == (std::wstring)(wchar_t*)name) {
			value[0] = x.data_4.x;
			value[1] = x.data_4.y;
			value[2] = x.data_4.z;
			value[3] = x.data_4.w;
			return;
		}
	}
	((RenderSystem*)this_)->info((L"未知变量:" + (std::wstring)(wchar_t*)name).c_str());
	((RenderSystem*)this_)->Error = true;
}
int _stdcall RenderSystem::getUniform1i(unsigned int this_, unsigned int name) {
	if ((std::wstring)(wchar_t*)name == R("System.Renderer.Width")) {
		return ((RenderSystem*)this_)->Render_Width;
	}
	if ((std::wstring)(wchar_t*)name == R("System.Renderer.Height")) {
		return ((RenderSystem*)this_)->Render_Height;
	}
	if ((std::wstring)(wchar_t*)name == R("System.Renderer.WorkGroupSize")) {
		return ((RenderSystem*)this_)->computeShader_workgroup_size;
	}
	for (auto& x : ((RenderSystem*)this_)->vars) {
		if (x.name == (std::wstring)(wchar_t*)name) {
			return x.data_int;
		}
	}
	((RenderSystem*)this_)->info((L"未知变量:" + (std::wstring)(wchar_t*)name).c_str());
	((RenderSystem*)this_)->Error = true;
	return 0;
}
void _stdcall RenderSystem::getUniformMatrix2x2f(unsigned int this_, unsigned int name, double* value) {
	for (auto& x : ((RenderSystem*)this_)->vars) {
		if (x.name == (std::wstring)(wchar_t*)name) {
			std::copy(std::begin(x.data_matrix2x2), std::end(x.data_matrix2x2), value);
			return;
		}
	}
	((RenderSystem*)this_)->info((L"未知变量:" + (std::wstring)(wchar_t*)name).c_str());
	((RenderSystem*)this_)->Error = true;
}
void _stdcall RenderSystem::getUniformMatrix3x3f(unsigned int this_, unsigned int name, double* value) {
	if ((std::wstring)(wchar_t*)name == R("System.Renderer.Camera.Rot")) {
		double v[3][3];
		((RenderSystem*)this_)->camera.getRotationMatrix(&v[0][0]);
		std::copy(&v[0][0], &v[0][0] + 9, value);
		return;
	}
	for (auto& x : ((RenderSystem*)this_)->vars) {
		if (x.name == (std::wstring)(wchar_t*)name) {
			std::copy(std::begin(x.data_matrix3x3), std::end(x.data_matrix3x3), value);
			return;
		}
	}
	((RenderSystem*)this_)->info((L"未知变量:" + (std::wstring)(wchar_t*)name).c_str());
	((RenderSystem*)this_)->Error = true;
}
void _stdcall RenderSystem::getUniformMatrix4x4f(unsigned int this_, unsigned int name, double* value) {
	if ((std::wstring)(wchar_t*)name == R("System.Renderer.Camera.Mat")) {
		double v[4][4];
		((RenderSystem*)this_)->camera.getMatrix(&v[0][0]);
		std::copy(&v[0][0], &v[0][0] + 16, value);
		return;
	}
	for (auto& x : ((RenderSystem*)this_)->vars) {
		if (x.name == (std::wstring)(wchar_t*)name) {
			std::copy(std::begin(x.data_matrix4x4), std::end(x.data_matrix4x4), value);
			return;
		}
	}
	((RenderSystem*)this_)->info((L"未知变量:" + (std::wstring)(wchar_t*)name).c_str());
	((RenderSystem*)this_)->Error = true;
}

void _stdcall getUniformArrayI(unsigned int this_, unsigned int name, int* Array) {
	if ((std::wstring)(wchar_t*)name == R("System.Renderer.KeyBoard")) {
		for (size_t i = 0; i < 256; i++) {
			Array[i] = ((RenderSystem*)this_)->Render_KeyBoard[i];
		}
		return;
	}
	((RenderSystem*)this_)->info((L"未知变量:" + (std::wstring)(wchar_t*)name).c_str());
	((RenderSystem*)this_)->Error = true;
}

unsigned int _stdcall RenderSystem::getShader(unsigned int this_, unsigned int name) {
	for (size_t i = 0; i < ((RenderSystem*)this_)->ComputeShadersName.size(); i++) {
		if (((RenderSystem*)this_)->ComputeShadersName[i] == (std::wstring)(wchar_t*)name) {
			return ((RenderSystem*)this_)->ComputePrograms[i];
		}
	}
	((RenderSystem*)this_)->info((L"未知Shader:" + (std::wstring)(wchar_t*)name).c_str());
	((RenderSystem*)this_)->Error = true;
	return 0;
}
unsigned int _stdcall RenderSystem::getCmdLineArg(unsigned int this_, unsigned int name, unsigned int buffer) {
	std::wstring name_ = L"--" + (std::wstring)(wchar_t*)name;
	try {
		for (size_t i{}; i < ((RenderSystem*)this_)->Render_CmdLineArgs.size(); i++) {
			if (((RenderSystem*)this_)->Render_CmdLineArgs[i] == name_) {
				i++;
				std::wstring arg = ((RenderSystem*)this_)->Render_CmdLineArgs[i];
				//copy to buffer
				std::copy(arg.begin(), arg.end(), (wchar_t*)buffer);
				((wchar_t*)buffer)[arg.size()] = 0;
				return 1;
			}
		}
	}
	catch (...) {};
	((RenderSystem*)this_)->info((L"未知命令行参数:" + (std::wstring)(wchar_t*)name).c_str());
	((RenderSystem*)this_)->Error = true;
	return 0;
}


void _stdcall RenderSystem::getImage(unsigned int this_, unsigned int name, unsigned int* value) {
	for (auto& x : ((RenderSystem*)this_)->vars) {
		if (x.name == (std::wstring)(wchar_t*)name) {
			x.data_image.getParameters(value);
			return;
		}
	}
	((RenderSystem*)this_)->info((L"未知变量:" + (std::wstring)(wchar_t*)name).c_str());
	((RenderSystem*)this_)->Error = true;
}

void _stdcall RenderSystem::getTex(unsigned int this_, unsigned int name, unsigned int* value) {
	for (auto& x : ((RenderSystem*)this_)->vars) {
		if (x.name == (std::wstring)(wchar_t*)name) {
			x.data_tex.getParameters(value);
			return;
		}
	}
	((RenderSystem*)this_)->info((L"未知变量:" + (std::wstring)(wchar_t*)name).c_str());
	((RenderSystem*)this_)->Error = true;
}