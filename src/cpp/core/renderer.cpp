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
		x = getShaderWithoutInclude(x, map);
		ComputeShaders.push_back(0);
		char errbuf[2048];
		ComputePrograms.push_back(shader.CompileComputeShader(MLang::to_byte_string(x).c_str(), (GLuint*)&ComputeShaders.back(), errbuf));
		if (ComputePrograms.back() < 0) {
			info((L"Shader编译失败:\n" + MLang::to_wide_string((std::string)errbuf)).c_str());
			return false;
		}
	}
	info(L"编译Shader成功");

	Lexer lexer;
	
	workPath = _ProjectDir + R("\\");
	Lexer lex{};
	lstring src = readFileString(project.MLang_file);
	src = 
		L"Class:_RenderSystem_{"
		"Public N:_this_;" 
		"Public N:functions;" 
		"_init_()->N:={"
		"functions=" + std::to_wstring((unsigned int)&functions) + L";"
		"_this_=" + std::to_wstring((unsigned int)this) + L";"
		"}}\n"
		+ src;
	bool err = lex.analyze(src);
	if (err) return false;
	PrepareLexer(lex);
	AST ast{};
	err = ast.analyze(lex.importedLibs, lex.globals, lex.functionSets, lex.structures, lex.ExternFunctions, lex.constants);
	if (err) return false;
	IRGenerator ir{};
	err = ir.analyze(ast.libs, ast.globalVars, ast.analyzed_functionSets, ast.sets, ast.structures, ast.ExtraFunctions, ast.constants);
	if (err) return false;
	ByteArray<unsigned char> mexe;
	err = !IR2MEXE(ir.IR, mexe);
	if (err) return false;

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
	return result;
}


unsigned int RenderSystem::functions(unsigned int this_, const wchar_t* func) {
	RenderSystem* _this = (RenderSystem*)this_;
	return 0;
}