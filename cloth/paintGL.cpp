#include"paintGL.h"


using namespace Eigen;
GLuint VBO;
GLuint BufNor;
GLuint IBO;
GLuint gScaleLocation;
GLuint viewProjMat;
GLuint lightDir;

const char* pvertexShader = "shader.vs";
const char* pfragmentShader = "shader.fs";

#define windowH 720
#define windowW 1280

Camera P_camera(windowW,windowH);


static int frame_count = 0;
static int simFrameCount = 0;
//mouse pos for recording
glm::vec2 m_mousepos = glm::vec2(windowW / 2.f, windowH / 2.f);

//camera angles for mouse interactions
static float m_angleH = 0.f;
static float m_angleV = 0.f;
//flags for mouse actions
bool mouseLeftDown = false;
bool mouseRightDown = false;

SimMain sim1;

static float amt = 0.1f;
float angle = 0.f;
glm::vec3 light = glm::vec3(-1, -1, -1);
unsigned int globalIndices[(clothH - 1)*(clothW - 1) * 3 * 2];
Eigen::Vector3f p_normal[clothH*clothW];
static void CreatestrIndexBuffer()
{
	unsigned int indices[(strLength-1)*2];
	for (int i = 0; i < strLength-1; i++)
	{
		indices[2*i] = i;
		indices[2*i + 1] = i + 1;
	}
	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
}


void CreatestrVertexBuffer()
{

	//Vertices[0] = Vector3f(-1.0f, -1.0f, 0.0f);
	//Vertices[1] = Vector3f(0.0f, -1.0f, 1.0f);
	//Vertices[2] = Vector3f(1.0f, -1.0f, 0.0f);
	//Vertices[3] = Vector3f(0.0f, 1.0f, 0.0f);
	//for (int i = 0; i < strLength; ++i)
	//{
	//	std::cout << sim1.vertices[i] << endl;
	//}
	//std::cout << endl;
	
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(sim1.strvertices), &sim1.strvertices[0], GL_STATIC_DRAW);
}

static void CreateclothIndexBuffer()
{
	//unsigned int indices[clothW*clothH];
	//for (int i = 0; i < clothH*clothW; ++i)
	//{
	//	indices[i] = i;
	//}
	//lines=================================================
	//unsigned int indices[clothW*(clothH)*2+clothH*(clothW-1)*2];
	//for (int i = 0; i < clothH; ++i)
	//{
	//	for (int j = 0; j < clothW - 1; ++j)
	//	{
	//		indices[2 * (i*(clothW - 1) + j)] = i*(clothW ) + j;
	//		indices[2 * (i*(clothW - 1) + j) + 1] = i*(clothW ) + j + 1;
	//	}
	//}
	//int css = clothH*(clothW - 1) * 2;
	//for (int i =0; i < clothW-1; ++i)
	//{
	//	//if (i == clothW - 1) continue;
	//	for (int j = 0; j < clothH ; ++j)
	//	{
	//	
	//		{
	//			indices[css + 2 * (i*(clothH) + j)] = i*(clothH)+j;
	//			indices[css + 2 * (i*(clothH) + j) + 1] = i*(clothH)+j + clothW;
	//		}
	//	}
	//}
	//=====================================================
	//triangles============================================
	
	for (int i = 0; i < clothH-1; i++)
	{
		for (int j = 0; j < clothW-1; ++j)
		{
			globalIndices[(i*(clothW - 1) + j)*6] = i*clothW + j;
			globalIndices[(i*(clothW - 1) + j) * 6  + 1] = (i + 1)*clothW + j;
			globalIndices[(i*(clothW - 1) + j) * 6  + 2] = (i + 1)*clothW + j + 1;
			globalIndices[(i*(clothW - 1) + j) * 6  + 3] = i *clothW + j;
			globalIndices[(i*(clothW - 1) + j) * 6  + 4] = i *clothW + j + 1;
			globalIndices[(i*(clothW - 1) + j) * 6  + 5] = (i + 1) *clothW + j + 1;
			//for (int k = 0; k < 6; ++k)
			//{
			//	std::cout << indices[i*(clothW - 1) + j + k] << ",";
			//}
			//std::cout << std::endl;
		}
	}
	//=======================================================
	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(globalIndices), globalIndices, GL_STATIC_DRAW);

}

void CreateclothNorBuffer()
{
	glGenBuffers(1, &BufNor);
	glBindBuffer(GL_ARRAY_BUFFER, BufNor);
	glBufferData(GL_ARRAY_BUFFER,sizeof(sim1.clothNormals),&sim1.clothNormals[0],GL_STATIC_DRAW);


}
void CalcNormals(const unsigned int* pIndices, unsigned int IndexCount, Eigen::Vector3f pVertices[], unsigned int VertexCount)
{
	for (int i = 0; i < VertexCount; i++)
	{
		p_normal[i] = Vector3f(0,0,0);
	}
	for (unsigned int i = 0; i < IndexCount; i += 3) {
		unsigned int Index0 = pIndices[i];
		unsigned int Index1 = pIndices[i + 1];
		unsigned int Index2 = pIndices[i + 2];
		Vector3f v1 = pVertices[Index1] - pVertices[Index0];
		Vector3f v2 = pVertices[Index2] - pVertices[Index0];
		glm::vec3 nor = glm::normalize(glm::cross(glm::vec3(v1(0), v1(1), v1(2)), glm::vec3(v2(0), v2(1), v2(2))));
		Vector3f Nor = Vector3f(nor[0], nor[1], nor[2]);
		p_normal[Index0] += Nor;
		p_normal[Index1] += Nor;
		p_normal[Index2] += Nor;
	}

	for (unsigned int i = 0; i < VertexCount; i++) {
		glm::vec3 tmp = glm::normalize(glm::vec3(p_normal[i](0), p_normal[i](1), p_normal[i](2)));
		p_normal[i] = Vector3f(tmp[0], tmp[1], tmp[2]);
		//std::cout << p_normal[i] << std::endl;
	}
}
void CreateclothVertexBuffer()
{
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(sim1.clothvertices), &sim1.clothvertices[0], GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(sim1.clothvertices), &sim1.clothvertices[0]);

	//CalcNormals(globalIndices, (clothH - 1)*(clothW - 1) * 3 * 2, sim1.clothvertices, clothH*clothW);
	//glGenBuffers(1, &BufNor);
	//glBindBuffer(GL_ARRAY_BUFFER, BufNor);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(p_normal), &p_normal[0], GL_STATIC_DRAW);

}


inline bool dampout()
{
	if (frame_count < dampOut)
	{
		frame_count++;
		//cout << " ";
		return false;
	}
	else {
		frame_count = 0;
		return true;
	}
}

static void renderscene()
{

	if (dampout())
	{
		
		glClear(GL_COLOR_BUFFER_BIT);
		glClear(GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		angle += 0.5;
		static float Scale = 0.f;
		Scale += 0.01f;
		//glUniform1f(gScaleLocation, sinf(Scale));

		//CreatestrIndexBuffer();
		//CreatestrVertexBuffer();
#ifndef CLOTHIDXBUFFER
#define CLOTHIDXBUFFER
		CreateclothIndexBuffer();
#endif // !CLOTHIDXBUFFER


		CreateclothVertexBuffer();
		CreateclothNorBuffer();

		glm::vec3 Dirs = glm::normalize(glm::vec3(glm::vec4(light, 1)*glm::rotate(glm::mat4(1.f), glm::radians(angle), glm::vec3(0, 1, 0))));

		glm::mat4 m1 = glm::mat4(cosf(Scale), 0.0f, -sinf(Scale), 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			sinf(Scale), 0.0f, cosf(Scale), 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f);
		glm::mat4 viewProj = P_camera.getViewProj();
		glUniformMatrix4fv(viewProjMat, 1, GL_FALSE, &viewProj[0][0]);
		glUniformMatrix4fv(gScaleLocation, 1, GL_TRUE, &m1[0][0]);
		glUniform3fv(lightDir, 1, &Dirs[0]);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, BufNor);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
		glDrawElements(GL_TRIANGLES, (clothH - 1)*(clothW - 1) * 3 * 2 /*clothW*(clothH ) * 2 + clothH*(clothW - 1) * 2*//*(strLength - 1) * 2*/, GL_UNSIGNED_INT, 0);
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);

		//========================
		glutPostRedisplay();
		glutSwapBuffers();
	}
}
bool Onkeyboard(char key)
{
	bool ret = false;
	switch (key)
	{	
	case 27:
	{
		exit(0);
	}
	break;
	case 'q':
	{
		if (!P_camera.enable_spherical)
			P_camera.TranslateAlongUp(amt);
		else
			P_camera.translateSphericalUp(amt);
		ret = true;
	}
	break;

	case 'e':
	{
		if (!P_camera.enable_spherical)
			P_camera.TranslateAlongUp(-amt);
		else
			P_camera.translateSphericalUp(-amt);
		ret = true;

	}
	break;

	case 'a':
	{
		if (!P_camera.enable_spherical)
			P_camera.TranslateAlongRight(-amt);
		else
			P_camera.translateSphericalRight(-amt);
		ret = true;
	}
	break;

	case 'd':
	{
		if (!P_camera.enable_spherical)
			P_camera.TranslateAlongRight(amt);
		else
			P_camera.translateSphericalRight(amt);
		ret = true;
	}
	break;
	
	case 'w':
	{
		if (!P_camera.enable_spherical)
			P_camera.TranslateAlongLook(amt);
		else
			P_camera.zoomInOut(amt);
		ret = true;
	}
	break;

	case 's':
	{
		if (!P_camera.enable_spherical)
			P_camera.TranslateAlongLook(-amt);
		else
			P_camera.zoomInOut(-amt);
		ret = true;
	}
	break;
	//=====================================
	case 'j':
	{
		if (!P_camera.enable_spherical)
			P_camera.RotateAboutRight(amt*10);
		else
			P_camera.rotateSphericalright(amt * 10);
		ret = true;
	}
	break;

	case 'i':
	{
		if (!P_camera.enable_spherical)
			P_camera.RotateAboutUp(amt * 10);
		else
			P_camera.rotateSphericalUp(amt * 10);
		ret = true;
	}
	break;

	case 'l':
	{
		if (!P_camera.enable_spherical)
			P_camera.RotateAboutRight(-amt * 10);
		else
			P_camera.rotateSphericalright(-amt * 10);
		ret = true;
	}
	break;

	case 'k':
	{		
		if (!P_camera.enable_spherical)
		P_camera.RotateAboutUp(-amt * 10);
		else
		P_camera.rotateSphericalUp(-amt * 10);
		ret = true;
	}
	break;
	}

	return ret;
}

void Onmouse(int x, int y)
{
	int deltaX = x - m_mousepos[0];
	int deltaY = y - m_mousepos[1];
	if (mouseRightDown)
	{

		float scale = 6.f;
		P_camera.translateSphericalRight(-float(deltaX/scale));
		P_camera.translateSphericalUp(float(deltaY/scale));

	}
	else if (mouseLeftDown)
	{
		float scale = 15.f;
		P_camera.rotateSphericalright(float(-deltaX / scale));
		P_camera.rotateSphericalUp(float(-deltaY / scale));
		
	}
	m_mousepos = glm::vec2(x, y);
	 //const int deltaX =  ;
}
//static  void SpecialKeyboardCB(int key, int x, int y)
//{
//	OGLDEV_KEY ogkey = GLUTKeyToOGLDEVKey(key);
//	Onkeyboard(key);
//
//}

void mousefuc(int x, int y)
{
	Onmouse(x, y);

}
void mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON&&state == GLUT_DOWN)
	{
		m_mousepos = glm::vec2(x, y);
		mouseLeftDown = true;
	}
	else if (button == GLUT_RIGHT_BUTTON&&state == GLUT_DOWN)
	{
		m_mousepos = glm::vec2(x, y);
		mouseRightDown = true;
	}
	else if (button == GLUT_UP)
	{
		mouseLeftDown = false;
		mouseRightDown = false;
	}
}

void keybordfuc(unsigned char key, int x, int y)
{
	Onkeyboard(key);
}

void OnTimerCb(int val)
{
	simFrameCount++;
	sim1.clothstep(simFrameCount);
	glutTimerFunc(0.1, OnTimerCb, 0);
	glutPostRedisplay();
}



static void InitializeGlutCallbacks()
{
	glutDisplayFunc(renderscene);
	glutIdleFunc(renderscene);
	//glutSpecialFunc(SpecialKeyboardCB);
	glutTimerFunc(0.1,OnTimerCb , 0);
	glutKeyboardFunc(keybordfuc);
	glutMouseFunc(mouse);
	glutMotionFunc(mousefuc);
}




static void AddShader(GLuint ShaderProgram, const char* pshadertext, GLenum shadertype)
{
	GLuint ShaderOBJ = glCreateShader(shadertype);
	if (!ShaderOBJ) {
		fprintf(stderr, "Error creating shader type %d\n", shadertype);
		exit(0);
	}
	const GLchar* p[1];
	p[0] = pshadertext;
	GLint Lengths[1];
	Lengths[0] = strlen(pshadertext);
	glShaderSource(ShaderOBJ, 1, p, Lengths);
	glCompileShader(ShaderOBJ);

	GLint success;
	glGetShaderiv(ShaderOBJ, GL_COMPILE_STATUS, &success);
	if (!success) {
		GLchar Infolog[1024];
		glGetShaderInfoLog(ShaderOBJ, 1024, NULL, Infolog);
		fprintf(stderr, "Error compiling shader type %d: '%s\n", shadertype, Infolog);
		exit(1);
	}
	glAttachShader(ShaderProgram, ShaderOBJ);
}

static void CompileShaders()
{
	GLuint ShaderProgram = glCreateProgram();
	if (ShaderProgram == 0)
	{
		fprintf(stderr, "Error creating shader program\n");
		exit(1);
	}

	string vs, fs;
	if (!ReadFile(pvertexShader, vs)) {
		exit(1);
	}
	if (!ReadFile(pfragmentShader, fs)) {
		exit(1);
	}

	AddShader(ShaderProgram, vs.c_str(), GL_VERTEX_SHADER);
	AddShader(ShaderProgram, fs.c_str(), GL_FRAGMENT_SHADER);

	GLint success = 0;
	GLchar Errorlog[1024] = { 0 };
	glLinkProgram(ShaderProgram);
	glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &success);
	if (success == 0) {
		glGetProgramInfoLog(ShaderProgram, sizeof(Errorlog), NULL, Errorlog);
		fprintf(stderr, "Error linking shader program:'%s'\n",Errorlog);
		exit(1);
	}

	glValidateProgram(ShaderProgram);
	glGetProgramiv(ShaderProgram, GL_VALIDATE_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(ShaderProgram, sizeof(Errorlog), NULL, Errorlog);
		fprintf(stderr, "Invalid shader program: '%s'\n", Errorlog);
		exit(1);
	}
	glUseProgram(ShaderProgram);

	gScaleLocation = glGetUniformLocation(ShaderProgram, "gScale");
	assert(gScaleLocation!=0xFFFFFFFF);

	lightDir = glGetUniformLocation(ShaderProgram, "lightDir");
	assert(lightDir != 0xFFFFFFFF);

	viewProjMat = glGetUniformLocation(ShaderProgram, "viewproj");
	assert(viewProjMat != 0xFFFFFFFF);
}

float getTime() {
	return clock() / float(CLOCKS_PER_SEC);
}



void callOpenGls(int argc, char ** argv)
{
	P_camera.enableSphericalCamera();
	glutInit(&argc, argv);
	glEnable(GL_DEPTH_TEST);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(windowW, windowH);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("STRING_SIM_VER1");
	InitializeGlutCallbacks();
	GLenum res = glewInit();
	if (res != GLEW_OK)
	{
		fprintf(stderr, "Error:'%s'\n", glewGetErrorString(res));
		/*return 1;*/
	}

	printf("GL Version :%s\n", glGetString(GL_VERSION));

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	//original creating buffer place
	CompileShaders();
	glutMainLoop();

}
void Timerupdate()
{
	int i = 0;
	float last = 0.f;
	while (1)
	{
		float now = getTime();
		if (now - last > 0.016f)
		{
			cout << ++i;
			last = now;
		}
	}
}

int main(int argc,char ** argv)
{
	//glutWarpPointer(windowW/2.f, windowH/2.f);
	P_camera.TranslateAlongLook(-55);
	P_camera.TranslateAlongUp(-10);
	P_camera.TranslateAlongRight(10);

	callOpenGls(argc, argv);
	return 0;
}

