#include"Simulator.h"
#include<iostream>

void SimMain::initialize()
{
	str1->stringInitialize();
	createstrVertices();
}

SimMain::SimMain()
{
	str1->stringInitialize();
	cloth1->initializeCloth();
}

void SimMain::createstrVertices()
{
	for (int i = 0; i < str1->Da_string.size(); ++i)
	{
		strvertices[i] = Eigen::Vector3f(str1->Da_string[i].pos[0], str1->Da_string[i].pos[1], str1->Da_string[i].pos[2]);
	}
}

void SimMain::createclothVertices()
{
	for (int i = 0; i < cloth1->Da_cloth.size(); ++i)
	{
		clothvertices[i] = Eigen::Vector3f(cloth1->Da_cloth[i].pos[0], cloth1->Da_cloth[i].pos[1], cloth1->Da_cloth[i].pos[2]);
		//std::cout << clothvertices[i] << endl;
	}
	
}

void SimMain::createclothNormals()
{
	for (int i = 0; i < clothH; ++i)
	{
		for (int j = 0; j < clothW; ++j)
		{
			std::vector<glm::vec3> neighborP;
			if (j + i*clothW - clothW >= 0)
			{
				glm::vec3 tmp = glm::vec3(clothvertices[j + i*clothW - clothW](0),
					clothvertices[j + i*clothW - clothW](1),
					clothvertices[j + i*clothW - clothW](2));
				neighborP.push_back(tmp);
			}
			if (j + i*clothW - 1 >= i*clothW)
			{
				glm::vec3 tmp = glm::vec3(clothvertices[j + i*clothW - 1](0),
					clothvertices[j + i*clothW - 1](1),
					clothvertices[j + i*clothW - 1](2));
				neighborP.push_back(tmp);
			}
			if (j + i*clothW + 1 < (i + 1)*clothW)
			{
				glm::vec3 tmp = glm::vec3(clothvertices[j + i*clothW + 1](0),
					clothvertices[j + i*clothW + 1](1),
					clothvertices[j + i*clothW + 1](2));
				neighborP.push_back(tmp);
			}
			if (j + i*clothW + clothW < clothW*clothH)
			{
				glm::vec3 tmp = glm::vec3(clothvertices[j + i*clothW + clothW](0),
					clothvertices[j + i*clothW + clothW](1),
					clothvertices[j + i*clothW + clothW](2));
				neighborP.push_back(tmp);
			}

			if (neighborP.size() == 2)
			{
				glm::vec3 tmpn = glm::normalize(glm::cross(neighborP[0], neighborP[1]));
				clothNormals[j + i*clothW] = Eigen::Vector3f(tmpn[0],tmpn[1],tmpn[2]);
			}
			else if (neighborP.size() == 4)
			{
				glm::vec3 tmpn1 = glm::normalize(glm::cross(neighborP[0], neighborP[1]));
				glm::vec3 tmpn2 = glm::normalize(glm::cross(neighborP[2], neighborP[3]));
				glm::vec3 tmpn = glm::normalize((tmpn1 + tmpn2) / 2.f);
				clothNormals[j + i*clothW] = Eigen::Vector3f(tmpn[0], tmpn[1], tmpn[2]);
			}
			else if(neighborP.size()==3)
			{
				if (i == 0)
				{
					glm::vec3 tmpn1 = glm::normalize(glm::cross(neighborP[1], neighborP[0]));
					glm::vec3 tmpn2 = glm::normalize(glm::cross(neighborP[0], neighborP[2]));
					glm::vec3 tmpn = glm::normalize((tmpn1 + tmpn2) / 2.f);
					clothNormals[j + i*clothW] = Eigen::Vector3f(tmpn[0], tmpn[1], tmpn[2]);
				}
				else if (i == clothH - 1)
				{
					glm::vec3 tmpn1 = glm::normalize(glm::cross(neighborP[0], neighborP[1]));
					glm::vec3 tmpn2 = glm::normalize(glm::cross(neighborP[2], neighborP[0]));
					glm::vec3 tmpn = glm::normalize((tmpn1 + tmpn2) / 2.f);
					clothNormals[j + i*clothW] = Eigen::Vector3f(tmpn[0], tmpn[1], tmpn[2]);
				}
				else if (j == 0 )
				{
					glm::vec3 tmpn1 = glm::normalize(glm::cross(neighborP[0], neighborP[2]));
					glm::vec3 tmpn2 = glm::normalize(glm::cross(neighborP[1], neighborP[2]));
					glm::vec3 tmpn = glm::normalize((tmpn1 + tmpn2) / 2.f);
					clothNormals[j + i*clothW] = Eigen::Vector3f(tmpn[0], tmpn[1], tmpn[2]);
				}
				else if (j == clothW - 1)
				{
					glm::vec3 tmpn1 = glm::normalize(glm::cross(neighborP[0], neighborP[1]));
					glm::vec3 tmpn2 = glm::normalize(glm::cross(neighborP[0], neighborP[2]));
					glm::vec3 tmpn = glm::normalize((tmpn1 + tmpn2) / 2.f);
					clothNormals[j + i*clothW] = Eigen::Vector3f(tmpn[0], tmpn[1], tmpn[2]);
				}

			}
			
		}
		
	}
}

void SimMain::strstep()
{
	str1->update();
	str1->computeExternalF();
	str1->computelasticF();
	str1->advect(dealtaT);
	createstrVertices();
}

void SimMain::clothstep(int framecount)
{
	cloth1->solverMainLoop(dealtaT,framecount);
	createclothVertices();
	createclothNormals();
}