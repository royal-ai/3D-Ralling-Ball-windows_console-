
/***********************************************************************

					Description : import obj and ppm file
					

************************************************************************/

#include "MyConsoleEngine.h"

/*******************************************************************
									INTERFACE
*********************************************************************/

BOOL IFileManager::ImportFile_OBJ//读取obj文件有点意思
(std::string pFilePath, 
	std::vector<Vertex>& refVertexBuffer,
	std::vector<UINT>& refIndexBuffer)
	//读取obj文件分为两部分，一部分是一次性读取全部的顶点，法向量，纹理等等数据
	//另一部分是将这些相应的数据放入顶点中
{
	std::fstream fileIn(pFilePath);
	if (!fileIn.good())
	{
		DEBUG_MSG1("Import OBJ : Open File failed!!");
		return FALSE;
	}

	std::vector<VECTOR3> pointList;//xyz buffer点的数组
	std::vector<VECTOR2> texcoordList;//texcoord buffer纹理坐标组合
	std::vector<VECTOR3> VNormalList;//vertex normal buffer法向量组合
	std::vector<OBJ_vertexInfoIndex> vertexInfoList;//indices combination顶点信息索引

														  //newly input string from file
	std::string currString;

	//,...............
	while (!fileIn.eof())
	{
		fileIn >> currString;

		//3d vertex : "v 1.0000000 0.52524242 5.12312345"
		if (currString == "v")//判断有用信息，v为几何体顶点
		{
			VECTOR3 currPoint(0, 0, 0);
			fileIn >> currPoint.x >> currPoint.y >> currPoint.z;
			pointList.push_back(currPoint);
		}

		//vertex normal "vn 1.0000000 0.52524242 5.12312345"
		if (currString == "vn")//顶点法线
		{
			VECTOR3 currNormal(0, 0, 0);
			fileIn >> currNormal.x >> currNormal.y >> currNormal.z;
			VNormalList.push_back(currNormal);
		}

		//texture coordinate "vt 1.0000000 0.0000000"
		if (currString == "vt")//贴图坐标顶点
		{
			VECTOR2 currTexCoord(0, 0);
			fileIn >> currTexCoord.x >> currTexCoord.y;
			texcoordList.push_back(currTexCoord);
		}

		//face : if this face is trangles(OBJ also support quads or curves etc..)
		//, the combination will be 
		//  vertex index  / texcoord index  /  vnormal index (for each vertex)
		// like "1/3/4" "5/6/7"
		if (currString == "f")//面 格式："f 顶点索引/uv点索引/法线索引"。
		{
			for (UINT i = 0;i < 3;++i)//一行有三个
			{
				//static UINT currUniqueVertexCount = 0;
				OBJ_vertexInfoIndex currVertex = { 0,0,0 };
				std::stringstream tmpSStream;
				std::string tmpString;

				//then a "1/2/3" "4/1/3" like string will be input
				fileIn >> tmpString;

				//replace "/" with SPACE ,so STD::STRINGSTREAM will automatically format
				//the string into 3 separate part
				for (auto& c : tmpString) { if (c == '/')c = ' '; }

				tmpSStream << tmpString;//将修改后的字符串放入流中
				tmpSStream >> currVertex.vertexID >> currVertex.texcoordID >> currVertex.vertexNormalID;
				//从之前的字符串里面读取数值
				--currVertex.texcoordID;
				--currVertex.vertexID;
				--currVertex.vertexNormalID;

				//this will be an n^2 searching....optimization will be needed
				//non-existed element will be created
				BOOL IsVertexExist = FALSE;
				UINT  existedVertexIndex = 0;
				for (UINT j = 0;j<vertexInfoList.size();j++)
				{
					//in DEBUG mode ,[] operator will be a big performance overhead
					if (vertexInfoList[j] == currVertex)
					{
						IsVertexExist = TRUE;
						existedVertexIndex =j;
						break;
					}
				}
				//将点的索引信息加入list中
				if (!IsVertexExist)
				{
					vertexInfoList.push_back(currVertex);
					//the newest vertex.....
					refIndexBuffer.push_back(vertexInfoList.size() - 1);
				}
				else
				{
					refIndexBuffer.push_back(existedVertexIndex);
				}
			}
		}
	}

	fileIn.close();

	// All interested data are acquired, now convert to VB/IB
	refVertexBuffer.resize(vertexInfoList.size());//vv.resize(int n,element)表示调整容器vv的大小为n，
	//扩容后的每个元素的值为element，默认为0
	//resize()会改变容器的容量和当前元素个数
	for (UINT i = 0;i < refVertexBuffer.size();++i)
	{
		Vertex tmpVertex = {};

		//several indices which can retrieve vertex information
		OBJ_vertexInfoIndex& indicesCombination = vertexInfoList.at(i);
		tmpVertex.pos = pointList.at(indicesCombination.vertexID);
		tmpVertex.normal = VNormalList.at(indicesCombination.vertexNormalID);
		tmpVertex.texcoord = texcoordList.at(indicesCombination.texcoordID);
		tmpVertex.color = VECTOR4(1.0f, 1.0f, 1.0f,1.0f);//全部为白色

		//.......
		refVertexBuffer.at(i) = (tmpVertex);//将每个点的信息传入一个集合里
		//相当于OpenGL的glVertexAttribPointer
	}

	return TRUE;
}


BOOL IFileManager::ImportFile_PPM(std::string filePath,
	UINT & outWidth, UINT & outHeight, std::vector<COLOR3>& outColorBuffer)//color3就是一个三维信息
	//把ppm中的颜色信息存入
{
	std::ifstream fileIn(filePath,std::ios::binary);
	if (fileIn.good() == FALSE)
	{
		DEBUG_MSG1("Load PPM : File Open Failed!!");
		return FALSE;
	}

	//file head (some fxxking ascii string)
	UINT maxColorIntensity;

	//skip the file Format "P6/n"跳过三个格式的字节信息
	fileIn.seekg(3);
	std::streampos filePos = fileIn.tellg();//tellg()是返回当前文件指针的位置
	fileIn >> outWidth>> outHeight>> maxColorIntensity;//maxcolorintersity是最大像素值

	//skip the '/r'
	fileIn.seekg(1, std::ios::cur);

	//read the main color chunk (binary)
	unsigned char* byteBuff=new unsigned char[outWidth*outHeight*3];
	fileIn.read((char*)byteBuff, 3 * outWidth*outHeight);//从文件中复制3*outWidth*outHeight个字节到bytebuff中
	outColorBuffer.resize(outWidth*outHeight);
	fileIn.close();

	//output
	for (UINT i = 0;i < outWidth*outHeight;++i)
	{
		COLOR3 tmpColor;
		tmpColor = { byteBuff[3 * i]/255.0f,byteBuff[3 * i+1] / 255.0f,byteBuff[3 * i+2] / 255.0f };
		outColorBuffer.at(i) = tmpColor;
	}//将信息输入colorbuffer

	return TRUE;
}


