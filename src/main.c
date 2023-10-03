/* 

	This file is part of asciigl.

	asciigl is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

	asciigl is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	You should have received a copy of the GNU General Public License along with asciigl. If not, see <https://www.gnu.org/licenses/>.

*/

#include "./../include/asciigl.h"

int main(int argc, char* argv[]){
	asciigl_init();

	Mesh *msh = load_mesh( "cube.obj" );
	Model *mdl = gen_model();
	mdl->mesh = msh;

	system( "PAUSE" );

	Vec3 position = {
		0,
		0,
		10
	}, rotation = {
		0,
		0,
		0
	};

	mdl->position = position;
	mdl->rotation = rotation;

	RGB white = { 255, 255, 255 };
	Vec3 normal = { 1, 0, 0 };
	normal = vec3_normalize( normal );

	add_ambient_light( "a_light", 100, white );
	add_directional_light( "light", 155, normal, white );

	while (1){
		asciigl_process_frame();
		mdl->rotation.y += 5;
	}

	return 0;
}

