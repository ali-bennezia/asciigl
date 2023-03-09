#ifndef RENDER_HEADERGUARD
#define RENDER_HEADERGUARD

#include "utils.h"

int is_viewspace_position_in_frustum(Vec3 pos, Vec2* clipSpace);

Vec3 worldspace_coords_to_viewspace_coords(Vec3 in);
Vec2 viewspace_coords_to_clipspace_coords(Vec3 in);

IntVec2 clipspace_coords_to_screenspace_coords(Vec2 in);

IntVec2 clipspace_to_screenspace(Vec2 in);

Vec2 scale_normal_axis(float normal_x, float normal_y, float scale_x, float scale_y);
Vec3 scale_normal(Vec3 normal, Vec3 scale);

void rasterize_and_draw_primitive(Vec3 a, Vec3 b, Vec3 c, Vec3* normals, Vec2* UVs, Texture* tex);
void draw_model(Model model);
void draw_fragment(int x, int y, float depth, Vec3 viewspacePosition, Vec3* normal, Vec2* UV, Texture* tex);

void clear_console();
RGBA sample_texture(float UV_x, float UV_y, const Texture* tex);

#endif
