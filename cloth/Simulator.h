#ifndef SIMULATOR_H
#define SIMULATOR_H

#include"stringphysics.h"
#include"clothphysics.h"
#include<vector>
#include<Eigen\Eigen>

class SimMain
{
public:
	Eigen::Vector3f strvertices[strLength] ;
	Eigen::Vector3f clothvertices[clothH*clothW];
	Eigen::Vector3f clothNormals[clothH*clothW];
	stringObj *str1 = new stringObj(strLength);
	clothObj *cloth1 = new clothObj();
	SimMain();
	void initialize();
	void createstrVertices();
	void createclothVertices();
	void createclothNormals();
	void strstep();
	void clothstep(int framecount);
	~SimMain() { delete str1;
	delete cloth1;
	}
};

#endif // !SIMULATOR_H
