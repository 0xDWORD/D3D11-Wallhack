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

bool Model::isMatch(int matchLevel, int strideCount, int indexCount, int vertexWidth, int indexWidth, int shaderTextureFormat)
{
	switch (matchLevel)
	{
	case 0:
		if ((Model::strideCount == strideCount) && (Model::vertexWidth == vertexWidth) && (Model::indexWidth == indexWidth) && (Model::indexCount == indexCount) && (Model::shaderTextureFormat == shaderTextureFormat))
			return true;
		break;
	case 1:
		if ((Model::strideCount == strideCount) && (Model::vertexWidth == vertexWidth) && (Model::indexWidth == indexWidth) && (Model::indexCount == indexCount))
			return true;
		break;
	case 2:
		if ((Model::strideCount == strideCount) && (Model::vertexWidth == vertexWidth) && (Model::indexWidth == indexWidth))
			return true;
		break;
	case 3:
		if ((Model::strideCount == strideCount) && (Model::vertexWidth == vertexWidth))
			return true;
		break;
	case 4:
		if ((Model::strideCount == strideCount) && (Model::indexWidth == indexWidth))
			return true;
		break;
	case 5:
		if ((Model::strideCount == strideCount) && (Model::indexWidth == indexWidth) && (Model::indexCount == indexCount))
			return true;
		break;
	case 6:
		if ((Model::strideCount == strideCount) && (Model::indexCount == indexCount))
			return true;
		break;
	case 7:
		if ((Model::strideCount == strideCount) && (Model::indexWidth == indexWidth) && (Model::vertexWidth == vertexWidth))
			return true;
		break;
	case 8:
		if ((Model::indexWidth == indexWidth) && (Model::vertexWidth == vertexWidth))
			return true;
		break;
	case 9:
		if ((Model::indexWidth == indexWidth) && (Model::vertexWidth == vertexWidth) && (Model::shaderTextureFormat == shaderTextureFormat))
			return true;
		break;
	default:
		break;
	}

	return false;
}
