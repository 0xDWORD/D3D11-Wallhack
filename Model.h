#pragma once
class Model
{
public:
	Model(int strideCount, int indexCount, int vertexWidth, int indexWidth, int shaderTextureFormat);
	bool isMatch(int matchLevel, int strideCount, int indexCount, int vertexWidth, int indexWidth, int shaderTextureFormat);
	~Model();
private:
	int strideCount, indexCount, vertexWidth, indexWidth, shaderTextureFormat;
};

