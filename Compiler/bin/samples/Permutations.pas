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

const N = 8;
var
        num, perm : array[0..31] of integer;
        i : integer;

procedure back(k:integer);
begin
        if ( k >= N ) then
        begin
                for i := 0 to N-1 do write(perm[i], ' ');
                writeln;
                exit;
        end;

        while ( 1 = 1 ) do
        begin

        perm[k] = perm[k] + 1;
        if ( perm[k] = N ) then begin
                perm[k] := -1;
                exit;
        end;

        if ( num[perm[k]] = 0 ) then begin
                num[perm[k]] := 1;
                back(k+1);
                num[perm[k]] := 0;
        end;

        end;
end;

procedure main
begin
        for i:=0 to 31 do perm[i] := -1;
        back(0);
end

