#include <Windows.h>
#include <windowsx.h>

#define global_var  static

// class name
const char* CLASS_NAME = "DickDrawClass";

// control program cyrcle
bool END_PROG = false;

// palete size
global_var int PALETE_WIDTH  = 40; 
global_var int PALETE_HEIGHT = 40; 

// color for pen
global_var COLORREF PEN_COLOR = RGB(255, 0, 0); 

struct BitmapBuffer
{
	BITMAPINFO  Info;
	void* Memory;
	int    Width  = 0 ;
	int    Height = 0;
	unsigned int    Pitch;
	unsigned short	BytesPerPixel = 4;
};

global_var BitmapBuffer BACK_BUFFER; 


void ResizeDBISection(BitmapBuffer* buffer, int newWidth, int newHeight)
{
	int newBufferSize = newWidth * newHeight* buffer->BytesPerPixel;
	int oldBufferSize = buffer->Height * buffer->Width * buffer->BytesPerPixel;

	if (oldBufferSize < newBufferSize)
	{
		// allocate buffer with new size;

		void* newMemory = VirtualAlloc(0, newBufferSize, MEM_COMMIT, PAGE_READWRITE);

		UINT8* tempMemory = static_cast<UINT8*>(newMemory); 
		UINT8* bufferTempMemory = static_cast<UINT8*>(buffer->Memory); 

		if (buffer->Memory)
		{
			int  newPitch = newWidth * buffer->BytesPerPixel; 

			// copy old buffer`s data to new buffer;
			for (int y = 0; y < buffer->Height; ++y)
			{
				for (int x = 0; x < buffer->Width * buffer->BytesPerPixel; ++x)
				{
					tempMemory[y * newPitch + x] = bufferTempMemory[y * buffer->Pitch + x]; 
				}
			} 

			// free old buffer
			VirtualFree(buffer->Memory, 0, MEM_RELEASE);

		}

		//make buffer memory point to new memory location
		buffer->Memory = newMemory;
		tempMemory = nullptr;
		newMemory = nullptr;

		buffer->Width = newWidth;
		buffer->Height = newHeight;

		buffer->Info.bmiHeader.biSize = sizeof(buffer->Info.bmiHeader);
		buffer->Info.bmiHeader.biWidth = buffer->Width;
		buffer->Info.bmiHeader.biHeight = -buffer->Height;
		buffer->Info.bmiHeader.biPlanes = 1;
		buffer->Info.bmiHeader.biBitCount = 32;
		buffer->Info.bmiHeader.biCompression = BI_RGB;

		buffer->Pitch = newWidth * buffer->BytesPerPixel;
	}
}


void ChangePixel(BitmapBuffer* buffer, UINT32 x, UINT32 y, COLORREF color)
{
	// set color for each pixel

	UINT32* pixel = (UINT32*)buffer->Memory;
	pixel += (y * (buffer->Pitch / buffer->BytesPerPixel)) + x;

	UINT8 red = GetRValue(color);
	UINT8 green = GetGValue(color);
	UINT8 blue = GetBValue(color);

	*pixel = (red << 16) | (green << 8) | blue;

}
void CreatePalete(BitmapBuffer* buffer)
{
	for (int y = 0; y < PALETE_HEIGHT; ++y)
	{
		for (int x = 0; x < PALETE_WIDTH; ++x)
		{
			if (y < 20 && x < 20)
			{
				ChangePixel(buffer, x, y, RGB(255, 0, 0)); 
			}
			else if (y < 40 && x < 20)
			{
				ChangePixel(buffer, x, y, RGB(0, 255, 0)); 
			}
			else if (y < 20 && x < 40)
			{
				ChangePixel(buffer, x, y, RGB(0, 0, 255)); 
			}
			else if (y < 40 && x < 40)
			{
				ChangePixel(buffer, x, y, RGB(255, 0, 255)); 
			}
		}
	}
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

LRESULT CALLBACK WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		
		case WM_PAINT: 
		{
			PAINTSTRUCT ps;

			HDC hDc = BeginPaint(hWnd, &ps);

			DisplayBuffer(hDc,  ps.rcPaint.right, ps.rcPaint.bottom,  &BACK_BUFFER);

			EndPaint(hWnd, &ps);
		} break;

		case WM_SIZE:
		{
			RECT rc; 
			GetClientRect(hWnd, &rc); 

			ResizeDBISection(&BACK_BUFFER, LOWORD(lParam), HIWORD(lParam)); 
		}break;

		case WM_KEYDOWN: 
 		{
			if (wParam == VK_ESCAPE)
			{
				PostMessage(hWnd, WM_CLOSE, 0, 0); 
			}
		}break;
		case WM_LBUTTONDOWN: 
		{
			UINT32 x = GET_X_LPARAM(lParam); 
			UINT32 y = GET_Y_LPARAM(lParam); 

			if (y < 20 && x < 20)
			{
				PEN_COLOR =  RGB(255, 0, 0);
			}
			else if (y < 40 && x < 20)
			{
				PEN_COLOR = RGB(0, 255, 0);
			}
			else if (y < 20 && x < 40)
			{
				PEN_COLOR = RGB(0, 0, 255);
			}
			else if (y < 40 && x < 40)
			{
				PEN_COLOR =RGB(255, 0, 255);
			}

		}break;
		case WM_MOUSEMOVE: 
		{
			if (wParam & MK_LBUTTON)
			{
				unsigned int x = GET_X_LPARAM(lParam);
				unsigned int y = GET_Y_LPARAM(lParam);

				// to not draw above the palette

				if (x >= 40 || y >= 40)
				{
					ChangePixel(&BACK_BUFFER, x, y, PEN_COLOR);
				} 
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
		{
			return DefWindowProc(hWnd, msg, wParam, lParam); 
		} 

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
		RECT clientRect = { 0,0, width, height };
		AdjustWindowRect(&clientRect, WS_OVERLAPPED | WS_SYSMENU | WS_SIZEBOX, FALSE);

		hWnd = CreateWindow(
			CLASS_NAME,
			"DickDraw",
			WS_OVERLAPPED|WS_SYSMENU|WS_SIZEBOX,
			100, 100,
			clientRect.right - clientRect.left,
			clientRect.bottom - clientRect.top,
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
	//create window
	HWND hWnd = Init(hInst, 800,500); 

	// draw palette; 
	CreatePalete(&BACK_BUFFER);

	//main loop 
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
