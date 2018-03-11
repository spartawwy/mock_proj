// GloaKmCapture.cpp : 定义 DLL 的初始化例程。
//

#include "stdafx.h"
#include "GloaKmCapture.h"

#include <algorithm>
#include <unordered_map>

#include "gloakm_capture_api.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#if 1
#define KEYDOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0) 
#define KEYUP(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 0 : 1) 
#else

#define KEYDOWN(vk_code) ((GetKeyState(vk_code) & 0x8000) ? 1 : 0) 
#define KEYUP(vk_code) ((GetKeyState(vk_code) & 0x8000) ? 0 : 1) 
#endif

//
//TODO: 如果此 DLL 相对于 MFC DLL 是动态链接的，
//		则从此 DLL 导出的任何调入
//		MFC 的函数必须将 AFX_MANAGE_STATE 宏添加到
//		该函数的最前面。 
//		例如: 
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// 此处为普通函数体
//		}
//
//		此宏先于任何 MFC 调用
//		出现在每个函数中十分重要。这意味着
//		它必须作为函数中的第一个语句
//		出现，甚至先于所有对象变量声明，
//		这是因为它们的构造函数可能生成 MFC
//		DLL 调用。
//
//		有关其他详细信息，
//		请参阅 MFC 技术说明 33 和 58。
//

// CGloaKmCaptureApp

BEGIN_MESSAGE_MAP(CGloaKmCaptureApp, CWinApp)
END_MESSAGE_MAP()

//
//struct T_KeysStatus
//{
//    int downed_keys_count; //how many keys down
//    bool is_left_ctr_down;
//    bool is_right_ctr_down;
//    DWORD pre_Key;
//};
// 
//T_KeysStatus g_key_status = {0};
//
//bool g_is_prepare_buy = false;
//bool g_is_prepare_sell = false;

ReCallFunc   p_recall_func = nullptr;
//char         g_win_tag[1024] = {0};

HHOOK        Hook;

LRESULT CALLBACK LauncherHook(int nCode, WPARAM wParam, LPARAM lParam);

CGloaKmCaptureApp theApp;

CGloaKmCaptureApp::CGloaKmCaptureApp()
{
	// TODO:  
}
 
BOOL CGloaKmCaptureApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}
 
extern "C"  DllExport BOOL WINAPI InstallLaunchEv(ReCallFunc func, char* tag_str)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	OutputDebugString("Enter InstallLaunchEv\n");
	 
	Hook = (HHOOK)SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC)LauncherHook, theApp.m_hInstance, 0);
    if( !Hook )
        return FALSE;
    p_recall_func = func;
    /*if( tag_str )
        strcpy_s(g_win_tag, sizeof(g_win_tag), tag_str);*/
    return TRUE;
}

WPARAM g_pre_action = 0;
DWORD g_pre_key = 0;
//int g_count = 0;

LRESULT CALLBACK LauncherHook(int nCode, WPARAM wParam, LPARAM lParam)
{
	// 发现 用 hook 不灵光: 第一次按 PLUS 时不会触发. 长按某健 多次 进入此函数 不满足需求
	// todo: to use GetKeyboardState
	/* http://blog.csdn.net/linrulei11/article/details/7612197
	关于GetAsyncKeyState与GetKeyState区别： 
	GetAsyncKeyState上面已经讲差不多了，关于GetAsyncKeyState与GetKeyState二者最大区别:GetAsyncKeyState在按键不按的情况下为0，而GetKeyState在按键不按的情况下开始为0，当一次‘按下抬起’后变为1，依次循环。
	*/ 

//#define  PRE_CALL
#ifdef PRE_CALL
	LRESULT Result = CallNextHookEx(Hook, nCode, wParam, lParam);
#endif 
	KBDLLHOOKSTRUCT* pSturct = (KBDLLHOOKSTRUCT*)lParam;
	// filter long time pressing
	if ( g_pre_action == wParam && g_pre_key == pSturct->vkCode )
	{
		goto EXIT_PROC;
	}
	
#if 0
	auto lctr_val = KEYUP(VK_LCONTROL);
	auto rctr_val = KEYUP(VK_RCONTROL);
	char buf[1024] = {0};
	sprintf(buf, "%d Enter LauncherHook  line 119-- act:%0x key:%0x lctr:%d rctr:%d \n \0"
		, g_count++, wParam, pSturct->vkCode, lctr_val, rctr_val );
	OutputDebugString(buf);
#endif
	if( g_pre_action != wParam )
		g_pre_action = wParam;
	if( g_pre_key != pSturct->vkCode )
		g_pre_key = pSturct->vkCode;
	  

	if( (wParam == WM_KEYDOWN || wParam == WM_KEYUP)
		&& (VK_OEM_PLUS == pSturct->vkCode || VK_OEM_MINUS == pSturct->vkCode || VK_LCONTROL == pSturct->vkCode || VK_RCONTROL == pSturct->vkCode) ) 
	{
		//OutputDebugString("Enter LauncherHook line 147 \n"); 
		 
		if( p_recall_func )
		{  
			p_recall_func();
		} 
		goto EXIT_PROC;
#if 0 
		if( (KEYDOWN(VK_LCONTROL) || KEYDOWN(VK_RCONTROL)) && !(KEYDOWN(VK_LCONTROL) && KEYDOWN(VK_RCONTROL)) )
		{ 
			if( KEYDOWN(VK_OEM_PLUS) && KEYUP(VK_OEM_MINUS) )
			{ 
				OutputDebugString("LauncherHook  line 136 set g_is_prepare_buy = true\n");
				g_is_prepare_buy = true;
			}
			else if( KEYDOWN(VK_OEM_MINUS) && KEYUP(VK_OEM_PLUS) )
			{
				OutputDebugString("LauncherHook  line 141 set g_is_prepare_sell = true\n");
				g_is_prepare_sell = true;
			}
			else if( KEYDOWN(VK_OEM_MINUS) && KEYDOWN(VK_OEM_PLUS) )
			{
				OutputDebugString("LauncherHook  line 146 set all false\n");
				g_is_prepare_buy = false;
				g_is_prepare_sell = false;
			} 
		}
		if( KEYUP(VK_OEM_PLUS) )
		{
			if (g_is_prepare_buy && KEYUP(VK_OEM_MINUS))
			{
				OutputDebugString("LauncherHook  line 155 set g_is_prepare_buy = false\n");
				g_is_prepare_buy = false; 
				if( p_recall_func )
				{ 
					OutputDebugString("TO BUY\n");
					p_recall_func(true, "");
				} 
			} 
		}
		if( KEYUP(VK_OEM_MINUS) )
		{
			if (g_is_prepare_sell && KEYUP(VK_OEM_PLUS))
			{
				OutputDebugString("LauncherHook  line 168 set g_is_prepare_sell = false\n");
				g_is_prepare_sell = false;
				if( p_recall_func )
				{
					OutputDebugString("TO SELL\n");
					p_recall_func(false, "");
				} 
			} 
		}
		if(  KEYUP(VK_LCONTROL) && KEYUP(VK_RCONTROL) )
		{ 
			OutputDebugString("LauncherHook  line 179 set all = false\n");
			g_is_prepare_buy = false;
			g_is_prepare_sell = false;
		}
#endif
	} //if

EXIT_PROC:
#ifdef PRE_CALL
	return Result;
#else
	return CallNextHookEx(Hook,nCode,wParam,lParam);
#endif
}

extern "C" DllExport void WINAPI UnInstallLaunchEv()
{
	// todo:
}