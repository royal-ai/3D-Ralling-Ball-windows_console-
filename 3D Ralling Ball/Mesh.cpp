
/***********************************************************************

							�ࣺNoiseMesh

				��������װ��һ�������࣬��SceneManager������   

***********************************************************************/


#include "MyConsoleEngine.h"

using namespace Math;

static UINT c_VBstride_Default = sizeof(Vertex);		//VertexBuffer��ÿ��Ԫ�ص��ֽڿ��
static UINT c_VBoffset = 0;				//VertexBuffer�������ƫ�� ��Ϊ��ͷ��ʼ����offset��0

IMesh::IMesh()//��ʼ�����캯��
{
	mRotationX_Pitch = 0.0f;
	mRotationY_Yaw = 0.0f;
	mRotationZ_Roll = 0.0f;

	m_pVB_Mem		= new std::vector<Vertex>;//�����Ա
	m_pIB_Mem			= new std::vector<UINT>;//�����������Ա

	mMatrixWorld.Identity();//mMatrixWorld��һ���ĳ��ľ��󣬽��䵥λ��

	Material defaultMat;//Ĭ�ϵ��������
	defaultMat.ambient = { 0.2f,0.2f,0.2f };
	defaultMat.diffuse	= {0.8f,0.8f,0.8f };
	defaultMat.specular = { 1.0f,1.0f,1.0f };
	defaultMat.specularSmoothLevel = 20;
	IMesh::SetMaterial(defaultMat);

	m_pTexture = nullptr;
}

IMesh::~IMesh()
{
};

void IMesh::Destroy()
{
	delete m_pVB_Mem;
	delete m_pIB_Mem;
};

void	IMesh::CreatePlane(float fWidth,float fDepth,UINT iRowCount,UINT iColumnCount)
{
	//check if the input "Step Count" is illegal
	if(iColumnCount <= 2)	{iColumnCount =2;}
	if(iRowCount <= 2)		{iRowCount = 2;}

	m_pVB_Mem->clear();
	m_pIB_Mem->clear();

	//delegate vert/idx creation duty to MeshGenerator 
	mMeshGenerator.CreatePlane(fWidth, fDepth, iRowCount, iColumnCount, *m_pVB_Mem, *m_pIB_Mem);

};

void IMesh::CreateBox(float fWidth,float fHeight,float fDepth,UINT iDepthStep,UINT iWidthStep,UINT iHeightStep)
{
	m_pVB_Mem->clear();
	m_pIB_Mem->clear();

	//mesh creation delegate to MeshGenerator
	mMeshGenerator.CreateBox(fWidth, fHeight, fDepth, iDepthStep, iWidthStep, iHeightStep, *m_pVB_Mem, *m_pIB_Mem);
}

void	IMesh::CreateSphere(float fRadius,UINT iColumnCount, UINT iRingCount, BOOL bInvertNormal)
{
	//check if the input "Step Count" is illegal
	if(iColumnCount <= 3)	{iColumnCount =3;}
	if(iRingCount <= 1)		{iRingCount = 1;}

	m_pVB_Mem->clear();
	m_pIB_Mem->clear();

	//mesh creation delegate to MeshGenerator
	mMeshGenerator.CreateSphere(fRadius, iColumnCount, iRingCount,bInvertNormal, *m_pVB_Mem, *m_pIB_Mem);

};

void IMesh::CreateCylinder(float fRadius,float fHeight,UINT iColumnCount,UINT iRingCount)
{
		//check if the input "Step Count" is illegal
	if(iColumnCount <= 3)	{iColumnCount =3;}
	if(iRingCount <= 2)		{iRingCount = 2;}

	m_pVB_Mem->clear();
	m_pIB_Mem->clear();

	//mesh creation delegate to MeshGenerator
	mMeshGenerator.CreateCylinder(fRadius,fHeight, iColumnCount, iRingCount, *m_pVB_Mem, *m_pIB_Mem);

};



BOOL IMesh::LoadFile_OBJ(std::string pFilePath)
{
	//check if buffers have been created
	m_pVB_Mem->clear();
	m_pIB_Mem->clear();


	std::vector<Vertex> tmpCompleteVertexList;
	std::vector<UINT> tmpNormalList;

	//����STL
	BOOL fileLoadSucceeded = FALSE;
	fileLoadSucceeded = IFileManager::ImportFile_OBJ(pFilePath, *m_pVB_Mem, *m_pIB_Mem);
	if (!fileLoadSucceeded)
	{
		DEBUG_MSG1("Noise Mesh : Load OBJ failed!");
		return FALSE;
	}

	return TRUE;
}

void IMesh::SetColor(const VECTOR4& color)
{
	for (auto& v : *m_pVB_Mem)
	{
		v.color = color;
	}
}

void IMesh::SetMaterial(const Material & mat)
{
	mMaterial = mat;
}

void IMesh::SetTexture(IPicture * pTex)
{
	m_pTexture = pTex;
}

void IMesh::SetPosition(float x,float y,float z)
{
	mPosition.x = x;
	mPosition.y = y;
	mPosition.z = z;
}

void IMesh::SetPosition(const VECTOR3 & pos)
{
	mPosition = pos;
}

void IMesh::SetRotation(float angleX, float angleY, float angleZ)
{
	mRotationX_Pitch	= angleX;
	mRotationY_Yaw		= angleY;
	mRotationZ_Roll		= angleZ;
}

void IMesh::SetRotationX_Pitch(float angleX)
{
	mRotationX_Pitch = angleX;
};

void IMesh::SetRotationY_Yaw(float angleY)
{
	mRotationY_Yaw = angleY;
};

void IMesh::SetRotationZ_Roll(float angleZ)
{
	mRotationZ_Roll = angleZ;
}

VECTOR3 IMesh::GetPosition()
{
	return mPosition;
}

void IMesh::RotateX_Pitch(float angle)
{
	SetRotationX_Pitch(mRotationX_Pitch + angle);
}

void IMesh::RotateY_Yaw(float angle)
{
	SetRotationY_Yaw(mRotationY_Yaw + angle);
}

void IMesh::RotateZ_Roll(float angle)
{
	SetRotationZ_Roll(mRotationZ_Roll + angle);
}

float IMesh::GetRotationY_Yaw()
{
	return mRotationY_Yaw;
}

float IMesh::GetRotationX_Pitch()
{
	return mRotationX_Pitch;
}

float IMesh::GetRotationZ_Roll()
{
	return mRotationZ_Roll;
;
}

void IMesh::GetWorldMatrix(MATRIX4x4 & outMat) 
{
	mFunction_UpdateWorldMatrix();
	outMat = mMatrixWorld;
}

UINT IMesh::GetVertexCount()
{
	return m_pVB_Mem->size();
}

void IMesh::GetVertex(UINT iIndex, Vertex& outVertex)
{
	if (iIndex < m_pVB_Mem->size())
	{
		outVertex = m_pVB_Mem->at(iIndex);
	}
}

void IMesh::GetVertexBuffer(std::vector<Vertex>& outBuff)
{
	std::vector<Vertex>::iterator iterBegin, iterLast;
	iterBegin = m_pVB_Mem->begin();
	iterLast = m_pVB_Mem->end();
	outBuff.assign(iterBegin,iterLast);
}

void IMesh::ComputeBoundingBox(BOUNDINGBOX& outBox)
{
	mFunction_ComputeBoundingBox();
	outBox= mBoundingBox;
}


/***********************************************************************
								PRIVATE					                    
***********************************************************************/

void	IMesh::mFunction_UpdateWorldMatrix()
{
	MATRIX4x4	tmpMatrixScaling;
	MATRIX4x4	tmpMatrixTranslation;
	MATRIX4x4	tmpMatrixRotation;
	MATRIX4x4	tmpMatrix;

	//��ʼ���������
	tmpMatrix.Identity();
		
	//��ת����
	tmpMatrix=Matrix_YawPitchRoll(mRotationY_Yaw, mRotationX_Pitch, mRotationZ_Roll);
	
	//�޸�ƽ����
	tmpMatrix.m[0][3] = mPosition.x;
	tmpMatrix.m[1][3] = mPosition.y;
	tmpMatrix.m[2][3] = mPosition.z;

	mMatrixWorld = tmpMatrix;
}

void IMesh::mFunction_ComputeBoundingBox()
{
	mFunction_UpdateWorldMatrix();
	//�����Χ��.......����1

	UINT i = 0;
	VECTOR4 tmpV;
	//�������ж��㣬�����Χ��3�������� С/�� ����������
	for (i = 0;i < m_pVB_Mem->size();i++)
	{
		if (i == 0)
		{
			mBoundingBox.min = m_pVB_Mem->at(0).pos;
			mBoundingBox.max = m_pVB_Mem->at(0).pos;
		}

		//N_DEFAULT_VERTEX
		tmpV = VECTOR4(m_pVB_Mem->at(i).pos.x, m_pVB_Mem->at(i).pos.y, m_pVB_Mem->at(i).pos.z, 1.0f);
		//tmpV = Math::Matrix_Multiply(mMatrixWorld, VECTOR4(tmpV.x,tmpV.y,tmpV.z,1.0f));
		if (tmpV.x <(mBoundingBox.min.x)) { mBoundingBox.min.x = tmpV.x; }
		if (tmpV.y <(mBoundingBox.min.y)) { mBoundingBox.min.y = tmpV.y; }
		if (tmpV.z <(mBoundingBox.min.z)) { mBoundingBox.min.z = tmpV.z; }

		if (tmpV.x >(mBoundingBox.max.x)) { mBoundingBox.max.x = tmpV.x; }
		if (tmpV.y >(mBoundingBox.max.y)) { mBoundingBox.max.y = tmpV.y; }
		if (tmpV.z >(mBoundingBox.max.z)) { mBoundingBox.max.z = tmpV.z; }
	}

	mBoundingBox.max += mPosition;
	mBoundingBox.min += mPosition;
}

void IMesh::mFunction_ComputeBoundingBox(std::vector<VECTOR3>* pVertexBuffer)
{
	mFunction_UpdateWorldMatrix();
	//�����Χ��.......����1

	UINT i = 0;
	VECTOR4 tmpV;
	//�������ж��㣬�����Χ��3�������� С/�� ����������
	for (i = 0;i < pVertexBuffer->size();i++)
	{
		if (i == 0)
		{
			mBoundingBox.min = pVertexBuffer->at(0);
			mBoundingBox.max = pVertexBuffer->at(0);
		}

		//N_DEFAULT_VERTEX
		tmpV = VECTOR4(pVertexBuffer->at(i).x, pVertexBuffer->at(i).y, pVertexBuffer->at(i).z, 1.0f);
		//tmpV = Math::Matrix_Multiply(mMatrixWorld, VECTOR4(tmpV.x,tmpV.y,tmpV.z,1.0f));
		if (tmpV.x <(mBoundingBox.min.x)) { mBoundingBox.min.x = tmpV.x; }
		if (tmpV.y <(mBoundingBox.min.y)) { mBoundingBox.min.y = tmpV.y; }
		if (tmpV.z <(mBoundingBox.min.z)) { mBoundingBox.min.z = tmpV.z; }

		if (tmpV.x >(mBoundingBox.max.x)) { mBoundingBox.max.x = tmpV.x; }
		if (tmpV.y >(mBoundingBox.max.y)) { mBoundingBox.max.y = tmpV.y; }
		if (tmpV.z >(mBoundingBox.max.z)) { mBoundingBox.max.z = tmpV.z; }
	}

	mBoundingBox.max += mPosition;
	mBoundingBox.min += mPosition;
}


inline VECTOR2 IMesh::mFunction_ComputeTexCoord_SphericalWrap(VECTOR3 vBoxCenter, VECTOR3 vPoint)
{
	//��...����������򵥵��������ΰ���

	VECTOR2 outTexCoord(0,0);
	VECTOR3 tmpP= vPoint - vBoxCenter;

	//ͶӰ����λ����
	tmpP.Normalize();

	//�����Ǻ�����������ϵ���꣬Ȼ��Ƕ�ֵӳ�䵽[0,1]
	float angleYaw = 0.0f;
	float anglePitch = 0.0f;
	float tmpLength = sqrtf(tmpP.x*tmpP.x + tmpP.z*tmpP.z);

	// [ -PI/2 , PI/2 ]
	anglePitch = atan2(tmpP.y,tmpLength);

	// [ -PI	, PI ]
	angleYaw =	atan2(tmpP.z, tmpP.x);	

	//map to [0,1]
	outTexCoord.x = (angleYaw +  Math::CONST_PI) / (2.0f * Math::CONST_PI);
	outTexCoord.y = (anglePitch + (Math::CONST_PI /2.0f) ) / Math::CONST_PI;

	return outTexCoord;
};
