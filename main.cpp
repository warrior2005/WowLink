#include "WowLink.h"
#include "Resource.h"
#include <windows.h>

WowLinkPtr g_pGame = NULL;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
				   PSTR szCmdLine, int iCmdShow)
{
	// 864 * 576
	WowLink* pGame = new WowLink(hInstance, TEXT("WOW Link"), 
		TEXT("WOW Link"), IDI_WOWLINK, IDI_WOWLINK_SM, COL * 48, ROW * 48, 10);
	g_pGame = std::shared_ptr<WowLink>(pGame);
	if (!g_pGame || !g_pGame->Initialize(iCmdShow))
	{
		return FALSE;
	}
	g_pGame->Run();

	return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWindow, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// Route all Windows messages to the game engine
	return g_pGame->HandleEvent(hWindow, msg, wParam, lParam);
}