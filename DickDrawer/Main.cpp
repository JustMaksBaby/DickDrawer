#include <Windows.h>


const char* CLASS_NAME = "DickDrawClass";


LRESULT CALLBACK WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return DefWindowProc(hWnd, msg, wParam, lParam); 
}

HWND Init(HINSTANCE hInst, UINT width, UINT height)
{
	/* 
		init function: 
			create and register class
			create window
	*/

	
	WNDCLASSEX wndClass{};
 
	
	wndClass.cbSize = sizeof(wndClass);
	wndClass.style = CS_OWNDC|CS_HREDRAW|CS_VREDRAW;
	wndClass.lpfnWndProc = WindowProc;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0 ;
	wndClass.hInstance = hInst;
	wndClass.hIcon = LoadIcon(0, IDI_APPLICATION);
	wndClass.hCursor = LoadCursor(0, IDC_APPSTARTING);
	wndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndClass.lpszMenuName = 0;
	wndClass.lpszClassName =CLASS_NAME ;
	
	if (!RegisterClassEx(&wndClass))
	{
		MessageBox(0, "ERROR: Register Class", 0, MB_OK);
	}
	else
	{
		HWND hWnd = CreateWindow(
			CLASS_NAME,
			"DickDraw",
			WS_OVERLAPPED|WS_SYSMENU|WS_SIZEBOX|WS_MAXIMIZEBOX|WS_MINIMIZEBOX,
			100, 100,
			width, height,
			0,
			0,
			hInst,
			0);


		ShowWindow(hWnd, SW_SHOW);

		return hWnd; 
	}

}

int WINAPI wWinMain(HINSTANCE hInst, HINSTANCE hPrevInst, PWSTR pCmdLine, int showMode)
{

	HWND hWnd = Init(hInst, 400,500); 


	MSG msg; 
	while (GetMessage(&msg, hWnd, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	






	UnregisterClass(CLASS_NAME, hInst); 

}