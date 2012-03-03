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
 * actions ran once at world create
 * compile 'game.pas' after modifing this
 *)

procedure OnGameStart
begin
	game_createBody( SPHERE, 1, 0.5, 1, 1, '3 2 5', 0, '0 1 0' );
	game_createBody( BOX, 1, 1, 1, 1, '4 2 3', 0, '0 1 0' );

	game_setVar( CAR_POSITION, '0 0.278 0' );
	game_setVar( CAR_MASS, 10.0 );
	game_setVar( CAR_ANTISWAYFORCE, 300.0 );

	game_setVar( WHEEL_STEERLOCK, 30.0 ); (* max degrees of wheel steering *)
	game_setVar( WHEEL_SUSPENSION_ERP, 0.15 );
	game_setVar( WHEEL_SUSPENSION_CFM, 0.03 );
end
