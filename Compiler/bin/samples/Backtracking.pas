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
procedure writeln(..) external 1;
function ord(c:char):integer external 5;

const N = 4;
var
	_succ, st : array[0..255] of char;
	_num : array[0..255] of integer;
	nl, nr : integer;

procedure inc( var i:integer ) begin i := i + 1; end;
procedure dec( var i:integer ) begin i := i - 1; end;

procedure back(k : integer)
var i : integer;
begin        
	if ( k >= N * 2 ) then begin
		writeln(st);
		exit;
	end;

	while ( 1 = 1 ) do
	begin

	dec(_num[ ord(st[k]) ]);
	st[k] := _succ[ ord(st[k]) ];
	inc(_num[ ord(st[k]) ]);
	if ( st[k] = ' ' ) then
		exit;

	if ((st[0] = '(') and
		( _num[ ord('(') ] <= N ) and
		( _num[ ord(')') ] <= N ) and
		(_num[ord('(')] >= _num[ord(')')] ) )
	then
		back(k+1);
	end;
end;

procedure main;
begin
	var i : integer;
	for i:=0 to 255 do begin
		st[I] := ' ';
		_succ[I] := ' ';
	end;

	_succ[ord(' ')] := '(';
	_succ[ord('(')] := ')';
	_succ[ord(')')] := ' ';

	back(0);
end