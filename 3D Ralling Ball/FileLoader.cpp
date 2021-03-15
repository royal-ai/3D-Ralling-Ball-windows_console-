
/***********************************************************************

					Description : import obj and ppm file
					

************************************************************************/

#include "MyConsoleEngine.h"

/*******************************************************************
									INTERFACE
*********************************************************************/

BOOL IFileManager::ImportFile_OBJ//��ȡobj�ļ��е���˼
(std::string pFilePath, 
	std::vector<Vertex>& refVertexBuffer,
	std::vector<UINT>& refIndexBuffer)
	//��ȡobj�ļ���Ϊ�����֣�һ������һ���Զ�ȡȫ���Ķ��㣬������������ȵ�����
	//��һ�����ǽ���Щ��Ӧ�����ݷ��붥����
{
	std::fstream fileIn(pFilePath);
	if (!fileIn.good())
	{
		DEBUG_MSG1("Import OBJ : Open File failed!!");
		return FALSE;
	}

	std::vector<VECTOR3> pointList;//xyz buffer�������
	std::vector<VECTOR2> texcoordList;//texcoord buffer�����������
	std::vector<VECTOR3> VNormalList;//vertex normal buffer���������
	std::vector<OBJ_vertexInfoIndex> vertexInfoList;//indices combination������Ϣ����

														  //newly input string from file
	std::string currString;

	//,...............
	while (!fileIn.eof())
	{
		fileIn >> currString;

		//3d vertex : "v 1.0000000 0.52524242 5.12312345"
		if (currString == "v")//�ж�������Ϣ��vΪ�����嶥��
		{
			VECTOR3 currPoint(0, 0, 0);
			fileIn >> currPoint.x >> currPoint.y >> currPoint.z;
			pointList.push_back(currPoint);
		}

		//vertex normal "vn 1.0000000 0.52524242 5.12312345"
		if (currString == "vn")//���㷨��
		{
			VECTOR3 currNormal(0, 0, 0);
			fileIn >> currNormal.x >> currNormal.y >> currNormal.z;
			VNormalList.push_back(currNormal);
		}

		//texture coordinate "vt 1.0000000 0.0000000"
		if (currString == "vt")//��ͼ���궥��
		{
			VECTOR2 currTexCoord(0, 0);
			fileIn >> currTexCoord.x >> currTexCoord.y;
			texcoordList.push_back(currTexCoord);
		}

		//face : if this face is trangles(OBJ also support quads or curves etc..)
		//, the combination will be 
		//  vertex index  / texcoord index  /  vnormal index (for each vertex)
		// like "1/3/4" "5/6/7"
		if (currString == "f")//�� ��ʽ��"f ��������/uv������/��������"��
		{
			for (UINT i = 0;i < 3;++i)//һ��������
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

				tmpSStream << tmpString;//���޸ĺ���ַ�����������
				tmpSStream >> currVertex.vertexID >> currVertex.texcoordID >> currVertex.vertexNormalID;
				//��֮ǰ���ַ��������ȡ��ֵ
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
				//�����������Ϣ����list��
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
	refVertexBuffer.resize(vertexInfoList.size());//vv.resize(int n,element)��ʾ��������vv�Ĵ�СΪn��
	//���ݺ��ÿ��Ԫ�ص�ֵΪelement��Ĭ��Ϊ0
	//resize()��ı������������͵�ǰԪ�ظ���
	for (UINT i = 0;i < refVertexBuffer.size();++i)
	{
		Vertex tmpVertex = {};

		//several indices which can retrieve vertex information
		OBJ_vertexInfoIndex& indicesCombination = vertexInfoList.at(i);
		tmpVertex.pos = pointList.at(indicesCombination.vertexID);
		tmpVertex.normal = VNormalList.at(indicesCombination.vertexNormalID);
		tmpVertex.texcoord = texcoordList.at(indicesCombination.texcoordID);
		tmpVertex.color = VECTOR4(1.0f, 1.0f, 1.0f,1.0f);//ȫ��Ϊ��ɫ

		//.......
		refVertexBuffer.at(i) = (tmpVertex);//��ÿ�������Ϣ����һ��������
		//�൱��OpenGL��glVertexAttribPointer
	}

	return TRUE;
}


BOOL IFileManager::ImportFile_PPM(std::string filePath,
	UINT & outWidth, UINT & outHeight, std::vector<COLOR3>& outColorBuffer)//color3����һ����ά��Ϣ
	//��ppm�е���ɫ��Ϣ����
{
	std::ifstream fileIn(filePath,std::ios::binary);
	if (fileIn.good() == FALSE)
	{
		DEBUG_MSG1("Load PPM : File Open Failed!!");
		return FALSE;
	}

	//file head (some fxxking ascii string)
	UINT maxColorIntensity;

	//skip the file Format "P6/n"����������ʽ���ֽ���Ϣ
	fileIn.seekg(3);
	std::streampos filePos = fileIn.tellg();//tellg()�Ƿ��ص�ǰ�ļ�ָ���λ��
	fileIn >> outWidth>> outHeight>> maxColorIntensity;//maxcolorintersity���������ֵ

	//skip the '/r'
	fileIn.seekg(1, std::ios::cur);

	//read the main color chunk (binary)
	unsigned char* byteBuff=new unsigned char[outWidth*outHeight*3];
	fileIn.read((char*)byteBuff, 3 * outWidth*outHeight);//���ļ��и���3*outWidth*outHeight���ֽڵ�bytebuff��
	outColorBuffer.resize(outWidth*outHeight);
	fileIn.close();

	//output
	for (UINT i = 0;i < outWidth*outHeight;++i)
	{
		COLOR3 tmpColor;
		tmpColor = { byteBuff[3 * i]/255.0f,byteBuff[3 * i+1] / 255.0f,byteBuff[3 * i+2] / 255.0f };
		outColorBuffer.at(i) = tmpColor;
	}//����Ϣ����colorbuffer

	return TRUE;
}


