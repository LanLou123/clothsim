#include"clothphysics.h"
#include<math.h>

clothObj::clothObj(){}

void clothObj::initializeCloth()
{
	sphereCentre = glm::vec3(12.5, -13, 10);
	for (int i = 0; i < clothH; ++i)
	{
		for (int j = 0; j < clothW; ++j)
		{
			clothvertexP tmp;
			tmp.pos = glm::vec3(  j, 0, i);
			tmp.m = 1.f;
			tmp.vel = glm::vec3(0.f);
			tmp.w = 1/float(tmp.m);
			Da_cloth.push_back(tmp);
			if(i==0&&(j==0||j==clothW-1))
			Da_cloth[j].m = std::numeric_limits<float>::max();
		}
		
	}
	
}

void clothObj::update()
{
	for (int i = 0; i < Da_cloth.size(); ++i)
	{
		Da_cloth[i].elasticf = glm::vec3(0.f);
		Da_cloth[i].externalf = glm::vec3(0.f);
	}
}

void clothObj::computeElasticF()
{
	for (int i = 0; i < clothH; ++i)
	{
		for (int j = 0; j < clothW; ++j)
		{
		if (j+i*clothW - clothW >= 0)
		{
			glm::vec3 offset = (-Da_cloth[j + i*clothW].pos + Da_cloth[j + i*clothW - clothW].pos);
			Da_cloth[j + i*clothW].elasticf += hook*glm::normalize(offset)*(glm::length(offset) - 1.f);
		}
		if (j + i*clothW - 1 >= i*clothW)
		{
			glm::vec3 offset = (-Da_cloth[j + i*clothW].pos + Da_cloth[j + i*clothW - 1].pos);
			Da_cloth[j + i*clothW].elasticf += hook*glm::normalize(offset)*(glm::length(offset) - 1.f);
		}
		if (j + i*clothW + 1 < (i+1)*clothW)
		{
			glm::vec3 offset = (-Da_cloth[j + i*clothW].pos + Da_cloth[j + i*clothW + 1].pos);
			Da_cloth[j + i*clothW].elasticf += hook*glm::normalize(offset)*(glm::length(offset) - 1.f);
		}
		if (j + i*clothW + clothW < Da_cloth.size()&& j + i*clothW + clothW<(i+2)*clothW)
		{
			glm::vec3 offset = (-Da_cloth[j + i*clothW].pos + Da_cloth[j + i*clothW + clothW].pos);
			Da_cloth[j + i*clothW].elasticf += hook*glm::normalize(offset)*(glm::length(offset) - 1.f);
		}
		//=======================shear force ,a bit wierd for now
		//if (j + i*clothW - clothW - 1 >= 0 && j + i*clothW - clothW - 1 >= (i - 1)*clothW)
		//{
		//	glm::vec3 offset = (-Da_cloth[j + i*clothW].pos + Da_cloth[j + i*clothW - clothW - 1].pos);
		//	Da_cloth[j + i*clothW].elasticf += hook2*glm::normalize(offset)*(glm::length(offset) - Sqrt2);
		//}
		//if (j + i*clothW - clothW + 1 < i*clothW&&j + i*clothW - clothW + 1<Da_cloth.size())
		//{
		//	glm::vec3 offset = (-Da_cloth[j + i*clothW].pos + Da_cloth[j + i*clothW - clothW + 1].pos);
		//	Da_cloth[j + i*clothW].elasticf += hook2*glm::normalize(offset)*(glm::length(offset) - Sqrt2);
		//}
		//if (j + i*clothW + clothW - 1 >= (i + 1)*clothW&&j + i*clothW + clothW - 1<Da_cloth.size())
		//{
		//	glm::vec3 offset = (-Da_cloth[j + i*clothW].pos + Da_cloth[j + i*clothW + clothW - 1].pos);
		//	Da_cloth[j + i*clothW].elasticf += hook2*glm::normalize(offset)*(glm::length(offset) - Sqrt2);
		//}
		//if (j + i*clothW + clothW + 1 < (i + 2)*clothW&&j + i*clothW + clothW + 1 < Da_cloth.size())
		//{
		//	glm::vec3 offset = (-Da_cloth[j + i*clothW].pos + Da_cloth[j + i*clothW + clothW + 1].pos);
		//	Da_cloth[j + i*clothW].elasticf += hook2*glm::normalize(offset)*(glm::length(offset) - Sqrt2);
		//}
		}

	}
}

void clothObj::advect(float dt)
{
	for (int i = 0; i < Da_cloth.size(); ++i)
	{
		Da_cloth[i].vel += dt*(Da_cloth[i].elasticf + Da_cloth[i].externalf) / float(Da_cloth[i].m);
		Da_cloth[i].pos += dt*Da_cloth[i].vel;
	}

}

void clothObj::boundaryCondition()
{
	for (int i = 0; i < Da_cloth.size(); ++i)
	{
		glm::vec3 curPos = Da_cloth[i].pos;
		float len = glm::distance(curPos, sphereCentre);
		if (len < 5)
		{
			glm::vec3 tmp = (curPos - sphereCentre) * 5.f / float(len);
			curPos = tmp + sphereCentre;
			Da_cloth[i].pos = curPos;
			glm::vec3 ForeVel = Da_cloth[i].vel;
			glm::vec3 tangiental = glm::normalize(curPos - sphereCentre);
			Da_cloth[i].vel -= glm::dot(tangiental, ForeVel)*tangiental;
			//Da_cloth[i].vel = glm::vec3(0.f,0.f,0.f);
		}
	}
}

void clothObj::solverMainLoop(float dt, int framecount)
{
	this->framecount = framecount;
	update();
	computeExternalF(framecount);
	computeElasticF();
	advect(dt);
	boundaryCondition();
}

void clothObj::computeExternalF(int frames)
{
	for (int i = 0; i < Da_cloth.size(); ++i)
	{
		glm::vec3 windF = /*float(rand()%1)**/0.f*(1.f+(sinf((glm::radians(float(frames/8.f)))))/2.f)*glm::vec3(0, 0, 1);
		glm::vec3 dampingF = Da_cloth[i].vel*damping;
		Da_cloth[i].externalf = dealtaT*(Gconst*glm::vec3(0,1,0)*Da_cloth[i].m- dampingF+windF)/float(Da_cloth[i].m);
	}
}

