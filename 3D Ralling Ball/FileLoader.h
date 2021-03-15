#pragma once

class IFileManager
{
public:
	

	BOOL ImportFile_OBJ(std::string pFilePath,
		std::vector<Vertex>& refVertexBuffer,
		std::vector<UINT>& refIndexBuffer);

	BOOL ImportFile_PPM
	(std::string filePath,
		UINT& outWidth,
		UINT& outHeight,
		std::vector<COLOR3>& outColorBuffer);

	
private:

};