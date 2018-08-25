#include"stringphysics.h"

stringObj::stringObj() :vert_count(12) {};

stringObj::stringObj(int count) :vert_count(count)
{};

void stringObj::update()
{
	for (int i = 0; i < Da_string.size(); ++i)
	{
		Da_string[i].m_e = glm::vec3(0);
		Da_string[i].m_f = glm::vec3(0);
	}
}


bool stringObj::stringInitialize()
{
		for (int i = 0; i < vert_count; ++i)
		{
			vertexP tmp;
			tmp.pos = glm::vec3(0, float(10 - i), 0);
			Da_string.push_back(tmp);
		}
		Da_string[Da_string.size()-1].vel = glm::vec3(15.f, 0, 0);
		return true;
	
}

//mass-spring elastic force
void stringObj::computelasticF()
{
	for (int i = 0; i < Da_string.size(); ++i)
	{
		if (i - 1 >= 0)
		{
			glm::vec3 offset = (-Da_string[i].pos + Da_string[i - 1].pos);
			Da_string[i].m_f += hook*glm::normalize(offset)*(glm::length(offset)-1.f);
		}
		if (i + 1 < Da_string.size())
		{
			glm::vec3 offset = (-Da_string[i].pos + Da_string[i +1 ].pos);
			Da_string[i].m_f += hook*glm::normalize(offset)*(glm::length(offset) - 1.f);
		}
	}
	int a=10;
}


//integration over timestep dt
void stringObj::advect(const float dt)
{

	for (int i = 1; i < Da_string.size(); ++i)
	{
		Da_string[i].vel += (Da_string[i].m_f+Da_string[i].m_e / Da_string[i].m_p)*dt;
	}
	for (int i = 1; i < Da_string.size(); ++i)
	{
		Da_string[i].pos += Da_string[i].vel*dt;
	}
}

//damping force and gravity
void stringObj::computeExternalF()
{
	for (int i = 1; i < Da_string.size(); ++i)
	{
		Da_string[i].m_e = Gconst*Da_string[i].m_p*glm::vec3(0,1,0)-damping*Da_string[i].vel;
	}
}