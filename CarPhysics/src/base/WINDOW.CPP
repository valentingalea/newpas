/*
 * Copyright (c) 2012 Valentin Galea
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <cstdio>
#include "window.h"
#include "error.h"
#include "input.h"
#include "../mmgr/mmgr.h"

int window_t::width;
int window_t::height;

/*
 * .name	EnumAll()
 *
 * .info	enumerates all display modes
 */
void window_t::vidModeDetails_t::EnumAll()
{
	DEVMODE dm;
	int i, result;

	i = 0;
	while ( EnumDisplaySettings(NULL, i++, &dm) )
	{
		printf ( 
			"%s v%d.%d > %dx%dx%d @ %dHz ... ",
			dm.dmDeviceName,
			LOBYTE(dm.dmDriverVersion),
			HIBYTE(dm.dmDriverVersion),
			dm.dmPelsWidth,
			dm.dmPelsHeight,
			dm.dmBitsPerPel,
			dm.dmDisplayFrequency
			);
		
		result = ChangeDisplaySettings ( &dm, CDS_TEST );
		switch ( result )
		{				
		case DISP_CHANGE_SUCCESSFUL:
			printf ( "ok\n" );
			break;
		case DISP_CHANGE_FAILED:
			printf ( "failed\n" ); 
			break;
		case DISP_CHANGE_BADMODE:
			printf ( "bad mode\n" );
			break;
		case DISP_CHANGE_BADFLAGS:
			printf ( "bad flags\n" ); 
			break;
		case DISP_CHANGE_RESTART:
			printf ( "restart required\n" ); 
			break;
		}
	}
}

/*
 * .name	window_t constructor
 *
 * .info	spawns an OpenGL Win32 window
 */
window_t::window_t( window_t::vidModeDetails_t& mode )
{
	if ( mode.txt_console )
		if ( !createDosConsole() )
			MessageBox(NULL, "Error spawning Dos console.", "Error", MB_ICONEXCLAMATION | MB_OK);

	DEVMODE dmScreenSettings;
	WNDCLASS wc;
	RECT wrect;
	int pf; //pixel format
	int dwExStyle, dwStyle; //stupid Microsoft notated params.

	width = mode.width;
	height = mode.height;

	hinst = GetModuleHandle(NULL);
	wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc		= (WNDPROC)WndProc;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hInstance		= hinst;
	wc.hIcon			= LoadIcon(NULL, IDI_WINLOGO);
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground	= NULL;
	wc.lpszMenuName 	= NULL;
	/*
	 * make a new window-class-name each time, for some reason this is
	 * needed, unregistering the old name isn't enough !
	 */
	static int count = 1;
	sprintf(winClassName, "OpenGL SubSystem(%2d)", count);
	count++;
	wc.lpszClassName	= winClassName;

	static PIXELFORMATDESCRIPTOR pfdesc =
	{
		sizeof(PIXELFORMATDESCRIPTOR), //size
		1, //version
		PFD_DRAW_TO_WINDOW|PFD_SUPPORT_OPENGL|PFD_DOUBLEBUFFER, //flags
		PFD_TYPE_RGBA, // rgba format
		(unsigned char)mode.bits_per_pixel,
		0, 0, 0, 0, 0, 0, // color bits
		mode.alpha_buffer, // alpha buffer
		0, // shift bit,
		mode.accumul_buffer, // accumulation buffer
		0, 0, 0, 0, // accumulation bits
		mode.z_buffer_bits, // z-buffer depth
		mode.stencil_buffer, // stencil buffer
		mode.aux_buffer, // auxiliary buffer
		PFD_MAIN_PLANE, // main drawing layer
		0, // rezerved
		0, 0, 0 // layer masks
	};

	if (!RegisterClass(&wc))
		throw error("OpenGL subsystem failed : register class");

	if ( mode.full_screen )
	{
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth  = width;
		dmScreenSettings.dmPelsHeight = height;
		dmScreenSettings.dmBitsPerPel = mode.bits_per_pixel;
		dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;

		if (ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL)
			throw error("OpenGL subsystem failed : ch. displ. sett.");
	}

	wrect.left = 0;
	wrect.right = width;
	wrect.top = 0;
	wrect.bottom = height;

	if ( mode.full_screen )
	{
		dwExStyle = WS_EX_APPWINDOW;
		dwStyle = WS_POPUP;
		ShowCursor(false);
	}
	else
	{
		dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
		dwStyle = WS_OVERLAPPEDWINDOW;
	}

	AdjustWindowRectEx(&wrect, dwStyle, false, dwExStyle);

	hwnd = CreateWindowEx(
				dwExStyle,
				winClassName, mode.name,
				dwStyle,
				0, 0, wrect.right - wrect.left, wrect.bottom - wrect.top,
				NULL, NULL, hinst, NULL);
	if ( !hwnd )
		throw error("OpenGL subsystem failed : !hwnd ");

	if ( !(hdc = GetDC(hwnd)) )
		throw error("OpenGL subsystem failed : GetDC()");

	if ( !(pf = ChoosePixelFormat(hdc, &pfdesc)) )
		throw error("OpenGL subsystem failed : ChoosePixelFormat()");

	if ( !SetPixelFormat(hdc, pf, &pfdesc) )
		throw error("OpenGL subsystem failed : SetPixelFormat()");

	if ( !(hrc = wglCreateContext(hdc)) )
		throw error("OpenGL subsystem failed : wglCreateContext()");

	if ( !wglMakeCurrent(hdc, hrc) )
		throw error("OpenGL subsystem failed : wglMakeCurrent()");

	ShowWindow(hwnd, SW_SHOW);
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);

	vidMode = mode;

	/*
	sprintf(glDescription,
		"Vendor: %s\nRenderer: %s\nVersion: %s\nExtensions:\n%s\n\0",
		glGetString(GL_VENDOR),
		glGetString(GL_RENDERER),
		glGetString(GL_VERSION),
		glGetString(GL_EXTENSIONS));
	*/

}

/*
 * .name	window_t destructor
 *
 * .info	kill!
 */
window_t::~window_t()
{
	if ( vidMode.txt_console )
		FreeConsole();

	if ( vidMode.full_screen )
	{
		ChangeDisplaySettings(NULL, 0);
		ShowCursor(true);
	}

	if (hrc && !wglMakeCurrent(NULL,NULL) && !wglDeleteContext(hrc) )
		throw error("OpenGL subsystem kill failed : wglMakeCurrent() && wglDeleteContext()");

	if (hdc && !ReleaseDC(hwnd, hdc))
		throw error("OpenGL subsystem kill failed : ReleaseDC()");

	if (hwnd && !DestroyWindow(hwnd))
		throw error("OpenGL subsystem kill failed : DestroyWindow()");

	if (hinst && !UnregisterClass(winClassName, hinst))
		throw error("OpenGL subsystem kill failed : UnregisterClass()");

	hdc = NULL;
	hinst = NULL;
	hrc = NULL;
	hwnd = NULL;

}

/*
 * .name	startFrame()
 *
 * .info	message-pump
 */
bool window_t::startFrame()
{
	MSG msg;

	Sleep( 1 );

	while ( PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) )
	{
		if ( msg.message == WM_QUIT )
			return false;
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return true;
}

/*
 * .name	endFrame()
 *
 * .info	flushes the frame to the renderer
 */
void window_t::endFrame()
{
	SwapBuffers(hdc);
}

/*
 * .name	WndProc()
 *
 * .info	win32 messages handler
 */
LRESULT CALLBACK window_t::WndProc(
							HWND	hWnd,			// Handle For This Window
							UINT	uMsg,			// Message For This Window
							WPARAM	wParam,			// Additional Message Information
							LPARAM	lParam)			// Additional Message Information
{

	keyboard_t &kb = keyboard_t::getHandle();
	mouse_t &mouse = mouse_t::getHandle();

	switch ( uMsg )
	{
		case WM_CLOSE:
		{
			PostQuitMessage( 0 );
			return false;
		}

		case WM_SIZE:
		{
			width = LOWORD(lParam);
			height= HIWORD(lParam);

			return true;
		}

		case WM_KEYDOWN:
		{
			kb.keycode = (lParam >> 16 ) & 255;
			kb.keydown[ kb.keycode ] = true;

			return true;
		}

		case WM_KEYUP:
		{
			kb.keycode = (lParam >> 16 ) & 255;
			kb.keydown[ kb.keycode ] = false;
			kb.keychar = 0;
			kb.keycode = 0;
			
			return true;
		}

		case WM_CHAR:
		case WM_SYSCHAR:
		{
			kb.keychar = wParam & 255;

			return true;
		}

		/* handled in WM_KEYUP
		case WM_DEADCHAR:
		case WM_DEADSYSCHAR:
		*/

		case WM_LBUTTONDOWN:
		{
			mouse.button = mouse_t::left;
			mouse.x	= lParam & 0xFFFF;
			mouse.y = lParam >> 16;
			return true;
		}

		case WM_LBUTTONUP:
		{
			mouse.button = 0;
			return true;
		}

		case WM_RBUTTONDOWN:
		{
			mouse.button = mouse_t::right;
			mouse.x	= lParam & 0xFFFF;
			mouse.y = lParam >> 16;
			return true;
		}

		case WM_RBUTTONUP:
		{
			mouse.button = 0;
			return true;
		}

		case WM_MOUSEMOVE:
		{
			int mx = mouse.x;
			int my = mouse.y;
			mouse.x	= lParam & 0xFFFF;
			mouse.y = lParam >> 16;

			mouse.motion[0] = /*mouse.motion[0] + */(mouse.x - mx);
			mouse.motion[1] = /*mouse.motion[1] + */(mouse.y - my);

			return true;
		}

	}

	// Pass All Unhandled Messages To DefWindowProc
	kb.keycode = 0;
	kb.keychar = 0;
	return DefWindowProc(hWnd,uMsg,wParam,lParam);
}

/*
 * .name	createDosConsole()
 *
 * .info	spawns an MS-DOS prompt console usefull for debuging
 */
bool window_t::createDosConsole()
{

	FreeConsole();
	if ( !AllocConsole() )
		return false;

	//redirect I/O
	if ( !freopen ("CONIN$","rt",stdin) )
		return false;
	if ( !freopen ("CONOUT$","wt",stdout) )
		return false;
	if ( !freopen ("CONOUT$","wt",stderr) )
		return false;

	if ( !SetConsoleTitle ("I/O console") )
		return false;

	return true;
}
