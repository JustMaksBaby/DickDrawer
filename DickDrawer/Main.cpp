#include <Windows.h>


const char* CLASS_NAME = "DickDrawClass";
bool END_PROG = false;

LRESULT CALLBACK WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{

	switch (msg)
	{
	//case WM_KEYDOWN: 
	//{
	//	if (wParam == VK_ESCAPE)
	//	{
	//		SendMessage(hWnd, WM_CLOSE, 0, 0); 
	//	}
	//}break;
	/*case WM_CLOSE: 
	{
		int answer = MessageBox(hWnd, "Do you want to close the prorgam?", "EXIT", MB_ICONWARNING | MB_YESNO);
		
		if (answer == IDYES)
		{
			END_PROG = true;
			DestroyWindow(hWnd);
		}

		return 0; 
	}break;*/
	case WM_DESTROY: 
	{
		PostQuitMessage(0); 
	}break;
	}

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

	while (!END_PROG)
	{
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				END_PROG = true;
				break;
			}

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	





	UnregisterClass(CLASS_NAME, hInst); 

}