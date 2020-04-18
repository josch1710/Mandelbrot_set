#ifndef SAVE_IMAGE__H
#define SAVE_IMAGE__H

#include <vector>
#include <tuple>

#include "window.h"
#include <lib3mf_implicit.hpp>

template<typename T>
using height_vector = std::vector<T>;
template<typename T>
using height_matrix = std::vector<height_vector<T>>;

using namespace Lib3MF;

// Utility functions to create vertices and triangles
sLib3MFPosition fnCreateVertex(float x, float y, float z)
{
	sLib3MFPosition result;
	result.m_Coordinates[0] = x;
	result.m_Coordinates[1] = y;
	result.m_Coordinates[2] = z;
	return result;
}

template<typename T>
sLib3MFTriangle fnCreateTriangle(T v0, T v1, T v2)
{
	sLib3MFTriangle result;
	result.m_Indices[0] = v0;
	result.m_Indices[1] = v1;
	result.m_Indices[2] = v2;
	return result;
}

template<typename T>
void createRectangle(T x, T y, T z, PModel model, PWrapper wrapper, T xres, T yres)
{
	std::vector<sLib3MFPosition> vertices(8);
	std::vector<sLib3MFTriangle> triangles(12);

	float fSizeX = xres*1.0f;
	float fSizeY = yres*1.0f;
	float fSizeZ = z*1.0f;

	// Manually create vertices
	vertices[0] = fnCreateVertex(x+0.0f, y+0.0f, 5.0f);
	vertices[1] = fnCreateVertex(x+fSizeX, y+0.0f, 5.0f);
	vertices[2] = fnCreateVertex(x+fSizeX, y+fSizeY, 5.0f);
	vertices[3] = fnCreateVertex(x+0.0f, y+fSizeY, 5.0f);
	vertices[4] = fnCreateVertex(x+0.0f, y+0.0f, 5.0f+fSizeZ);
	vertices[5] = fnCreateVertex(x+fSizeX, y+0.0f, 5.0f+fSizeZ);
	vertices[6] = fnCreateVertex(x+fSizeX, y+fSizeY, 5.0f+fSizeZ);
	vertices[7] = fnCreateVertex(x+0.0f, y+fSizeY, 5.0f+fSizeZ);

	// Manually create triangles
	triangles[0] = fnCreateTriangle(2, 1, 0);
	triangles[1] = fnCreateTriangle(0, 3, 2);
	triangles[2] = fnCreateTriangle(4, 5, 6);
	triangles[3] = fnCreateTriangle(6, 7, 4);
	triangles[4] = fnCreateTriangle(0, 1, 5);
	triangles[5] = fnCreateTriangle(5, 4, 0);
	triangles[6] = fnCreateTriangle(2, 3, 7);
	triangles[7] = fnCreateTriangle(7, 6, 2);
	triangles[8] = fnCreateTriangle(1, 2, 6);
	triangles[9] = fnCreateTriangle(6, 5, 1);
	triangles[10] = fnCreateTriangle(3, 0, 4);
	triangles[11] = fnCreateTriangle(4, 7, 3);

	PMeshObject meshObject = model->AddMeshObject();
	meshObject->SetName("Box");
	meshObject->SetGeometry(vertices, triangles);
	model->AddBuildItem(meshObject.get(), wrapper->GetIdentityTransform());
}

template<typename T>
void plot(window<T> &scr, height_matrix<T> &heights, int iter_max, const char *fname) {
	static_assert(std::is_scalar<T>::value, "Type must be scalar");

	unsigned int width = scr.width(), height = scr.height();

	auto wrapper = CWrapper::loadLibrary();
	auto model = wrapper->CreateModel();
	model->SetUnit(eModelUnit::MilliMeter);

	int progress = -1;
	for(T i = scr.y_min(); i < scr.y_max(); i += scr.y_res())
	{
		for(T j = scr.x_min(); j < scr.x_max(); j += scr.x_res())
		{
			createRectangle<T>(j, i, heights[i][j], model, wrapper, scr.x_res(), scr.y_res());
		}
		if(progress < static_cast<int>(i*100.0/scr.y_max())){
			progress = static_cast<int>(i*100.0/scr.y_max());
			std::cout << progress << "%\n";
		}
	}

	// Write file
	PWriter writer = model->QueryWriter("3mf");
	writer->WriteToFile(fname);
}

#endif