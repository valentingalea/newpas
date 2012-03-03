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

procedure write(..) external 1;

type
     vtype	= integer
     vector	= record
		x, y, z: vtype;
		end;


operator * (u : vector; s : vtype) = r : vector
begin
	r.x = u.x * s;
	r.y = u.y * s;
	r.z = u.z * s;
end

operator ^ (u, v : vector) = r : vtype
begin
	r = u.x * v.x + u.y * v.y + u.z * v.z;
end

procedure set(var v : vector; x, y, z : vtype)
begin
	v.x = x; v.y = y; v.z = z;
end

operator + (u, v : vector) = r : vector
begin
	r.x = u.x + v.x;
	r.y = u.y + v.y;
	r.z = u.z + v.z;
end

procedure main
begin
	var v : array[0..2] of vector

	set(v[0], 1, 1, 1);
	set(v[1], 1, 1, 1);
	
	write( v[0] + v[1] );
end
