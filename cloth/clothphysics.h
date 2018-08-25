#ifndef CLOTHPHYSICS_H
#define CLOTHPHYSICS_H

#include"glmInc.h"
#include<vector>
#include"constants.h"

using namespace std;

struct clothvertexP
{
	glm::vec3 pos = glm::vec3(0.f);
	glm::vec3 vel = glm::vec3(0.f);
	float m = 1.f;
	float w = 1 / m;
	glm::vec3 externalf = glm::vec3(0.f);
	glm::vec3 elasticf = glm::vec3(0.f);
};


class clothObj
{
public:
	clothObj();
	vector<clothvertexP> Da_cloth;
	int framecount;
	glm::vec3 sphereCentre;
	void initializeCloth();
	void computeExternalF(int frames);
	void update();
	void computeElasticF();
	void boundaryCondition();
	void advect(float dt);
	void solverMainLoop(float dt,int framecount);
};


#endif