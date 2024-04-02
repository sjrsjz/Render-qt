#pragma once
#include <vector>
#include <unordered_map>
#include <Windows.h>
#include <string>

#include <QtWidgets/qopenglwidget.h>
#include <QtGui/qopenglfunctions_3_3_compatibility.h>
#include <QtGui/qopenglextrafunctions.h>
#include <QtWidgets/qapplication.h>
#include <gl/GLU.h>

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
	GLCharacterList() {}
	~GLCharacterList() {
		for (auto& x : list) {
			glDeleteLists(x.second, 1);
		}
		list.clear();
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