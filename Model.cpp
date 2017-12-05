#include "Model.h"

Model::Model(int strideCount, int indexCount, int vertexWidth, int indexWidth, int shaderTextureFormat)
{
	Model::strideCount = strideCount;
	Model::indexCount = indexCount;
	Model::vertexWidth = vertexWidth;
	Model::indexWidth = indexWidth;
	Model::shaderTextureFormat = shaderTextureFormat;
}


Model::~Model()
{
}

bool Model::isMatch(int strideCount, int indexCount, int vertexWidth, int indexWidth, int shaderTextureFormat) 
{
	if (Model::strideCount != strideCount)
		return false;
	if (Model::vertexWidth != vertexWidth)
		return false;
	if (Model::indexWidth != indexWidth)
		return false;
	if (Model::indexCount != indexCount)
		return false;
	if (Model::shaderTextureFormat != shaderTextureFormat)
		return false;
	return true;
}
