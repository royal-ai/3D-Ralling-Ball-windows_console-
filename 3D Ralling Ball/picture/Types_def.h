#pragma once

using namespace Math;

enum TIMER_TIMEUINT//就是时间单位的意思
{
	TIMER_TIMEUNIT_MILLISECOND = 0,//毫秒
	TIMER_TIMEUNIT_SECOND = 1,
};

typedef VECTOR3 COLOR3;


struct DirectionalLight
{
public:
	DirectionalLight()
	{
		ZeroMemory(this, sizeof(*this));//将一片内存区域设置为零
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
		if (memcmp(this, &Light, sizeof(Light)) == 0)//memcmp是比较内存区域buf1和buf2的前count个字节。该函数是按字节比较的。
		{
			return TRUE;
		}
		return FALSE;
	}

	VECTOR3 mAmbientColor;//周围颜色
	float		mSpecularIntensity;//反射强度
	VECTOR3 mDiffuseColor;
	float			mDiffuseIntensity;//分散强度
	VECTOR3 mSpecularColor;	
	BOOL		mIsEnabled;
	VECTOR3 mDirection;
};

struct Material
{
	Material() 
	{
		ambient = { 0,0,0 };//AMBIENT表示各种光线照射到该材质上，经过很多次反射后最终遗留在环境中的光线强度（颜色）。
		diffuse = { 1.0f,0,0 };//GL_DIFFUSE表示光线照射到该材质上，经过漫反射后形成的光线强度（颜色）
		specular = { 1.0f,1.0f,1.0f };//L_SPECULAR表示光线照射到该材质上，经过镜面反射后形成的光线强度（颜色）。
		specularSmoothLevel = 10;
	};
	VECTOR3	ambient;//周围，环绕
	VECTOR3	diffuse;//弥漫，扩散
	VECTOR3	specular;//镜子
	UINT			specularSmoothLevel;
};

struct Vertex
{
	VECTOR3 pos;
	VECTOR4 color;
	VECTOR3 normal;//法向量
	VECTOR2 texcoord;//纹理坐标
};

struct VertexShaderOutput_Vertex//阴影，输出
{
	VECTOR4 posH;//homogenous position齐次位置
	VECTOR4 color;
	VECTOR2 texcoord;

	//Gouraud shading don't need to pass down the posW and normalW to pixel shader
	//while Phong shading (per-pixel) need it
};

struct RasterizedFragment//栅格化碎片
{
	UINT pixelX;
	UINT pixelY;
	VECTOR4 color;
	VECTOR2 texcoord;
	//VECTOR3 normal;
};

struct RenderPipeline_InitData
{
	UINT bufferWidth;//缓冲区宽度
	UINT bufferHeight;//缓冲区高度

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
struct OBJ_vertexInfoIndex//顶点信息索引
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

