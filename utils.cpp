#include "utils.h"
FragmentAttr getLinearInterpolation(const FragmentAttr& a, FragmentAttr& b, int x_position){
    FragmentAttr result;
    result.x = x_position;
    float t = (x_position - a.x) / float(b.x - a.x);

    result.z = a.z + t * (b.z - a.z);
	
    result.color.r = a.color.r + t * (b.color.r - a.color.r);
    result.color.g = a.color.g + t * (b.color.g - a.color.g);
    result.color.b = a.color.b + t * (b.color.b - a.color.b);

    result.normal.x = a.normal.x + t * (b.normal.x - a.normal.x);
    result.normal.y = a.normal.y + t * (b.normal.y - a.normal.y);
    result.normal.z = a.normal.z + t * (b.normal.z - a.normal.z);

    result.pos_mv.x = a.pos_mv.x + t * (b.pos_mv.x - a.pos_mv.x);
    result.pos_mv.y = a.pos_mv.y + t * (b.pos_mv.y - a.pos_mv.y);
    result.pos_mv.z = a.pos_mv.z + t * (b.pos_mv.z - a.pos_mv.z);

    return result;
}

float getLinearInterpolation_z(FragmentAttr& a, FragmentAttr& b, FragmentAttr& c, int x_position, int y_position) {
	float A, B, C, D;
	int x1 = a.x, x2 = b.x, x3 = c.x;
	int y1 = a.y, y2 = b.y, y3 = c.y;
	float z1 = a.z, z2 = b.z, z3 = c.z;

	A = (y2 - y1) * (z3 - z1) - (z2 - z1) * (y3 - y1);
	B = (z2 - z1) * (x3 - x1) - (x2 - x1) * (z3 - z1);
	C = (x2 - x1) * (y3 - y1) - (y2 - y1) * (x3 - x1);
	D = -(A * x1 + B * y1 + C * z1);

	return -(A * x_position + B * y_position + D) / C;
}

vec3 getLinearInterpolation_color(FragmentAttr& a, FragmentAttr& b, FragmentAttr& c, int x_position, int y_position) {
	float A, B, C, D;
	int x1 = a.x, x2 = b.x, x3 = c.x;
	int y1 = a.y, y2 = b.y, y3 = c.y;
	vec3 result = vec3(0, 0, 0);

	for (int i = 0; i <= 2; i++) {
		float z1 = a.color[i], z2 = b.color[i], z3 = c.color[i];
		A = (y2 - y1) * (z3 - z1) - (z2 - z1) * (y3 - y1);
		B = (z2 - z1) * (x3 - x1) - (x2 - x1) * (z3 - z1);
		C = (x2 - x1) * (y3 - y1) - (y2 - y1) * (x3 - x1);
		D = -(A * x1 + B * y1 + C * z1);

		result[i]= -(A * x_position + B * y_position + D) / C;
	}
	return result;
}

vec3 getLinearInterpolation_norm(FragmentAttr& a, FragmentAttr& b, FragmentAttr& c, int x_position, int y_position) {
	float A, B, C, D;
	int x1 = a.x, x2 = b.x, x3 = c.x;
	int y1 = a.y, y2 = b.y, y3 = c.y;
	vec3 result = vec3(0, 0, 0);

	for (int i = 0; i <= 2; i++) {
		float z1 = a.normal[i], z2 = b.normal[i], z3 = c.normal[i];
		A = (y2 - y1) * (z3 - z1) - (z2 - z1) * (y3 - y1);
		B = (z2 - z1) * (x3 - x1) - (x2 - x1) * (z3 - z1);
		C = (x2 - x1) * (y3 - y1) - (y2 - y1) * (x3 - x1);
		D = -(A * x1 + B * y1 + C * z1);

		result[i] = -(A * x_position + B * y_position + D) / C;
	}
	return result;
}

vec3 getLinearInterpolation_pos(FragmentAttr& a, FragmentAttr& b, FragmentAttr& c, int x_position, int y_position) {
	float A, B, C, D;
	int x1 = a.x, x2 = b.x, x3 = c.x;
	int y1 = a.y, y2 = b.y, y3 = c.y;
	vec3 result = vec3(0, 0, 0);

	for (int i = 0; i <= 2; i++) {
		float z1 = a.pos_mv[i], z2 = b.pos_mv[i], z3 = c.pos_mv[i];
		A = (y2 - y1) * (z3 - z1) - (z2 - z1) * (y3 - y1);
		B = (z2 - z1) * (x3 - x1) - (x2 - x1) * (z3 - z1);
		C = (x2 - x1) * (y3 - y1) - (y2 - y1) * (x3 - x1);
		D = -(A * x1 + B * y1 + C * z1);

		result[i] = -(A * x_position + B * y_position + D) / C;
	}
	return result;
}

vec3 vecNormalize(vec3 a) {
	float len = sqrt(a[0] * a[0] + a[1] * a[1] + a[2] * a[2]);

	return vec3(a[0] / len, a[1] / len, a[2] / len);
}

float vecDot(vec3 a, vec3 b) {
	return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

vec3 vecCross(vec3 a, vec3 b) {
	return vec3(a[1] * b[2] - a[2] * b[1], a[2] * b[0] - a[0] * b[2], a[0] * b[1] - a[1] * b[0]);
}

float vecDistance(vec3 a, vec3 b) {
	return sqrt((a[0] - b[0])* (a[0] - b[0])+(a[1]-b[1])* (a[1] - b[1])+(a[2]-b[2])* (a[2] - b[2]));
}

vec3 vecAdd(vec3 a, vec3 b) {
	return vec3(a[0] + b[0], a[1] + b[1], a[2] + b[2]);
}

vec3 vecSub(vec3 a, vec3 b) {
	return vec3(a[0] - b[0], a[1] - b[1], a[2] - b[2]);
}

vec3 vecDiv(vec3 a, float f) {
	return vec3(a[0] / f, a[1] / f, a[2] / f);
}

std::string getShader(const char* file) {
	std::ifstream fileStream(file, std::ios::in);
	std::stringstream buffer;
	buffer << fileStream.rdbuf();
	fileStream.close();
	return buffer.str();
}

void renderWithTexture(vec3* render_buffer,int h, int w) {
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glEnable(GL_TEXTURE_2D);	
	GLuint texID;
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_FLOAT, render_buffer);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texID);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex2f(-1.0f, -1.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex2f(-1.0f, 1.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex2f(1.0f, 1.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex2f(1.0f, -1.0f);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glPopAttrib();
}