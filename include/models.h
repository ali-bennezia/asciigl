/* 

	This file is part of asciigl.

	asciigl is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

	asciigl is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	You should have received a copy of the GNU General Public License along with asciigl. If not, see <https://www.gnu.org/licenses/>.

*/

#ifndef MODELS_HEADERGUARD
#define MODELS_HEADERGUARD

typedef struct Model Model;

enum PRIMITIVE_TYPE{
	TRIANGLE_PRIMITIVE,
	QUAD_PRIMITIVE
};

int load_model_obj_strategy(const char* path, Model* destination);

#endif