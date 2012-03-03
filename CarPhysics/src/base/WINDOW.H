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

#ifndef __WINDOW_H__
#define __WINDOW_H__

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

#include <gl\gl.h>
#include <gl\glu.h>

class window_t
{
public:
	struct vidModeDetails_t
	{
		// generic attributes
		const char * name;
		int width, height, bits_per_pixel;
		bool full_screen, txt_console;

		// PFD specific (TODO: more)
		unsigned char alpha_buffer, accumul_buffer, z_buffer_bits, stencil_buffer, aux_buffer;

		// default settings
		vidModeDetails_t()
		{
			name = "OpenGL Window";
			width = 320;
			height = 240;
			bits_per_pixel = 16;
			full_screen = false;
			txt_console = false;
			alpha_buffer = 0;
			accumul_buffer = 0;
			z_buffer_bits = 16;
			stencil_buffer = 0;
			aux_buffer = 0;
		}

		// displays all available modes
		static void EnumAll();
	};

	window_t( vidModeDetails_t& );
	~window_t();

	bool startFrame(); //return false if shutdown
	void   endFrame();

	// there is only one active window, so this makes sense
	static int width, height;

private:
	vidModeDetails_t vidMode;
	char winClassName[64];

public:
	char glDescription[1024];

private:
	HWND hwnd;
	HDC hdc;
	HGLRC hrc;
	HINSTANCE hinst;
	
	static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
	static bool createDosConsole();
};

#endif //__WINDOW_H__
