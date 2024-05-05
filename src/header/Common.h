#pragma once
#include <vector>
#include <unordered_map>
#include <Windows.h>
#include <string>
#include <iostream>	

#include <QtWidgets/qopenglwidget.h>
#include <QtGui/qopenglfunctions_3_3_compatibility.h>
#include <QtGui/qopenglextrafunctions.h>
#include <QtWidgets/qapplication.h>
#include <gl/GLU.h>
#include <QtOpenGL/qglbuffer.h>
#include <QtOpenGLExtensions/qopenglextensions.h>
#include <QtGui/qopenglfunctions_4_5_compatibility.h>

struct GLRECT
{
	float x{};
	float y{};
	float w{};
	float h{};
};




class GLCharacterList {
public:
	std::unordered_map<int, int> list{};
	std::unordered_map<int, float> width{};
	std::unordered_map<int, float> height{};
	std::unordered_map<int, float> dx{};
	std::unordered_map<int, float> dy{};
	std::unordered_map<int, float> cx{};
	std::unordered_map<int, float> cy{};

	GLCharacterList() {}
	~GLCharacterList() {
		for (auto& x : list) {
			glDeleteLists(x.second, 1);
		}
		list.clear();
		width.clear();
		height.clear();
		dx.clear();
		dy.clear();
		cx.clear();
		cy.clear();
	}
};


#if defined(_MAIN_) and !defined(_Global_)
#define _Global_
GLCharacterList _GLCharacterList{};
HFONT GLhFont{};
#else
extern GLCharacterList _GLCharacterList;
extern HFONT GLhFont;
#endif


