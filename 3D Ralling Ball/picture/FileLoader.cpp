
/***********************************************************************

					Description : STL file Operation

************************************************************************/

#include "MyConsoleEngine.h"

/*******************************************************************
									INTERFACE
*********************************************************************/
BOOL IFileManager::ImportFile_STL(std::string pFilePath, 
	std::vector<VECTOR3>& refVertexBuffer, //顶点缓冲区？
	std::vector<UINT>& refIndexBuffer,
	std::vector<VECTOR3>& refNormalBuffer, 
	std::string & refFileInfo)
{
	std::ifstream tmpFile(pFilePath, std::ios::binary);//以二进制形式打开

	if (!tmpFile.good())
	{
		DEBUG_MSG1("Load STL : file Open Failed!!");//DEBUG_MSG1的意思是打开对话框并提问
		return FALSE;
	}

	//move file cursor to the end
	tmpFile.seekg(0, std::ios::end);//移动到文件尾
	std::streamoff static_fileSize = tmpFile.tellg();//tellg(),返回当前文件指针的streampos（字节数）
	if (static_fileSize < 84L)
	{
		DEBUG_MSG1("Load STL : file Damaged!!File Size is Too Small!!");
		return FALSE;
	}
	tmpFile.seekg(0);//将文件指针移回零

	char firstChar;
	tmpFile.read(&firstChar, 1);
	tmpFile.close();

	//ASCII STL starts with "solid"
	if (firstChar == 's')
	{
		return mFunction_ImportFile_STL_Ascii
		(pFilePath, refVertexBuffer, refIndexBuffer, refNormalBuffer, refFileInfo);
	}
	else
	{
		return mFunction_ImportFile_STL_Binary
		(pFilePath, refVertexBuffer, refIndexBuffer, refNormalBuffer, refFileInfo);
	}//它stl文件放哪了
}

BOOL IFileManager::ImportFile_OBJ//读取obj文件有点意思
(std::string pFilePath, 
	std::vector<Vertex>& refVertexBuffer,
	std::vector<UINT>& refIndexBuffer)
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
		tmpVertex.color = VECTOR4(1.0f, 1.0f, 1.0f,1.0f);

		//.......
		refVertexBuffer.at(i) = (tmpVertex);//将每个点的信息传入一个集合里
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

BOOL IFileManager::ExportFile_PPM(std::string filePath, 
	UINT width, UINT height, const std::vector<COLOR3>& colorBuffer, BOOL bOverrideExistedFile)
{
	std::ofstream fileOut;
	if (bOverrideExistedFile == TRUE)
	{
		//trunc to 0 size
		fileOut.open(filePath.c_str(), std::ios::trunc);//如果文件存在则截断文件
	}
	else
	{
		fileOut.open(filePath.c_str(), std::ios::_Noreplace);//如果文件不存在则建立新文件,如果文件以存在则操作失败
	};

	if(fileOut.good()==FALSE)
	{
		DEBUG_MSG1("Save PPM : File Open Failed!!");
		return FALSE;
	}

	//Write File Head
	fileOut << "P6" << std::endl;
	fileOut << width << std::endl;
	fileOut << height << std::endl;
	fileOut << 255 << std::endl;
	for (auto& c : colorBuffer)
	{
		unsigned char tmpByteColor[3] = { BYTE(c.x * 255),BYTE(c.y * 255), BYTE(c.z * 255) };
		fileOut.write((char*)&tmpByteColor[0], 3);
	}
	fileOut.close();

	return TRUE;
}


/*******************************************************************
										PRIVATE
*********************************************************************/
BOOL IFileManager::mFunction_ImportFile_STL_Binary(std::string pFilePath, std::vector<VECTOR3>& refVertexBuffer,
	std::vector<UINT>& refIndexBuffer, std::vector<VECTOR3>& refNormalBuffer, std::string& refFileInfo)
{
	std::ifstream fileIn(pFilePath, std::ios::binary);

	if (!fileIn.good())
	{
		DEBUG_MSG1("Load STL Binary : Open File Failed!");
		return FALSE;
	}

	/*STL: Baidu encyclopedia

	binary STL use fixed length of bit patterns to store vertex information,
	At the beginning, 80 bytes of header will be some custom info,
	Right behind the header , next 4 bytes will be the total number of triangles;

	the rest of the file will represent every triangles in 50 bytes blocks:

	3xfloat =12bytes ------- Facet Normal
	3xfloat =12bytes ------- Vertex1
	3xfloat =12bytes ------- Vertex2
	3xfloat =12bytes ------- Vertex3
	2 byte ------ face attribute info (I don't know what's the use...)

	the length of a complete STL will be 50 *(triangleCount) + 84  */


	//newly input string
	char headerInfo[81] = {};
	fileIn.read(headerInfo, 80);

	//the first char could not be "s" in BINARY mode , in order to distinguish form ASCII mode,
	//which starts with "solid"
	if (headerInfo[0] == 's')
	{
		DEBUG_MSG1("Load STL Binary : File Damaged!! It's not binary STL file!");
		return FALSE;
	}

	refFileInfo = headerInfo;
	//move reading cursor
	fileIn.seekg(80);

	//read bit patterns, and reinterpret the data to interested type
	//using decltype()
#define REINTERPRET_READ(var) \
	fileIn.read(dataBlock,sizeof(var));\
	var=*(decltype(var)*)(void*)dataBlock;\

	//a char array used to store bit pattern (used in REINTERPRET_READ)
	char dataBlock[5] = {};

	uint32_t triangleCount = 0;
	REINTERPRET_READ(triangleCount);

	if (triangleCount > 500000)
	{
		DEBUG_MSG1("Load STL Binary : Triangle Count is larger than 500000 / Data Damamged!!");
		fileIn.close();
		return FALSE;
	}

	//then reserve spaces for vectors (optimization)
	refVertexBuffer.reserve(triangleCount * 3);
	refNormalBuffer.reserve(triangleCount);
	refIndexBuffer.reserve(triangleCount * 3);

	while (!fileIn.eof())
	{

		//a facet normal
		VECTOR3 tmpVec3(0, 0, 0);
		REINTERPRET_READ(tmpVec3.x);
		REINTERPRET_READ(tmpVec3.y);
		REINTERPRET_READ(tmpVec3.z);
		refNormalBuffer.push_back(tmpVec3);

		//3 vertices
		for (UINT i = 0;i < 3;i++)
		{
			REINTERPRET_READ(tmpVec3.x);
			REINTERPRET_READ(tmpVec3.z);
			REINTERPRET_READ(tmpVec3.y);
			refVertexBuffer.push_back(tmpVec3);
		}

		uint16_t faceAttr = 0;
		REINTERPRET_READ(faceAttr);
	}

	//clockwise or counterClockwise
	for (UINT i = 0;i < refVertexBuffer.size();i += 3)
	{
		std::swap(refVertexBuffer[i + 1], refVertexBuffer[i + 2]);
	}

	//without optimization, vertices can be overlapped
	refIndexBuffer.resize(refVertexBuffer.size());
	for (UINT i = 0;i < refIndexBuffer.size();i++)
	{
		refIndexBuffer.at(i) = i;
	}


	fileIn.close();

	return TRUE;
}

BOOL IFileManager::mFunction_ImportFile_STL_Ascii(std::string pFilePath, std::vector<VECTOR3>& refVertexBuffer, std::vector<UINT>& refIndexBuffer, std::vector<VECTOR3>& refNormalBuffer, std::string& refFileInfo)
{
	std::ifstream fileIn(pFilePath);

	if (!fileIn.good())
	{
		DEBUG_MSG1("Load STL Ascii : Open File Failed!!");
		return FALSE;
	}

	//newly input string
	std::string currString;

	//object Name
	std::string objectName;

	//the first line (object name) need to be dealt with specially
	fileIn >> currString;

	//the beginning of the 
	if (currString == "solid")
	{
		//the name of the object could be null, thus input the first line instead of
		//input next string will be safer
		char pFirstLineString[80] = {};
		fileIn.getline(pFirstLineString, 80);
		objectName = pFirstLineString;

		//delete the space at the front
		if (objectName.size()> 0)objectName.erase(objectName.begin());
	}
	else
	{
		DEBUG_MSG1("Load STL Ascii : file damaged!!");
		return FALSE;
	}

	refFileInfo = objectName;


	//loop reading
	while (!fileIn.eof())
	{
		fileIn >> currString;

		if (currString == "endsolid")break;

		//"facet normal" + x+y+z
		if (currString == "normal")
		{
			VECTOR3 tmpFaceNormal(0, 0, 0);
			fileIn >> tmpFaceNormal.x >> tmpFaceNormal.y >> tmpFaceNormal.z;
			//face normal (may be used as vertex normal)
			refNormalBuffer.push_back(tmpFaceNormal);
		}

		//"vertex" +x +y+z
		if (currString == "vertex")
		{
			VECTOR3 tmpPoint(0, 0, 0);
			fileIn >> tmpPoint.x >> tmpPoint.z >> tmpPoint.y;
			refVertexBuffer.push_back(tmpPoint);
		}
	}

	//clockwise or counterClockwise
	for (UINT i = 0;i < refVertexBuffer.size();i += 3)
	{
		std::swap(refVertexBuffer[i + 1], refVertexBuffer[i + 2]);
	}


	fileIn.close();

	//without optimization, vertices can be overlapped
	refIndexBuffer.resize(refVertexBuffer.size());
	for (UINT i = 0;i < refIndexBuffer.size();i++)
	{
		refIndexBuffer.at(i) = i;
	}

	return TRUE;
}

