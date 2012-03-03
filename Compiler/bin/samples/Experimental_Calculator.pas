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

procedure writeln(..) external 1;
procedure read(c : char) external 3
procedure val(s : string; var n : integer) external 4

var
	i : char
	rez : real;

procedure skip_white
begin
	while i <= ' ' do begin read(i) end
end;

procedure expression(var value:real);forward;

procedure get_num(var value : real);
begin
	var n:integer
    value := 0.0;
	skip_white;
	if ( i = '(' ) then
	begin
		read(i)
		expression(value);
		if ( i <> ')' ) then writeln("')' expected");
		read(i);
	end
	else
	while ( i >= '0' ) and ( i <= '9' ) do
	begin
		val(i, n);
	    value := 10.0 * value + n;
	    read(i);
	end;
end;

procedure term(var value:real);
begin
	var n:real;
	get_num(value);
	while ( i = '*' ) or ( i = '/' ) do
	case i of
	'*' :
		begin
			read(i);
			get_num(n);
			value := value * n;
		end
	'/' :
		begin
			read(i);
			get_num(n);
			value := value / n;
		end
	end
end;

procedure expression(var value:real);
begin
	var n:real;
	term(value);
	while ( i = '+' ) or ( i = '-' ) do
	case i of
	'+' :
		begin
			read(i);
			term(n);
			value := value + n;
		end
	'-' :
		begin
			read(i);
			term(n);
			value := value - n;
		end
	end
end;


procedure main;
begin
	read(i);
	expression(rez);
	writeln(rez);
end
