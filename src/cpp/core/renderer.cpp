#include "../../header/core/renderer.h"

bool texture::loadBmp2D(std::wstring file)
{
// Load the bitmap
	HBITMAP hBmp = (HBITMAP)LoadImage(NULL, file.c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
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

	// Release the bitmap
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
	_ProjectDir = ProjectFile.substr(0, ProjectFile.find_last_of(L"\\"));
	_ProjectFile = ProjectFile.substr(ProjectFile.find_last_of(L"\\") + 1);
	info(L"读取工程文件中...");
	//Load the Project File
	std::wifstream Project(ProjectFile);
	if (!Project.is_open()) {
		info(L"工程文件打开失败");
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
		return false;
	}
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
		return false;
	}
	info(L"读取工程文件成功");
	info(L"编译Shader中...");
	//Compile the Shader
	ComputeShaders.clear();
	ComputePrograms.clear();
	for (auto& x : project.Shaders) {
		std::unordered_set<std::wstring> map{};
		x = subreplace(getShaderWithoutInclude(x, map), L"####", std::to_wstring(computeShader_workgroup_size));
		ComputeShaders.push_back(0);
		char errbuf[2048];
		ComputePrograms.push_back(shader.CompileComputeShader(MLang::to_byte_string(x).c_str(), (GLuint*)&ComputeShaders.back(), errbuf));
		if (ComputePrograms.back() < 0) {
			info((L"Shader编译失败:\n" + MLang::to_wide_string((std::string)errbuf)).c_str());
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
	return false;
Success:
	info(L"编译MLang程序成功");
	info(L"初始化变量中...");
	//Load the Var
	std::wifstream VarFile(project.Var_file);
	if (!VarFile.is_open()) {
		info(L"变量文件打开失败");
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

		if (type == L"float") {
			if (tokens.size() < 3) {
				info((L"变量 " + name + L" 参数过少").c_str());
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
				return false;
			}
			std::copy(std::begin(v.data_matrix2x2), std::end(v.data_matrix2x2), std::begin(v.default_matrix2x2));
			vars.push_back(v);
		}

		else if (type == L"mat3x3") {
			if (tokens.size() < 10) {
				info((L"变量 " + name + L" 参数过少").c_str());
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
				return false;
			}
			std::copy(std::begin(v.data_matrix3x3), std::end(v.data_matrix3x3), std::begin(v.default_matrix3x3));
			vars.push_back(v);
		}
		else if (type == L"mat4x4") {
			if (tokens.size() < 18) {
				info((L"变量 " + name + L" 参数过少").c_str());
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
				return false;
			}
			std::copy(std::begin(v.data_matrix4x4), std::end(v.data_matrix4x4), std::begin(v.default_matrix4x4));
			vars.push_back(v);
		}
		else if (type == L"color") {
			if (tokens.size() < 5) {
				info((L"变量 " + name + L" 参数过少").c_str());
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


unsigned int RenderSystem::functions(unsigned int this_, const wchar_t* func) {
	RenderSystem* _this = (RenderSystem*)this_;
	return 0;
}

unsigned int RenderSystem::callbacks(unsigned int this_, const wchar_t* name, const wchar_t* func) {
	RenderSystem* _this = (RenderSystem*)this_;
	return 0;
}
