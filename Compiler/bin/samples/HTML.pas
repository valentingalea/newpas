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

procedure write(..) external 0;
procedure printf(s : string; ..) external 2;
procedure read(var c : char) external 3;
function ord(c : char) :integer external 5

procedure main
begin
	var c:char
	var s:string[128];
	var i:integer;

	printf("<HTML><HEAD>\n");
	printf("<META content=\"text/html\"; http-equiv=Content-Type>\n");
	printf("<META content=\"NewPas Compiler\" name=GENERATOR>\n</HEAD><BODY>\n");

	while ord(c) <> -1 do
	begin
		read(c);

		if ((c>='a') and (c<='z')) or
		   ((c>='A') and (c<='Z')) or
		   ((c>='0') and (c<='9')) or
		   (c='_') or (c="\"") then
		begin
			s[i] := c;
			i := i+1
			s[i] := "\0"
		end
		else
		begin
			if i then
			begin
				if (s = "begin") or
				   (s = "end") or
				   (s = "procedure") or
				   (s = "function") or
				   (s = "var" )
				then
					printf("<font color=\"0000FF\">%s</font>", s);
				else
					if (s[0]>="0") and (s[0]<="9") then
						printf("<font color=\"FF0000\">%s</font>", s);
					else
						if s[0] = "\"" then
							printf("<font color=\"00AAAA\">%s</font>", s);
					  else
							printf("%s", s);
				i := 0;
			end

			if c = "\n" then
				write("<br>");
			else
			if c <= ' ' then
				write("&nbsp")
			else
				write(c);
		end
	end

	printf("\n</BODY></HTML>");
end
