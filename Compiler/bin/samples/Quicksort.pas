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

procedure write(..) external 0
procedure writeln(..) external 1
procedure printf(s : string; ..) external 2
procedure randomize() external 7
function random(i : integer) : integer external 8

const
  Max = 500

type
  List = array[1 .. Max] of Integer;

var
  a: List;
  I: Integer;

procedure QuickSort(var a: list; L, R: Integer);
var
  i, j, x, y: integer;
begin
  i := l; j := r; x := a[(l+r) DIV 2];
  repeat
    while a[i] < x do i := i + 1;
    while x < a[j] do j := j - 1;
    if i <= j then
    begin
      y := a[i]; a[i] := a[j]; a[j] := y;
      i := i + 1; j := j - 1;
    end;
  until i > j;
  if l < j then QuickSort(a, l, j);
  if i < r then QuickSort(a, i, r);
end;


procedure main
begin
  Write('Now generating ', max, ' random numbers...');
  Randomize;
  for i := 1 to Max do a[i] := Random(3000);
  Writeln;
  Write('Now sorting random numbers...');
  QuickSort(a, 1, Max);
  Writeln;
  for i := 1 to max do printf("%5i", a[i]);
  writeln;
end

