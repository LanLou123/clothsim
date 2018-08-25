#include "camera.h"


#include <iostream>


Camera::Camera():
    Camera(600, 600)
{
    look = glm::vec3(0,0,-1);
    up = glm::vec3(0,1,0);
    right = glm::vec3(1,0,0);
	enable_spherical = false;
	vertical = horizontal = dis = 0;
}

Camera::Camera(unsigned int w, unsigned int h):
    Camera(w, h, glm::vec3(0,0,10), glm::vec3(0,0,0), glm::vec3(0,1,0))
{
	enable_spherical =false;
	vertical = horizontal = dis = 0;
}

Camera::Camera(unsigned int w, unsigned int h, const glm::vec3 &e, const glm::vec3 &r, const glm::vec3 &worldUp):
    fovy(45),
    width(w),
    height(h),
    near_clip(0.1f),
    far_clip(1000),
    eye(e),
    ref(r),
    world_up(worldUp),
	enable_spherical(false),
	vertical(0),
	horizontal(0),
	dis(0)
{
    RecomputeAttributes();
}

Camera::Camera(const Camera &c):
    fovy(c.fovy),
    width(c.width),
    height(c.height),
    near_clip(c.near_clip),
    far_clip(c.far_clip),
    aspect(c.aspect),
    eye(c.eye),
    ref(c.ref),
    look(c.look),
    up(c.up),
    right(c.right),
    world_up(c.world_up),
    V(c.V),
    H(c.H)
{}


void Camera::RecomputeAttributes()
{
	if (!enable_spherical)
	{
		look = glm::normalize(ref - eye);
		right = glm::normalize(glm::cross(look, world_up));
		up = glm::cross(right, look);
	}
	else
	{
		SphericalTransformation();
	}

    float tan_fovy = tan(glm::radians(fovy/2));
    float len = glm::length(ref - eye);
    aspect = width/height;
    V = up*len*tan_fovy;
    H = right*len*aspect*tan_fovy;
	vertical = horizontal = dis = 0;
	sphericalTrans = glm::vec3(0, 0, 0);

}

void Camera::SphericalTransformation()
{
	glm::mat4 SphericalMat = glm::rotate(glm::mat4(1.f), glm::radians(horizontal), up)
		*glm::rotate(glm::mat4(1.f), glm::radians(vertical), right)
		*glm::translate(glm::mat4(1.f),/*sphericalTrans*/dis*look);
	ref = glm::vec3(SphericalMat*glm::vec4(ref, 1));
	eye = glm::vec3(SphericalMat*glm::vec4(eye, 1));
	eye += sphericalTrans;
	ref += sphericalTrans;
	look = glm::vec3(SphericalMat*glm::vec4(glm::normalize(ref - eye), 1));
	right = glm::vec3(SphericalMat*glm::vec4(glm::normalize(glm::cross(look, world_up)), 1));
	up = glm::vec3(SphericalMat*glm::vec4(glm::cross(right, look), 1));
}

glm::mat4 Camera::getViewProj()
{
    return glm::perspective(glm::radians(fovy), width / (float)height, near_clip, far_clip) * glm::lookAt(eye, ref, up);
}

void Camera::RotateAboutUp(float deg)
{
    glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(deg), up);
    ref = ref - eye;
    ref = glm::vec3(rotation * glm::vec4(ref, 1));
    ref = ref + eye;
    RecomputeAttributes();
}

void Camera::RotateFixup(float deg)
{
    glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(deg), glm::vec3(0,1,0));
    ref = ref - eye;
    ref = glm::vec3(rotation * glm::vec4(ref, 1));
    ref = ref + eye;
    RecomputeAttributes();
}

void Camera::RotateFixRight(float deg)
{
    glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(deg), right);
    ref = ref - eye;
    ref = glm::vec3(rotation * glm::vec4(ref, 1));
    ref = ref + eye;
    RecomputeAttributes();
}

void Camera::RotateAboutRight(float deg)
{
    glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(deg), right);
    ref = ref - eye;
    ref = glm::vec3(rotation * glm::vec4(ref, 1));
    ref = ref + eye;
    RecomputeAttributes();
}

void Camera::TranslateAlongLook(float amt)
{
    glm::vec3 translation = look * amt;
    eye += translation;
    ref += translation;
    RecomputeAttributes();
}

void Camera::TranslateAlongRight(float amt)
{
    glm::vec3 translation = right * amt;
    eye += translation;
    ref += translation;
    RecomputeAttributes();
}
void Camera::TranslateAlongUp(float amt)
{
    glm::vec3 translation = up * amt;
    eye += translation;
    ref += translation;
    RecomputeAttributes();
}

void Camera::rotateSphericalright(float amt)
{
	horizontal = amt;
	RecomputeAttributes();
}

void Camera::rotateSphericalUp(float amt)
{
	vertical = amt;
	RecomputeAttributes();
}

void Camera::translateSphericalRight(float amt)
{
	sphericalTrans += amt*glm::vec3(1,0,0);
	RecomputeAttributes();
}

void Camera::translateSphericalUp(float amt)
{
	sphericalTrans += amt*glm::vec3(0,1,0);
	RecomputeAttributes();
}

void Camera::zoomInOut(float amt)
{
	dis = amt;
	//sphericalTrans += amt*look;
	RecomputeAttributes();
}