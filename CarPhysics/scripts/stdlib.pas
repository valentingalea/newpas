(*
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
 *)
 
(*
 * library with external functions
 * compile 'game.pas' after modifing this
 *)

function ftoi(f : real) : integer; external 0;
function ord(c : char) : integer; external 1;
function chr(i : integer) : char; external 2;
procedure write(..) external 3;

type bodyTypes = (
	BOX,
	CYLINDER,
	CAPPED_CYLINDER,
	SPHERE
);

procedure game_createBody(
	kind : integer;
	mass : real;
	sizeParam1 : real;
	sizeParam2 : real;
	sizeParam3 : real;
	position : string;
	orientAngle : real;
	orientAxis : string
) external 4;

type variables = (
	CAR_POSITION,
	CAR_MASS,
	CAR_ANTISWAYFORCE,
	WHEEL_STEERLOCK,
	WHEEL_SUSPENSION_ERP,
	WHEEL_SUSPENSION_CFM,
	INPUT_ACCELERATION,
	INPUT_BRAKE,
	INPUT_TURNING,
	CAM_SPRING_STIFF,
	CAM_SPRING_DAMPING,
	CAM_MASS
);

procedure game_setVar(
	kind : variables;
	..
) external 5;
