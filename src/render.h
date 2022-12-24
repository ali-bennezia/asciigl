#ifndef RENDER_HEADERGUARD
#define RENDER_HEADERGUARD

#include "utils.h"

int is_viewspace_position_in_frustum(Vec3 pos, Vec2* clipSpace);
Vec3 worldspace_coords_to_viewspace_coords(Vec3 in);
Vec2 viewspace_coords_to_clipspace_coords(Vec3 in);
IntVec2 clipspace_coords_to_screenspace_coords(Vec2 in);
IntVec2 clipspace_to_screenspace(Vec2 in);
void rasterize_and_draw_primitive(Vec3 a, Vec3 b, Vec3 c, Vec3* normals);
void draw_model(Model model);

#endif