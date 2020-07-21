#include <Windows.h>
#include <windowsx.h>

#define global_var  static

// class name
const char* CLASS_NAME = "DickDrawClass";

// control program cyrcle
bool END_PROG = false;


struct BitmapBuffer
{
	BITMAPINFO  Info;
	void* Memory;
	int    Width;
	int    Height;
	unsigned int    Pitch;
	unsigned short	BytesPerPixel = 4;
};

global_var BitmapBuffer BACK_BUFFER; 


void ResizeDBISection(BitmapBuffer* buffer, int Width, int Height)
{
	int NewBitmapMemorySize = (Width * Height) * buffer->BytesPerPixel;

	void* tempMemory = VirtualAlloc(0, NewBitmapMemorySize, MEM_COMMIT, PAGE_READWRITE);
	if (buffer->Memory)
	{
		// allocate buffer with new size
		
		// copy old buffer`s data to new buffer; 
		int oldBufferSize = buffer->Height * buffer->Width * buffer->BytesPerPixel; 
		int bytesToCopy = oldBufferSize < NewBitmapMemorySize ? oldBufferSize : NewBitmapMemorySize; 

		memcpy(tempMemory, buffer->Memory, bytesToCopy);

		// free old buffer
		VirtualFree(buffer->Memory, 0, MEM_RELEASE);
		
		//make buffer memory point to new memory location
	}

	buffer->Memory = tempMemory;
	tempMemory = nullptr;

	buffer->Width = Width;
	buffer->Height = Height;
	buffer->BytesPerPixel = 4;

	buffer->Info.bmiHeader.biSize = sizeof(buffer->Info.bmiHeader);
	buffer->Info.bmiHeader.biWidth = buffer->Width;
	buffer->Info.bmiHeader.biHeight = -buffer->Height;
	buffer->Info.bmiHeader.biPlanes = 1;
	buffer->Info.bmiHeader.biBitCount = 32;
	buffer->Info.bmiHeader.biCompression = BI_RGB;

	buffer->Pitch = Width * buffer->BytesPerPixel;
}

void DisplayBuffer(HDC hDc,
	 int width, int height,
	BitmapBuffer* buffer)
{
	//copy data from buffer to DC

	StretchDIBits(hDc, 
		          0,0, width, height,
		          0,0, width, height,
		          buffer->Memory,
		          &(buffer->Info),
		          DIB_RGB_COLORS, 
		          SRCCOPY); 
}


void RenderFrame(BitmapBuffer* buffer, UINT32 x, UINT32 y,  COLORREF color)
{
	
	UINT32 * pixel = (UINT32 *)buffer->Memory;
	pixel += (y * (buffer->Pitch/ buffer->BytesPerPixel)) + x;

	UINT8 red   = GetRValue(color);
	UINT8 green = GetGValue(color); 
	UINT8 blue  = GetBValue(color); 

	*pixel = (red << 16) | (green << 8) | blue;

}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{

	switch (msg)
	{
		
	case WM_PAINT: 
	{
		PAINTSTRUCT ps;
		//InvalidateRect(hWnd, nullptr, FALSE); 

		HDC hDc = BeginPaint(hWnd, &ps);
		DisplayBuffer(hDc,  ps.rcPaint.right, ps.rcPaint.bottom,  &BACK_BUFFER);
		EndPaint(hWnd, &ps);
	} break;
	case WM_SIZE:
	{
		ResizeDBISection(&BACK_BUFFER, LOWORD(lParam), HIWORD(lParam)); 
	}break;
	case WM_KEYDOWN: 
	{
		if (wParam == VK_ESCAPE)
		{
			PostMessage(hWnd, WM_CLOSE, 0, 0); 
		}
	}break;
	case WM_MOUSEMOVE: 
	{
		if (wParam & MK_LBUTTON)

		{
			HDC hdc = GetDC(hWnd);
			unsigned int x = GET_X_LPARAM(lParam);
			unsigned int y = GET_Y_LPARAM(lParam);
			
			RenderFrame(&BACK_BUFFER, x, y, RGB(255, 0, 0)); 

			ReleaseDC(hWnd, hdc);
		}
		return 0;
	}break;
	case WM_CLOSE: 
	{
		int answer = MessageBox(hWnd, "Do you want to close the prorgam?", "EXIT", MB_ICONWARNING | MB_YESNO);
		
		if (answer == IDYES)
		{
			END_PROG = true;
			DestroyWindow(hWnd);
		}

		return 0; 
	}break;
	case WM_DESTROY: 
	{
		PostQuitMessage(0);
		return 0; 
	}break;
	default: 
	{return DefWindowProc(hWnd, msg, wParam, lParam); } 

	}

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
	wndClass.hCursor =  LoadCursor((HINSTANCE)NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndClass.lpszMenuName = 0;
	wndClass.lpszClassName =CLASS_NAME ;
	


	// create window
	HWND hWnd = NULL; 

	if (!RegisterClassEx(&wndClass))
	{
		MessageBox(0, "ERROR: Register Class", 0, MB_OK);
	}
	else
	{
		ResizeDBISection(&BACK_BUFFER, width, height); 

		hWnd = CreateWindow(
			CLASS_NAME,
			"DickDraw",
			WS_OVERLAPPED|WS_SYSMENU,
			100, 100,
			width, height,
			0,
			0,
			hInst,
			0);

		ShowWindow(hWnd, SW_SHOW);

	}

	return hWnd; 

}

int WINAPI wWinMain(HINSTANCE hInst, HINSTANCE hPrevInst, PWSTR pCmdLine, int showMode)
{

	HWND hWnd = Init(hInst, 800,500); 


	MSG msg;

	while (!END_PROG)
	{
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		

		HDC hDc = GetDC(hWnd);

		RECT clientRect; 
		GetClientRect(hWnd, &clientRect); 

		DisplayBuffer(hDc,clientRect.right, clientRect.bottom, &BACK_BUFFER); 
		ReleaseDC(hWnd, hDc); 
	}
	

	UnregisterClass(CLASS_NAME, hInst); 

}