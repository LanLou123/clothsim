#ifndef STRINGPHYSICS_H
#define STRINGPHYSICS_H

#include"glmInc.h"
#include<vector>
#include"constants.h"

using namespace std;

struct vertexP 
{
	glm::vec3 pos=glm::vec3(0.f);
	float m_p = 30.f;
	glm::vec3 vel = glm::vec3(0.f);
	glm::vec3 m_f = glm::vec3(0.f);
	glm::vec3 m_e = glm::vec3(0.f);
};

class stringObj
{
public:
	int vert_count;
	vector<vertexP> Da_string;
	stringObj();
	stringObj(int vert_count);
	bool stringInitialize();
	void computelasticF();
	void computeExternalF();
	void advect(const float dt);
	void update();
};

#endif // !STRINGPHYSICS_H
