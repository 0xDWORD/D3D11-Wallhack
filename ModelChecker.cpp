#include "ModelChecker.h"

ModelChecker::ModelChecker()
{
	createModels();
}


bool ModelChecker::findModel(int strideCount, int indexCount, int vertexWidth, int indexWidth, int shaderTextureFormat)
{
	if (models.size() == 0)
		return false;

	for (auto &model : models) 
	{
		if (model.isMatch(strideCount, indexCount, vertexWidth, indexWidth, shaderTextureFormat))
		{
			return true;
		}
	}

	return false;
}

void ModelChecker::createModels()
{
	models.push_back(Model(28, 64692, 129384, 345940, 10));
	models.push_back(Model(28, 64446, 128892, 343644, 10));
	models.push_back(Model(28, 36924, 73848, 196532, 10));
	models.push_back(Model(28, 3000, 6000, 345940, 77));
	models.push_back(Model(0, 6072, 199528, 71, 0));
	models.push_back(Model(0, 6480, 515956, 77, 273756160));
	models.push_back(Model(0, 6348, 15680, 28, 273756160));
	models.push_back(Model(0, 6348, 1747616, 28, 273756160));
	models.push_back(Model(0, 129384, 345940, 77, -716099584));
	models.push_back(Model(0, 129384, 345940, 10, 64692));
	models.push_back(Model(0, 129384, 345940, 10, 0));
}
