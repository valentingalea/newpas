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

#ifndef __INPUT_H__
#define __INPUT_H__

/*
 *
 *	Keyboard
 *
 */
namespace KEY
{
	enum
	{
		ESC = 1,
		/* un-shifted chars (exact keyboard layout) */
		_1,_2,_3,_4,_5,_6,_7,_8,_9,_0,MINUS,EQUAL,/*BACKSLASH,*/BACKSPACE,
		TAB,Q,W,E,R,T,Y,U,I,O,P,LEFT_BRACKET,RIGHT_BRACKET,ENTER,CTRL,
		/*CAPS*/A,S,D,F,G,H,J,K,L,SEMICOLON,SINGLEQUOTE,
		Z=44,X,C,V,B,N,M,COMMA,POINT,FORWARDSLASH,

		/* special keys */
		TILDE = 41, BACKSLASH = 43, CAPSLOCK = 58,
		SPACE = 57,
		LCONTROL = CTRL, RCONTROL = CTRL,
		LSHIFT = 42, RSHIFT = 54, SHIFT = 42,
		ALT = 56,
		UP = 72, DOWN = 80, LEFT = 75, RIGHT = 77,
		F1 = 59, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,

		INSERT = 82, HOME = 71, PGUP = 73,
#undef DELETE //who the fuck defines it ?
		DELETE = 83, END = 79, PGDN = 81
	};
};

class keyboard_t
{
public:
	// full access
	int keycode;
	char keychar;
	bool keydown[128];

	// access-door
	static inline keyboard_t& getHandle()
	{
		return keyboard;
	}

private:
	// only one instance, singleton model
	static keyboard_t keyboard;

	// hide any constructors
	inline keyboard_t()
	{
	}
	inline keyboard_t(const keyboard_t &)
	{
	}

};

/*
 *
 *	Mouse
 *
 */
class mouse_t
{
public:
	enum buttons
	{
		left = 1,
		middle = 3,
		right = 2
	};

	// full access
	int x, y, button;
	int motion[2];
	float sensitivity;

	// access-door
	static inline mouse_t& getHandle()
	{
		return mouse;
	}

private:
	// only one instance, singleton model
	static mouse_t mouse;

	// hide any constructors
	inline mouse_t()
	{
	}
	inline mouse_t(const mouse_t &)
	{
	}

};

#endif //__INPUT_H__
