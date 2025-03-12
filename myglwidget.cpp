#include "myglwidget.h"
#include <GL/glew.h>
#include <algorithm>
#include <windows.h>

MyGLWidget::MyGLWidget(QWidget *parent)
	:QOpenGLWidget(parent)
{
}

MyGLWidget::~MyGLWidget()
{
	delete[] render_buffer;
	delete[] temp_render_buffer;
	delete[] temp_z_buffer;
	delete[] z_buffer;
	delete[] edge_recorder;
}

void MyGLWidget::resizeBuffer(int newW, int newH) {
	delete[] render_buffer;
	delete[] temp_render_buffer;
	delete[] temp_z_buffer;
	delete[] z_buffer;
	delete[] edge_recorder;
	WindowSizeW = newW;
	WindowSizeH = newH;
	render_buffer = new vec3[WindowSizeH*WindowSizeW];
	temp_render_buffer = new vec3[WindowSizeH*WindowSizeW];
	temp_z_buffer = new float[WindowSizeH*WindowSizeW];
	z_buffer = new float[WindowSizeH*WindowSizeW];
	edge_recorder = new vec3[WindowSizeH];
}

void MyGLWidget::initializeGL()
{
	initializeOpenGLFunctions();
	WindowSizeW = width();
	WindowSizeH = height();
	glViewport(0, 0, WindowSizeW, WindowSizeH);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glDisable(GL_DEPTH_TEST);
	offset = vec2(WindowSizeH / 2, WindowSizeW / 2);
	// 对定义的数组初始化
	render_buffer = new vec3[WindowSizeH*WindowSizeW];
	temp_render_buffer = new vec3[WindowSizeH*WindowSizeW];
	temp_z_buffer = new float[WindowSizeH*WindowSizeW];
	z_buffer = new float[WindowSizeH*WindowSizeW];
	edge_recorder = new vec3[WindowSizeH];
	for (int i = 0; i < WindowSizeH*WindowSizeW; i++) {
		render_buffer[i] = vec3(0, 0, 0);
		temp_render_buffer[i] = vec3(0, 0, 0);
		temp_z_buffer[i] = MAX_Z_BUFFER;			
		z_buffer[i] = MAX_Z_BUFFER;
	}
	for (int i = 0; i < WindowSizeH; i++) {
		edge_recorder[i] = vec3(-1, -1, 0);
	}
}

void MyGLWidget::keyPressEvent(QKeyEvent *e) {
	
	switch (e->key()) {
		case Qt::Key_0: scene_id = 0;update(); break;
		case Qt::Key_1: scene_id = 1;update(); break;
		case Qt::Key_9: degree += 35;update(); break;
	}
}

void MyGLWidget::paintGL()
{
	switch (scene_id) {
		case 0:scene_0(); break;
		case 1:scene_1(); break;
	}
}

void MyGLWidget::clearBuffer(vec3* now_buffer) {
	for (int i = 0; i < WindowSizeH*WindowSizeW; i++) {
		now_buffer[i] = vec3(0,0,0);
	}
}

void MyGLWidget::clearBuffer(int* now_buffer) {
	memset(now_buffer, 0, WindowSizeW * WindowSizeH * sizeof(int));
}


void MyGLWidget::clearZBuffer(float* now_buffer) {
	std::fill(now_buffer,now_buffer+WindowSizeW * WindowSizeH, MAX_Z_BUFFER);
}

void MyGLWidget::clearRecorder(vec3* now_buffer) {
	for (int i = 0; i < WindowSizeH; i++) {
		now_buffer[i] = vec3(-1, -1, 0);
	}
}

void MyGLWidget::updateRecorder(int x, int y) {
	int r0 = edge_recorder[y][0];
	int r1 = edge_recorder[y][1];

	//若在高度y上没有起点
	if (r0 == -1) {
		edge_recorder[y][0] = x;
	}
	//若在高度y上已有起点
	else {
		//若在高度y上没有终点
		if (r1 == -1) {
			edge_recorder[y][1] = max(r0, x);
			edge_recorder[y][0] = min(r0, x);
		}
		//若在高度y上已有终点
		else {
			edge_recorder[y][0] = min(min(r0, r1), x);
			edge_recorder[y][1] = max(max(r0, r1), x);
		}
	}
	edge_recorder[y][2] += 1;
}
// 窗口大小变动后，需要重新生成render_buffer等数组
void MyGLWidget::resizeGL(int w, int h)
{
	resizeBuffer(w, h);
	offset = vec2(WindowSizeH / 2, WindowSizeW / 2);
	clearBuffer(render_buffer);
}

// 原始方法渲染
void MyGLWidget::scene_0()
{
	// 选择要加载的model
	objModel.loadModel("./objs/teapot_600.obj");
	//objModel.loadModel("./objs/teapot_8000.obj");
	//objModel.loadModel("./objs/rock.obj");
	//objModel.loadModel("./objs/cube.obj");
	//objModel.loadModel("./objs/singleTriangle.obj");

	DWORD t1, t2;
	t1 = GetTickCount();

	// 自主设置变换矩阵
	camPosition = vec3(100 * sin(degree * 3.14 / 180.0) + objModel.centralPoint.y, 100 * cos(degree * 3.14 / 180.0) + objModel.centralPoint.x, 10 + objModel.centralPoint.z);
	camLookAt = objModel.centralPoint;     // 例如，看向物体中心
	camUp = vec3(0, 1, 0);         // 上方向向量
	projMatrix = glm::perspective(radians(20.0f), 1.0f, 0.1f, 2000.0f);

	// 单一点光源，可以改为数组实现多光源
	lightPosition = objModel.centralPoint + vec3(0, 100, 100);

	clearBuffer(render_buffer);
	clearZBuffer(z_buffer);

	for (int i = 0; i < objModel.triangleCount; i++) {
		Triangle nowTriangle = objModel.getTriangleByID(i);
		drawTriangle(nowTriangle);
	}
	glClear(GL_COLOR_BUFFER_BIT);
	renderWithTexture(render_buffer, WindowSizeH, WindowSizeW);

	t2 = GetTickCount();
	printf("Runtime = %lf s.\n", ((t2 - t1) * 1.0 / 1000));
}

// GLSL方法渲染
void MyGLWidget::scene_1()
{
	// 选择要加载的model
	//objModel.loadModel("./objs/teapot_600.obj");
	objModel.loadModel("./objs/teapot_8000.obj");
	//objModel.loadModel("./objs/rock.obj");
	//objModel.loadModel("./objs/cube.obj");
	//objModel.loadModel("./objs/singleTriangle.obj");

	DWORD t1, t2;
	t1 = GetTickCount();

	// 自主设置变换矩阵
	camPosition = vec3(100 * sin(degree * 3.14 / 180.0) + objModel.centralPoint.y, 100 * cos(degree * 3.14 / 180.0) + objModel.centralPoint.x, 10+ objModel.centralPoint.z);
	camLookAt = objModel.centralPoint;     // 例如，看向物体中心
	camUp = vec3(0, 1, 0);         // 上方向向量
	projMatrix = glm::perspective(radians(45.0f), 1.0f, 0.1f, 2000.0f);
	viewMatrix = glm::lookAt(camPosition, camLookAt, camUp);

	// 单一点光源，可以改为数组实现多光源
	lightPosition = objModel.centralPoint + vec3(0,100,100);

	GLint num = objModel.triangleCount;
	vec3* vertex = new vec3[3 * num];
	vec3* color = new vec3[3 * num];
	vec3* normal = new vec3[3 * num];
	vec3 objectColor = vec3(0.7f, 0.7f, 0.7f);

	// 输入模型信息
	for (int i = 0; i < num; i++) {
		Triangle nowTriangle = objModel.getTriangleByID(i);
		for (int j = 0; j < 3; j++) {
			vertex[i * 3 + j] = nowTriangle.triangleVertices[j];
			normal[i * 3 + j] = nowTriangle.triangleNormals[j];
			color[i * 3 + j] = objectColor;
		}
	}

	glEnable(GL_DEPTH_TEST);

	// 创建顶点着色器和片段着色器
	GLuint VertexShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	// 获取.glsl文件中的着色器代码
	std::string VertexShaderCode = getShader("VertexShader.glsl");
	std::string FragmentShaderCode = getShader("FragmentShader.glsl");
	const GLchar* VertexShaderCodeArrary =  VertexShaderCode.c_str();
	const GLchar* FragmentShaderCodeArrary =  FragmentShaderCode.c_str() ;
	glShaderSource(VertexShader, 1, &VertexShaderCodeArrary, NULL);
	glShaderSource(FragmentShader, 1, &FragmentShaderCodeArrary, NULL);

	// 编译着色器
	glCompileShader(VertexShader);
	glCompileShader(FragmentShader);
	
	// 创建着色器程序
	GLuint ShaderProgram = glCreateProgram();
	glAttachShader(ShaderProgram, VertexShader);
	glAttachShader(ShaderProgram, FragmentShader);
	glLinkProgram(ShaderProgram);
	glDeleteShader(VertexShader);
	glDeleteShader(FragmentShader);

	// 创建VBO和VAO
	GLuint VBO[3], VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(3, VBO);

	// 绑定VAO
	glBindVertexArray(VAO);

	// 将顶点位置与VBO[0]绑定
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * num * 3, vertex, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	// 将顶点颜色与VBO[1]绑定
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * num * 3, color, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	// 将顶点法向量与VBO[2]绑定
	glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * num * 3, normal, GL_STATIC_DRAW);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);

	// 指定着色器程序
	glUseProgram(ShaderProgram);

	// 定义着色器变量
	GLuint vMloc = glGetUniformLocation(ShaderProgram, "view");
	GLuint pMloc = glGetUniformLocation(ShaderProgram, "proj");
	glUniformMatrix4fv(vMloc, 1, GL_FALSE, &viewMatrix[0][0]);
	glUniformMatrix4fv(pMloc, 1, GL_FALSE, &projMatrix[0][0]);
	GLuint lightPosLoc = glGetUniformLocation(ShaderProgram, "LightPos");
	GLuint lightColorLoc = glGetUniformLocation(ShaderProgram, "LightColor");
	GLuint viewPosLoc = glGetUniformLocation(ShaderProgram, "ViewPos");
	GLuint camPosLoc = glGetUniformLocation(ShaderProgram, "CamPos");
	vec3 LightPos = lightPosition;
	glUniform3f(lightPosLoc, LightPos.x, LightPos.y, LightPos.z);
	vec3 LightColor = vec3(1.0, 1.0, 1.0);
	glUniform3f(lightColorLoc, LightColor.r, LightColor.g, LightColor.b);
	vec3 ViewPos = camLookAt;
	glUniform3f(viewPosLoc, ViewPos.x, ViewPos.y, ViewPos.z);
	vec3 CamPos = camPosition;
	glUniform3f(camPosLoc, CamPos.x, CamPos.y, CamPos.z);

	// 清除BUFFER
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// 渲染
	glDrawArrays(GL_TRIANGLES, 0, 3 * num);

	// 清除VBO和VAO
	glDeleteBuffers(3, VBO);
	glDeleteVertexArrays(1, &VAO);

	t2 = GetTickCount();
	printf("Runtime = %lf s.\n", ((t2 - t1) * 1.0 / 1000));
}

void MyGLWidget::drawTriangle(Triangle triangle) {
	// 三维顶点映射到二维平面
	vec3* vertices = triangle.triangleVertices;
	vec3* normals = triangle.triangleNormals;
	FragmentAttr transformedVertices[3];
	clearBuffer(this->temp_render_buffer);
	clearZBuffer(this->temp_z_buffer);
	clearRecorder(this->edge_recorder);
	mat4 viewMatrix = glm::lookAt(camPosition, camLookAt, camUp);

    for (int i = 0; i < 3; ++i) {
		vec4 ver_mv = viewMatrix * vec4(vertices[i], 1.0f);
		float nowz = glm::length(camPosition - vec3(ver_mv));
		vec4 ver_proj = projMatrix * ver_mv;
		transformedVertices[i].x = ver_proj.x + offset.x;
		transformedVertices[i].y = ver_proj.y + offset.y;
		transformedVertices[i].z = nowz;
		transformedVertices[i].pos_mv = ver_mv;  
		mat3 normalMatrix = mat3(viewMatrix);
		vec3 normal_mv = normalMatrix * normals[i];
		transformedVertices[i].normal = normal_mv;
    }

	// 将当前三角形渲染在temp_buffer中

	/*bresenham方法*/
	bresenham(transformedVertices[0], transformedVertices[1], 1);
	bresenham(transformedVertices[1], transformedVertices[2], 2);
	bresenham(transformedVertices[2], transformedVertices[0], 3);

	/*Phong方法*/
	int firstChangeLine = edge_walking(transformedVertices[0], transformedVertices[1], transformedVertices[2]);

	// 从firstChangeLine开始遍历，可以稍快
	for(int h = firstChangeLine; h < WindowSizeH ; h++){
		auto render_row = &render_buffer[h * WindowSizeW];
		auto temp_render_row = &temp_render_buffer[h * WindowSizeW];
		auto z_buffer_row = &z_buffer[h*WindowSizeW];
		auto temp_z_buffer_row = &temp_z_buffer[h*WindowSizeW];
		for (int i = 0 ; i < WindowSizeW ; i++){
			if (z_buffer_row[i] < temp_z_buffer_row[i])
				continue;
			else
			{
				z_buffer_row[i] = temp_z_buffer_row[i];
				render_row[i] = temp_render_row[i];
			}
		}

	}
}

//Phong模型的edge_walking方法
int MyGLWidget::edge_walking(FragmentAttr& a, FragmentAttr& b, FragmentAttr& c) {
	// 遍历edge_recorder在不同高度的起点、终点，用shading model计算内部每个像素的颜色
	for (int i = 0; i < WindowSizeH; i++) {
		if (edge_recorder[i][2] >= 2) {
			for (int j = edge_recorder[i][0]; j <= edge_recorder[i][1]; j++) {
				vec3 cur_norm = getLinearInterpolation_norm(a, b, c, j, i);
				vec3 cur_pos = getLinearInterpolation_pos(a, b, c, j, i);
				float cur_z = getLinearInterpolation_z(a, b, c, j, i);
				vec3 cur_color = PhongShading(cur_norm, cur_pos);
				temp_render_buffer[WindowSizeW * i + j] = cur_color;
				temp_z_buffer[WindowSizeW * i + j] = cur_z;
			}
		}
	}
	return min(min(a.y, b.y), c.y);
}

vec3 MyGLWidget::PhongShading(vec3 cur_norm, vec3 cur_pos) {

	vec3 objColor = vec3(0.7, 0.7, 0.7);
	float dist = vecDistance(lightPosition, cur_pos);
	float dist_a = 1.0f, dist_b = 0.001f, dist_c = 0.000001f;
	//环境光照
	vec3 aLightColor = vec3(1.0, 1.0, 1.0);
	float amb_k = 0.2f;
	vec3 amb = amb_k * aLightColor;

	//漫反射光照
	vec3 dLightColor = vec3(1.0, 1.0, 1.0);
	float dif_k = 1.0f;
	vec3 vec_L = vecNormalize(vecSub(lightPosition, cur_pos));
	vec3 vec_N = vecNormalize(cur_norm);
	float dif_alpha = 1.0f;
	vec3 dif = dif_k * max(pow(vecDot(vec_L, vec_N), dif_alpha), 0.0f) * dLightColor;

	//镜面反射光照
	vec3 sLightColor = vec3(1.0, 1.0, 1.0);
	float spe_k = 0.8f;
	vec3 vec_R = vecNormalize(reflect(- vec_L, vec_N));
	vec3 vec_V = vecNormalize(vecSub(camPosition, camLookAt));
	float spe_alpha = 2.0f;
	vec3 spe = spe_k * max(pow(vecDot(vec_V, vec_R), spe_alpha), 0.0f) * sLightColor;

	//计算最终颜色
	vec3 color = objColor * ((1 / (dist_a + dist_b * dist + dist_c * dist * dist)) * (dif + spe) + amb);
	return color;
}


void MyGLWidget::bresenham(FragmentAttr& start, FragmentAttr& end, int id) {
	// 根据起点、终点，计算当前边在画布上的像素
	int x0 = start.x, x1 = end.x, x = start.x;
	int y0 = start.y, y1 = end.y, y = start.y;
	int dx = abs(x1 - x0);
	int dy = abs(y1 - y0);
	int sx = (x1 >= x0) ? 1 : -1; //x增量
	int sy = (y1 >= y0) ? 1 : -1; //y增量
	int flag;

	if (dy <= dx) {
		flag = 2 * dy - dx;
		for (int i = 0; i <= dx; i++) {
			if (x < WindowSizeW && x >= 0 && y < WindowSizeH && y >= 0) {
				//temp_render_buffer[WindowSizeW * y + x] = vec3(0.0f, 1.0f, 0.0f);
				//FragmentAttr interpolation = getLinearInterpolation(start, end, x);
				//temp_z_buffer[WindowSizeW * y + x] = interpolation.z;
				updateRecorder(x, y);
			}
			x += sx;
			if (flag < 0) {
				flag += 2 * dy;
			}
			else {
				y += sy;
				flag += 2 * dy - 2 * dx;
			}
		}
	}
	else {
		flag = 2 * dx - dy;
		for (int i = 0; i <= dy; i++) {
			if (x < WindowSizeW && x >= 0 && y < WindowSizeH && y >= 0) {
				//temp_render_buffer[WindowSizeW * y + x] = vec3(0.0f, 1.0f, 0.0f);
				//FragmentAttr interpolation = getLinearInterpolation(start, end, x);
				//temp_z_buffer[WindowSizeW * y + x] = interpolation.z;

				updateRecorder(x, y);
			}
			y += sy;
			if (flag < 0) {
				flag += 2 * dx;
			}
			else {
				x += sx;
				flag += 2 * dx - 2 * dy;
			}
		}
	}
}