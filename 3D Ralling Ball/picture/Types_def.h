#pragma once

using namespace Math;

enum TIMER_TIMEUINT//����ʱ�䵥λ����˼
{
	TIMER_TIMEUNIT_MILLISECOND = 0,//����
	TIMER_TIMEUNIT_SECOND = 1,
};

typedef VECTOR3 COLOR3;


struct DirectionalLight
{
public:
	DirectionalLight()
	{
		ZeroMemory(this, sizeof(*this));//��һƬ�ڴ���������Ϊ��
//#define RtlZeroMemory(Destination,Length) memset((Destination),0,(Length))
//#define ZeroMemory RtlZeroMemory
		mSpecularIntensity = 1.0f;
		mDirection = VECTOR3(1.0f, 0, 0);
		mDiffuseIntensity = 0.5;
		mIsEnabled = TRUE;
	}

	BOOL operator==(DirectionalLight& Light)
	{
		//two memory fragments are identical
		if (memcmp(this, &Light, sizeof(Light)) == 0)//memcmp�ǱȽ��ڴ�����buf1��buf2��ǰcount���ֽڡ��ú����ǰ��ֽڱȽϵġ�
		{
			return TRUE;
		}
		return FALSE;
	}

	VECTOR3 mAmbientColor;//��Χ��ɫ
	float		mSpecularIntensity;//����ǿ��
	VECTOR3 mDiffuseColor;
	float			mDiffuseIntensity;//��ɢǿ��
	VECTOR3 mSpecularColor;	
	BOOL		mIsEnabled;
	VECTOR3 mDirection;
};

struct Material
{
	Material() 
	{
		ambient = { 0,0,0 };//AMBIENT��ʾ���ֹ������䵽�ò����ϣ������ܶ�η�������������ڻ����еĹ���ǿ�ȣ���ɫ����
		diffuse = { 1.0f,0,0 };//GL_DIFFUSE��ʾ�������䵽�ò����ϣ�������������γɵĹ���ǿ�ȣ���ɫ��
		specular = { 1.0f,1.0f,1.0f };//L_SPECULAR��ʾ�������䵽�ò����ϣ��������淴����γɵĹ���ǿ�ȣ���ɫ����
		specularSmoothLevel = 10;
	};
	VECTOR3	ambient;//��Χ������
	VECTOR3	diffuse;//��������ɢ
	VECTOR3	specular;//����
	UINT			specularSmoothLevel;
};

struct Vertex
{
	VECTOR3 pos;
	VECTOR4 color;
	VECTOR3 normal;//������
	VECTOR2 texcoord;//��������
};

struct VertexShaderOutput_Vertex//��Ӱ�����
{
	VECTOR4 posH;//homogenous position���λ��
	VECTOR4 color;
	VECTOR2 texcoord;

	//Gouraud shading don't need to pass down the posW and normalW to pixel shader
	//while Phong shading (per-pixel) need it
};

struct RasterizedFragment//դ����Ƭ
{
	UINT pixelX;
	UINT pixelY;
	VECTOR4 color;
	VECTOR2 texcoord;
	//VECTOR3 normal;
};

struct RenderPipeline_InitData
{
	UINT bufferWidth;//���������
	UINT bufferHeight;//�������߶�

	std::vector<float>*		pZBuffer;//depth buffer
	std::vector<VECTOR3>*	pOutColorBuffer;//output color buffer
};

struct RenderPipeline_DrawCallData
{
	std::vector<Vertex>*		pVertexBuffer;
	std::vector<UINT>*		pIndexBuffer;
	UINT offset;
	UINT VertexCount;//the number of vertex
};

//in OBJ file ,vertex info is composed of indices
struct OBJ_vertexInfoIndex//������Ϣ����
{
	OBJ_vertexInfoIndex()
	{
		vertexID = texcoordID = vertexNormalID = 0;
	};

	OBJ_vertexInfoIndex(int vID, int texcID, int vnID)
	{
		vertexID = vID;
		texcoordID = texcID;
		vertexNormalID = vnID;
	}

	inline BOOL operator==(OBJ_vertexInfoIndex const& v)const
	{
		if (vertexID == v.vertexID && texcoordID == v.texcoordID && vertexNormalID == v.vertexNormalID)
		{
			return TRUE;
		}
		return FALSE;
	}

	UINT vertexID;
	UINT texcoordID;
	UINT vertexNormalID;
};

struct Renderer_Color3ToConsolePixel
{
	COLOR3	color;
	BYTE			asciiChar;
	WORD		textAttr;
};

