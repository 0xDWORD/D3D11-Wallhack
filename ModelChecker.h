#pragma once
#include "Model.h"
#include <vector>
using namespace std;

class ModelChecker
{
private:
	vector<Model> models;

public:
	ModelChecker();
	~ModelChecker();

	bool findModel(int strideCount, int indexCount, int vertexWidth, int indexWidth, int shaderTextureFormat);
	void createModels();

};

