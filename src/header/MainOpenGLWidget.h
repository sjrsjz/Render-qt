#pragma once
#include "Common.h"
#include "GLButton.h"
#include <QtGui/QResizeEvent>
#include <QtGui/qpainter.h>
#include <QtGui/qscreen.h>

#include "shader.h"

// 2.继承相关类
class MainOpenGLWidget : public QOpenGLWidget, QOpenGLFunctions_4_5_Compatibility
{
    Q_OBJECT
public:
    explicit MainOpenGLWidget(QWidget* parent = nullptr);
    Shader shader;
private:
    void DrawBorder();
    void PaintUI();
    void DrawScene();
    void DrawBlurredBackground();

	const char* gaussian_blur_compute_shader = R"(
	#version 430 core
	layout(local_size_x = 16, local_size_y = 16) in;
	layout(binding = 0) uniform sampler2D input_image;
	layout(rgba32f, binding = 0) uniform image2D output_image;
	uniform int width;
	uniform int height;
	uniform int top;
    uniform int left;
    uniform bool WE;

	float sdBox(float x, float a ,float b) {
        float d1 = a-x;
		float d2 = x-b;
		return max(d1,d2);
	}

	void main() {
		ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);
		const int R = 16;
		if (pixel_coords.x > width + left + R || pixel_coords.y > height + top + R || pixel_coords.x < left - R || pixel_coords.y < top - R){
			imageStore(output_image, pixel_coords, texelFetch(input_image, pixel_coords ,0));
			return;
		}
		vec4 color = vec4(0.0);

		float W = 0;
		ivec2 texSize = ivec2(textureSize(input_image,0)) - 1;

		float D=-max(sdBox(pixel_coords.x,left,left+width),sdBox(pixel_coords.y,top,top+height));
		float blurFactor = exp(-0.01*max(D,0));
		for (int i = -R; i < R; i++) {
				ivec2 offset;
				float W0 = exp(-i * i * 0.001);
				if (WE)
					offset = ivec2(i, 0);
				else
					offset = ivec2(0, i);
				ivec2 p = pixel_coords + offset;
				p.x = clamp(p.x, 0, texSize.x);
				p.y = clamp(p.y, 0, texSize.y);
				vec4 sample_ = texelFetch(input_image, p, 0);
				color += sample_ * W0;
				W += W0;
		}
		imageStore(output_image, pixel_coords, mix(vec4(43,43,45,255)/255.0, color / W, blurFactor));
	}
)";

	GLint GaussianBlurProgram = 0;
	GLuint GaussianBlurComputeShader = 0;
	void GaussianBlur(GLuint input_image, GLuint output_image, GLRECT BlurRect, bool WE);
	GLuint DesktopPBO_write = 0;
	GLuint DesktopImageTex = 0;
	GLuint DesktopImageTex_Swap = 0;

	void CaptureScreen( GLuint PBO, int width, int height, GLuint Tex);
	~MainOpenGLWidget();
protected:
    // 3.重载相关虚函数
    virtual void initializeGL();
    virtual void resizeGL(int w, int h);
    virtual void paintGL();

signals:

};
