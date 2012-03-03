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
 
uses 'stdlib.pas';
uses 'spawn.pas';

(* run on user request, hit ENTER in game *)
procedure OnGameFrame
begin
    // Physics coefficient which controls the influence of the camera's position
    // over the spring force. The stiffer the spring, the closer it will stay to
	// the chased object. (k of Hooke law)
	game_setVar( CAM_SPRING_STIFF, 1700 );

    // Physics coefficient which approximates internal friction of the spring.
    // Sufficient damping will prevent the spring from oscillating infinitely.
	game_setVar( CAM_SPRING_DAMPING, 600 );

    // Mass of the camera body. Heavier objects require stiffer springs with less
    // damping to move at the same rate as lighter objects.
	game_setVar( CAM_MASS, 1 );

	game_setVar( INPUT_ACCELERATION, 10.0 );
	game_setVar( INPUT_BRAKE, 1.0 );
	game_setVar( INPUT_TURNING, 45.0 );
end
