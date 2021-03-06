
/************************************************************

				Renderer-Interfaces exposed to user

************************************************************/

#include "MyConsoleEngine.h"

IRenderer::IRenderer()
{
	for (UINT i = 0;i < 8;i++)m_pPalette[i] = new std::vector<Renderer_Color3ToConsolePixel>;//一个包含rgb颜色和控制台颜色的组合
	mFunction_GeneratePalette();//设置调色板
	m_pConsoleWindowTitle = new std::string;
}

IRenderer::~IRenderer()
{
	CloseHandle(m_hScreenBuffer);
	CloseHandle(m_hScreenOutput);
	delete m_pCharBuffer;
	delete m_pZBuffer;
	for (UINT i = 0;i < 8;i++)
	{
		m_pPalette[i]->clear();
		delete m_pPalette[i];
	}

}

void IRenderer::Init(UINT bufferWidth, UINT bufferHeight)
{
	//Get the std stream output handle
	m_hScreenOutput = ::GetStdHandle(STD_OUTPUT_HANDLE);//全局作用域

	//-----------------FONT-----------------
	//That is to say, we can adjust the real pixel size of console "pixel"!!!!!!!!!!!!!!!!!!!!
	//I can even make it square!!!!!
	CONSOLE_FONT_INFOEX fontInfo = { 0 };
	::GetCurrentConsoleFontEx(m_hScreenOutput, FALSE, &fontInfo);
	fontInfo.cbSize = sizeof(fontInfo);
	fontInfo.dwFontSize = { c_ConsoleCharSizeX, c_ConsoleCharSizeY };
	fontInfo.FontWeight = 700;	//700才能显示	//range from 100~1000, 400 is normal while >400 is bold
	::SetCurrentConsoleFontEx(m_hScreenOutput, FALSE, &fontInfo);
	//这里就是设置像素大小咯

	//Create a Screen buffer (we use double-buffer technique to 
	//present data)(But I don't know if there is a swap chain?????)双缓冲吗？？
	m_hScreenBuffer = ::CreateConsoleScreenBuffer(
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		CONSOLE_TEXTMODE_BUFFER,
		NULL
		);

	//hide cursors
	CONSOLE_CURSOR_INFO cci;
	cci.bVisible = 0;
	cci.dwSize = 1;
	::SetConsoleCursorInfo(m_hScreenOutput, &cci);
	::SetConsoleCursorInfo(m_hScreenBuffer, &cci);


	//----------INITIALIZATION OF BUFFERS-----------------
	BOOL isSuceeded = FALSE;

	COORD maxWindowSize = ::GetLargestConsoleWindowSize(m_hScreenOutput);//得到屏幕的宽和高
	mBufferWidth = SHORT(bufferWidth) > maxWindowSize.X ? maxWindowSize.X : bufferWidth;
	mBufferHeight = SHORT(bufferHeight) > maxWindowSize.Y ? maxWindowSize.Y : bufferHeight;
	
	
	//set the size of buffer
	COORD dwBuffSize;
	dwBuffSize.X = SHORT(bufferWidth);
	dwBuffSize.Y = SHORT(bufferHeight);
	isSuceeded = ::SetConsoleScreenBufferSize(m_hScreenBuffer, dwBuffSize);//设置缓冲区大小

	m_pCharBuffer = new std::vector<char>(mBufferHeight*mBufferWidth);
	m_pZBuffer = new std::vector<float>(mBufferHeight*mBufferWidth);
	m_pColorBuffer = new std::vector<COLOR3>(mBufferHeight*mBufferWidth);
	m_pTextAttrBuffer = new std::vector<WORD>(mBufferHeight*mBufferWidth);

	//set the buffer as an active buffer
	::SetConsoleActiveScreenBuffer(m_hScreenBuffer);
	
	//----------------------------INIT RENDER PIPELINE-----------------------
	RenderPipeline_InitData initData;
	initData.bufferWidth = mBufferWidth;
	initData.bufferHeight = mBufferHeight;
	initData.pOutColorBuffer = m_pColorBuffer;
	initData.pZBuffer = m_pZBuffer;
	IRenderPipeline3D::Init(initData);
	
}

void IRenderer::Clear(COLOR3 clearColor,BOOL clearZBuff)
{
	//clear with SPACE; buffer is a continuous memory block, but it should
	//be regarded as a 2D buffer
	for (UINT i = 0;i < mBufferWidth*mBufferHeight;++i)
	{
		//clear the RGB buffer
		m_pColorBuffer->at(i) = clearColor;//清除，填充吧
	}

	//Depth Buffer
	if (clearZBuff == TRUE)
	{
		for (UINT i = 0;i < mBufferWidth*mBufferHeight;++i)
		{
			m_pZBuffer->at(i) = 1.0f;
		}
	}
}

void IRenderer::SetCamera(ICamera & cam)
{
	m_pCamera = &cam;
}

void IRenderer::SetLight(UINT index, const DirectionalLight & light)
{
	IRenderPipeline3D::SetLight(index, light);
}

BOOL IRenderer::DrawPicture(IPicture & pic, UINT x1, UINT y1, UINT x2, UINT y2)
{
	x1 = Clamp(x1, 0, mBufferWidth-1);
	x2 = Clamp(x2, 0, mBufferWidth-1);
	y1 = Clamp(y1, 0, mBufferHeight-1);
	y2 = Clamp(y2, 0, mBufferHeight-1);

	//x1和y1是缓冲区起点
	if (x1 >= x2 || y1 >= y2)
	{
		DEBUG_MSG1("Render Picture: region info error!");
		return FALSE;
	}

	UINT drawRegionWidth = x2 - x1+1;
	UINT drawRegionHeight = y2 - y1+1;//刻画的图片的宽高

	//because Minification/Magnification could occur, we must find the 
	//x/y ratio coord first to filter
	for (UINT i = x1;i <= x2;++i)
	{
		float x_ratio = float(i - x1) / drawRegionWidth;
		for (UINT j = y1;j <= y2;++j)
		{
			float y_ratio = float(j - y1) / drawRegionHeight;
			UINT picCoordX = UINT(pic.mWidth*x_ratio);
			UINT picCoordY = UINT(pic.mHeight*y_ratio);
			mFunction_SetPixel(i, j, pic.GetPixel(picCoordX, picCoordY));
		}
	}
	return TRUE;
}

void IRenderer::DrawLine(COLOR3 color, UINT x1, UINT y1, UINT x2, UINT y2)
{
	auto fractionPart = [](float f)->float 
	{
		return f - float(UINT(f));
	};//返回一个数的小数部分

	if (x1==x2)
	{
		//if slope doesn't exist
		for (UINT j = y1;j <= y2;j++)
		{
				mFunction_SetPixel(x1, j, color);//pixel outside the boundary won't be drawn
				//设置缓冲区中的像素点的颜色形成直线
		}
	}
	else
	{
		//.........It should a signed distance
		float	k = float(int(y2)-int(y1)) / float(int(x2)-int(x1));
		float k_inv = 1 / k;
		//bresenham-like line drawing

		//two circumstances
		//1. abs(slope)>=1; 
		//2. abs(slope)<=1;
		float offset = 0.0f;
		if (abs(k) <= 1.0f)
		{
			UINT i = x1;

			while(i!=x2)
			{
				//UINT() forced type conversion will truncate the fraction part
				mFunction_SetPixel(i, y1 + UINT(offset),color);//UINT可以截断小数部分

				//anti-alising
				//mFunction_BlendPixel(i, y1 + UINT(offset)+ 1,  fractionPart(offset),color);
				offset += k;//dy = dx * k;

				if (x2 > x1)++i;else --i;
			}
		}
		else
		{
			UINT j = y1;
			while(j!=y2)
			{
				mFunction_SetPixel(x1+UINT(offset), j, color);

				//anti-alising
				//mFunction_BlendPixel(x1+UINT(offset) +1,j, fractionPart(offset),color);
				offset += (k_inv);

				if (y2 > y1) 
					{ ++j; }
				else
					{ --j; }
			}
		}
	}

}

void IRenderer::DrawTriangle(COLOR3 color, const VECTOR2 & v1_pixel, const VECTOR2 & v2_pixel, const VECTOR2 & v3_pixel)
{
	//matrix set to identity
	MATRIX4x4 matIdentity;
	matIdentity.Identity();
	IRenderPipeline3D::SetWorldMatrix(matIdentity);
	IRenderPipeline3D::SetProjMatrix(matIdentity);
	IRenderPipeline3D::SetViewMatrix(matIdentity);

	//convert to pixel space
	auto convertToHomoSpace = [&](const VECTOR2& v, VECTOR2& outV)
	{
		outV.x = (v.x / float(mBufferWidth) *2.0f) - 1.0f;
		outV.y = (-v.y / float(mBufferHeight) *2.0f) + 1.0f;
	};

	VECTOR2 v1, v2, v3;
	convertToHomoSpace(v1_pixel, v1);
	convertToHomoSpace(v2_pixel, v2);
	convertToHomoSpace(v3_pixel, v3);

	//well, pipeline 3D will convert homo space coord to pixel space
	std::vector<Vertex> vertexArray(3);
	vertexArray[0].color = VECTOR4(color.x, color.y, color.z, 1.0f);
	vertexArray[0].pos = VECTOR3(v1.x, v1.y, 0.0f);
	vertexArray[0].normal = VECTOR3(0.0f, 1.0f, 0.0f);
	vertexArray[0].texcoord = VECTOR2(0.0f, 1.0f);

	vertexArray[1].color = VECTOR4(color.x, color.y, color.z, 1.0f);
	vertexArray[1].pos = VECTOR3(v2.x, v2.y, 0.0f);
	vertexArray[1].normal = VECTOR3(0.0f, 1.0f, 0.0f);
	vertexArray[1].texcoord = VECTOR2(1.0f, 0.0f);

	vertexArray[2].color = VECTOR4(color.x, color.y, color.z, 1.0f);
	vertexArray[2].pos = VECTOR3(v3.x, v3.y, 0.0f);
	vertexArray[2].normal = VECTOR3(0.0f, 1.0f, 0.0f);//与屏幕垂直的法向量
	vertexArray[2].texcoord = VECTOR2(1.0f, 1.0f);

	std::vector<UINT> indexArray = { 0,1,2 };

	RenderPipeline_DrawCallData drawCallData;
	drawCallData.offset = 0;
	drawCallData.pIndexBuffer = &indexArray;
	drawCallData.pVertexBuffer = &vertexArray;
	drawCallData.VertexCount = 3;

	//Pipeline will directly draw to ColorBuffer and ZBuffer......
	IRenderPipeline3D::DrawTriangles(drawCallData);

}

void IRenderer::DrawRect(const COLOR3 & color, UINT x1, UINT y1, UINT x2, UINT y2)//就画个长方形咯
{
	for (UINT i = x1;i <= x2;++i)
		for (UINT j = y1;j <= y2;++j)
			mFunction_SetPixel(i, j, color);
};



void IRenderer::RenderMesh(IMesh& mesh)
{
	if (m_pCamera == nullptr)return;

	//set WVP matrices
	MATRIX4x4 matW, matV, matP;
	mesh.GetWorldMatrix(matW);
	m_pCamera->GetViewMatrix(matV);
	m_pCamera->GetProjMatrix(matP);

	IRenderPipeline3D::SetWorldMatrix(matW);
	IRenderPipeline3D::SetProjMatrix(matP);
	IRenderPipeline3D::SetViewMatrix(matV);
	IRenderPipeline3D::SetCameraPos(m_pCamera->GetPosition());
	IRenderPipeline3D::SetMaterial(mesh.mMaterial);
	IRenderPipeline3D::SetTexture(mesh.m_pTexture);//nullptr is OK
	IRenderPipeline3D::SetLightingEnabled(TRUE);

	RenderPipeline_DrawCallData drawCallData;
	drawCallData.offset = 0;
	drawCallData.pIndexBuffer = mesh.m_pIB_Mem;
	drawCallData.pVertexBuffer = mesh.m_pVB_Mem;
	drawCallData.VertexCount = mesh.GetVertexCount();
	
	//Pipeline will directly draw to ColorBuffer and ZBuffer......
	IRenderPipeline3D::DrawTriangles(drawCallData);

}

void IRenderer::RenderPointCollection(IPointCollection & collection)
{
	if (m_pCamera == nullptr)return;

	//set WVP matrices
	MATRIX4x4 matW, matV, matP;
	matW.Identity();
	m_pCamera->GetViewMatrix(matV);
	m_pCamera->GetProjMatrix(matP);

	IRenderPipeline3D::SetWorldMatrix(matW);
	IRenderPipeline3D::SetProjMatrix(matP);
	IRenderPipeline3D::SetViewMatrix(matV);
	IRenderPipeline3D::SetCameraPos(m_pCamera->GetPosition());
	IRenderPipeline3D::SetTexture(nullptr);//nullptr is OK
	IRenderPipeline3D::SetLightingEnabled(FALSE);

	RenderPipeline_DrawCallData drawCallData;
	drawCallData.offset = 0;
	drawCallData.pIndexBuffer = collection.m_pIB_Mem;
	drawCallData.pVertexBuffer = collection.m_pVB_Mem;
	drawCallData.VertexCount = collection.GetVertexCount();

	//Pipeline will directly draw to ColorBuffer and ZBuffer......
	IRenderPipeline3D::DrawPoint(drawCallData);
}

void IRenderer::Present()
{
	//to fit and present the whole buffer
	
	mFunction_AdjustWindowSize();
	mFunction_UpdateCharAndTextAttrBuffer();

	COORD topLeftPos = { 0,0 };
	DWORD tmpBytesWritten = 0;

	//Set array of Text Attribute (attribute like COLOR....)
	//The character values at the positions written to are not changed.
	::WriteConsoleOutputAttribute(
		m_hScreenBuffer,
		&m_pTextAttrBuffer->at(0),
		mBufferWidth*mBufferHeight,
		topLeftPos,
		&tmpBytesWritten);

	//Set array of Text
	::WriteConsoleOutputCharacterA(
		m_hScreenBuffer,						//handle of console screen BUFFER!!!
		&m_pCharBuffer->at(0),//应该是开头指针的意思						//ptr to buffer memory block
		mBufferWidth*mBufferHeight,	//pixels count
		topLeftPos,								//output start pos
		&tmpBytesWritten);					//return the bytes written???
	
}

void IRenderer::SetWindowTitle(const char * titleStr)
{
	::SetConsoleTitleA(titleStr);
	*m_pConsoleWindowTitle = titleStr;
}

UINT IRenderer::GetBufferWidth()
{
	return mBufferWidth;
}

UINT IRenderer::GetBufferHeight()
{
	return mBufferHeight;
}

/****************************************************
							P R I V A T E
*****************************************************/

void IRenderer::mFunction_GeneratePalette()
{
	//I think I should make full use of the foreGround and BackGround color,充分运用前景色和背景色
	//control the percentage of foreground coverage, 用字符大小来控制前景色的比例
	//to simulate linear interpolation between foreground and background color,去模拟前景色与背景色之间的线性插值
	//in order to "GENERATE" more preset color sample in color space为了扩展更多颜色空间的颜色
	//if I only use console's color , I'll only got 15 colors in total, which is a severe shortage.

	//-Generate Palette (generate more console "pixel", extension of color space)一个简易的调色板
	constexpr UINT c_ConsoleTotalColorCount = 8;//constexpr是常量表达式的意思


	
	/*FOREGROUND_BLUE 蓝色
	FOREGROUND_GREEN 绿色
	FOREGROUND_RED 红色
	FOREGROUND_INTENSITY 加强
	BACKGROUND_BLUE 蓝色背景
	BACKGROUND_GREEN 绿色背景
	BACKGROUND_RED 红色背景
	BACKGROUND_INTENSITY 背景色加强
	COMMON_LVB_REVERSE_VIDEO 反色*/
	

	//foreGround Text Attribute
	std::pair<COLOR3, WORD>  fgrTextAttr[c_ConsoleTotalColorCount] =
	{
		{ COLOR3(0,0,0),				NULL },
		/*{ COLOR3(0.5f,0,0),			FOREGROUND_RED },
		{ COLOR3(0,0.5f,0),			FOREGROUND_GREEN },
		{ COLOR3(0,0,0.5f),			FOREGROUND_BLUE },
		{ COLOR3(0.5f,0.5f,0),		FOREGROUND_RED | FOREGROUND_GREEN },
		{ COLOR3(0,0.5f,0.5f),		FOREGROUND_GREEN | FOREGROUND_BLUE },
		{ COLOR3(0.5f,0,0.5f),		FOREGROUND_RED | FOREGROUND_BLUE },
		{ COLOR3(0.5f,0.5f,0.5f),	FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE },
		{ COLOR3(1.0f,0,0),			FOREGROUND_RED | FOREGROUND_INTENSITY },*/
		{ COLOR3(0,1.0f,0),			FOREGROUND_GREEN | FOREGROUND_INTENSITY },
		{ COLOR3(0,0,1.0f),			FOREGROUND_BLUE | FOREGROUND_INTENSITY },
		{ COLOR3(0,1.0f,1.0f),		FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY },
		{ COLOR3(1.0f,0,1.0f),		FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY },
		{ COLOR3(1.0f,1.0f,0),		FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY },
		{ COLOR3(1.0f,1.0f,1.0f),	FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY },
	};//这里一共有八种颜色组合，原本可以有十六种，剩下的全都是加强的

	//BackGround Text Attribute
	 std::pair<COLOR3, WORD>  bgrTextAttr[c_ConsoleTotalColorCount] =
	{
		{ COLOR3(0,0,0),				NULL },
		/*{ COLOR3(0.5f,0,0),			BACKGROUND_RED },
		{ COLOR3(0,0.5f,0),			BACKGROUND_GREEN },
		{ COLOR3(0,0,0.5f),			BACKGROUND_BLUE },
		{ COLOR3(0.5f,0.5f,0),		BACKGROUND_RED | BACKGROUND_GREEN },
		{ COLOR3(0,0.5f,0.5f),		BACKGROUND_GREEN | BACKGROUND_BLUE },
		{ COLOR3(0.5f,0,0.5f),		BACKGROUND_RED | BACKGROUND_BLUE },
		{ COLOR3(0.5f,0.5f,0.5f),	BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE },*/
		{ COLOR3(1.0f,0,0),			BACKGROUND_RED | BACKGROUND_INTENSITY },
		{ COLOR3(0,1.0f,0),			BACKGROUND_GREEN | BACKGROUND_INTENSITY },
		{ COLOR3(0,0,1.0f),			BACKGROUND_BLUE | BACKGROUND_INTENSITY },
		{ COLOR3(0,1.0f,1.0f),		BACKGROUND_GREEN | BACKGROUND_BLUE | BACKGROUND_INTENSITY },
		{ COLOR3(1.0f,0,1.0f),		BACKGROUND_RED | BACKGROUND_BLUE | BACKGROUND_INTENSITY },
		{ COLOR3(1.0f,1.0f,0),		BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_INTENSITY },
		{ COLOR3(1.0f,1.0f,1.0f),	BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE | BACKGROUND_INTENSITY },
	};

	constexpr UINT c_steps = 6;
	constexpr BYTE c_LerpChar[c_steps] = { ' ',	  '.' ,  '='  ,'&'  ,'#'	,'@'};//这里表示就是前景色所占比例增加的方法
	//constexpr BYTE c_LerpChar[c_steps] = { ' ',	  '.' , '='  ,'#' };

	for (UINT i = 0;i < c_ConsoleTotalColorCount;++i)
	{
		for (UINT j = i+1;j < c_ConsoleTotalColorCount;++j)
		{
			//for example,(0,0,0)->(0.5,0,0) and (0,0,0)->(1,0,0) will generate some overlap colors
			//and could also generate discontinuous colors in one axis
			//those whose Euclidean Distance less than one should not perform Lerp.
			COLOR3 colorVec = fgrTextAttr[i].first - bgrTextAttr[j].first;
			if (colorVec.Length() < 0.9f)goto label_nextLerp;//这里goto的意思就是跳过下面一大段，
			//到下面写着label_nextlerp语句的地方，这里的意思应该是为了截断掉颜色过于相近的部分，避免产生重复的颜色，
			//其实这里就是把前景色和背景色相同的部分取掉
			//to generate new color , perform linear interpolation
			//between preset colors, and use adequate ascii char(dependent on the 
			//pixel coverage) as Lerp ratio factor

			//lerp from bgr color to fgr color
			for (UINT k =0;k < c_steps-1;k++)
			{
				COLOR3 newColor = Lerp(bgrTextAttr[i].first, fgrTextAttr[j].first, float(k) *1.0f / float(c_steps-1));
				
				//put the color in corresponding palette (classfied by block similar to octant)
				//determined by 3 bit , each one for x/y/z
				UINT paletteID = 0;
				if (newColor.x >= 0.5)paletteID += 0x0004;
				if (newColor.y >= 0.5)paletteID += 0x0002;
				if (newColor.z >= 0.5)paletteID += 0x0001;
				
				//new color
				Renderer_Color3ToConsolePixel tmpObj;
				tmpObj.asciiChar = c_LerpChar[k];
				tmpObj.color = newColor;
				tmpObj.textAttr = bgrTextAttr[i].second | fgrTextAttr[j].second;
				m_pPalette[paletteID]->push_back(tmpObj);//将调出的颜色放到调色板中，但是只有调色板分为八个层级，并不是
				//只有八个颜色
			}

		label_nextLerp:;
		}
	}
}

void IRenderer::mFunction_BlendPixel(UINT x, UINT y, float blendFactor, const COLOR3 & newColor)//两个颜色的混合
{
	if (x < mBufferWidth && y < mBufferHeight)
	{
		COLOR3& c = m_pColorBuffer->at(y*mBufferWidth + x);
		c.x = Lerp(c.x, newColor.x, blendFactor);
		c.y = Lerp(c.y, newColor.y, blendFactor);
		c.z= Lerp(c.z, newColor.z, blendFactor);
	}

}

inline void IRenderer::mFunction_SetPixel(UINT x, UINT y, const COLOR3 & color)
{
	if(x<mBufferWidth && y <mBufferHeight)
		m_pColorBuffer->at(y*mBufferWidth + x) = color;
}

inline UINT IRenderer::mFunction_GetIndex(UINT x, UINT y)
{
	return y*mBufferWidth+x;
}

void IRenderer::mFunction_AdjustWindowSize()
{

#ifdef SHOOT_THE_CHICKEN_SYSTEM_VER_WIN8_1
	int frameX_Width = GetSystemMetrics(SM_CXFIXEDFRAME);//frame boarder thickness
	int frameY_Height = GetSystemMetrics(SM_CYFIXEDFRAME);//frame boarder thickness
	int frameY_Caption = GetSystemMetrics(SM_CYCAPTION);//caption thickness
	int scrWidth = GetSystemMetrics(SM_CXSCREEN);
	int scrHeight = GetSystemMetrics(SM_CYSCREEN);
	int windowWidth = mBufferWidth*c_ConsoleCharSize + 2 * frameX_Width+20;
	int windowHeight = int(mBufferHeight*c_ConsoleCharSize/c_ConsoleCharAspectRatio) + frameY_Caption + frameY_Height+20;
#endif

#ifdef SHOOT_THE_CHICKEN_SYSTEM_VER_WIN10
	int frameX_Width = GetSystemMetrics(SM_CXBORDER);//frame boarder thickness
	int frameY_Height = GetSystemMetrics(SM_CYBORDER);//frame boarder thickness
	//返回以像素值为单位的Windows窗口边框的宽度和高度，
	//如果Windows的为3D形态，则等同于SM_CXEDGE参数
	int frameY_Caption = GetSystemMetrics(SM_CYCAPTION);//caption thickness标题厚度
	int scrWidth = GetSystemMetrics(SM_CXSCREEN);
	int scrHeight = GetSystemMetrics(SM_CYSCREEN);//以像素为单位计算的屏幕尺寸。
	int windowWidth =1110;
	int windowHeight = 625;//为什么要加16和20呢？
	
#endif


	//adjust the size of window to fit the buffer size (directly set the window pixel size)
	HWND hwnd = ::FindWindowA(NULL, m_pConsoleWindowTitle->c_str());//::GetForegroundWindow();

	//get the top left rect
	/*RECT prevRect;
	::GetWindowRect(hwnd, &prevRect);//GetWindowRect是一个Windows API函数。
	//该函数返回指定窗口的边框矩形的尺寸，函数原型为void GetWindowRect(LPRECT lpRect) const。*/

	//set new window size(to keep appropriate width and height)
	::MoveWindow(
		hwnd,
		150,
		100,
		windowWidth,
		windowHeight,
		true);//MoveWindow是一种函数。功能是改变指定窗口的位置和大小。
	//对子窗口来说，位置和大小取决于父窗口客户区的左上角；对于Owned窗口，位置和大小取决于屏幕左上角。
	
}

inline void IRenderer::mFunction_UpdateCharAndTextAttrBuffer()//更新缓冲区的字符和颜色
{

	for (UINT i = 0;i < mBufferWidth*mBufferHeight;++i)
	{
		COLOR3& color = m_pColorBuffer->at(i);

		//Please refer to Generation of Palettes
		//determined by 3 bit , each one for x/y/z
		UINT paletteID = 0;
		if (color.x >= 0.5)paletteID += 0x0004;
		if (color.y >= 0.5)paletteID += 0x0002;
		if (color.z >= 0.5)paletteID += 0x0001;

		float minLength = 1.0f;
		UINT paletteColorID = 0;
		for (UINT j = 0;j < m_pPalette[paletteID]->size();++j)
		{
			//find the nearest preset color (Euclidean distance)
			VECTOR3 deltaVec = color - m_pPalette[paletteID]->at(j).color;
			float len = deltaVec.Length();
			if (len < minLength)
			{
				minLength = len;
				paletteColorID = j;
			}//找出最接近的
		}
		
		//finally convert to console char
		m_pCharBuffer->at(i) = m_pPalette[paletteID]->at(paletteColorID).asciiChar;
		m_pTextAttrBuffer->at(i) = m_pPalette[paletteID]->at(paletteColorID).textAttr;
	}
}

