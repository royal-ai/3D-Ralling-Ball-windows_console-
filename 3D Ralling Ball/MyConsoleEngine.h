#pragma once

#include <stdio.h>
#include <Windows.h>
#include "mmsystem.h"      
#pragma comment(lib,"winmm.lib")//播放音乐的几个头文件
#include <vector>
#include <cstring>
#include <cmath>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <random>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <process.h>        
#include "Math.h"
#include "Types_def.h"
#include "PointCollection.h"
#include "FileLoader.h"
#include "Picture2D.h"
#include "_GeometryMeshGenerator.h"
#include "Camera.h"
#include "Mesh.h"
#include "Timer.h"
#include "RenderPipeline3D.h"
#include "Renderer.h"
#include"SafeQueue.h"
#include"ThreadPool.h"
#define IS_KEY_DOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
#define IS_KEY_UP(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 0 : 1)
#define DEBUG_MSG1(msg)	::MessageBoxA(0,msg,0,0);//这里就是显示对话框的意思，msg是显示的字符串

typedef unsigned char BYTE;

