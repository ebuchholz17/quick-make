// WebIDL bindings
#include "../../game/quickmake_platform.h"

#include "bindings.cpp"

// addition embind bindings
#include <emscripten/bind.h>

using namespace emscripten;

// math.h
int sizeof_matrix3x3 () {
    return sizeof(matrix3x3);
}  
int sizeof_matrix4x4 () {
    return sizeof(matrix4x4);
}  
int sizeof_vector2 () {
    return sizeof(vector2);
}  
int sizeof_vector3 () {
    return sizeof(vector3);
}  
int sizeof_vector4 () {
    return sizeof(vector4);
}  
int sizeof_quaternion () {
    return sizeof(quaternion);
}  
int sizeof_aabb () {
    return sizeof(aabb);
}  
int sizeof_sphere () {
    return sizeof(sphere);
}  
int sizeof_plane () {
    return sizeof(plane);
}  
int sizeof_triangle () {
    return sizeof(triangle);
}  
int sizeof_line () {
    return sizeof(line);
}  

// platform.h
int sizeof_memory_arena () {
    return sizeof(memory_arena);
}  
int sizeof_asset_to_load () {
    return sizeof(asset_to_load);
}  
int sizeof_asset_list () {
    return sizeof(asset_list);
}  
int sizeof_float_mesh_attribute () {
    return sizeof(float_mesh_attribute);
}  
int sizeof_int_mesh_attribute () {
    return sizeof(int_mesh_attribute);
}  
int sizeof_loaded_mesh_asset () {
    return sizeof(loaded_mesh_asset);
}  
int sizeof_loaded_texture_asset () {
    return sizeof(loaded_mesh_asset);
}  
int sizeof_render_command_header () {
    return sizeof(render_command_header);
}  
int sizeof_render_command_model () {
    return sizeof(render_command_model);
}  
int sizeof_render_command_lines () {
    return sizeof(render_command_lines);
}  
int sizeof_render_command_sprite () {
    return sizeof(render_command_sprite);
}  
int sizeof_render_sprite () {
    return sizeof(render_sprite);
}  
int sizeof_render_command_sprite_list () {
    return sizeof(render_command_sprite_list);
}  
int sizeof_render_command_set_camera () {
    return sizeof(render_command_set_camera);
}
int sizeof_render_command_list () {
    return sizeof(render_command_list);
}  
int sizeof_game_input () {
    return sizeof(game_input);
}  
int sizeof_game_memory () {
    return sizeof(game_memory);
}  

EMSCRIPTEN_BINDINGS(bindings) {
    function ("sizeof_matrix3x3", &sizeof_matrix3x3);
    function ("sizeof_matrix4x4", &sizeof_matrix4x4);
    function ("sizeof_vector3", &sizeof_vector3);
    function ("sizeof_quaternion", &sizeof_quaternion);
    function ("sizeof_aabb", &sizeof_aabb);
    function ("sizeof_sphere", &sizeof_sphere);
    function ("sizeof_plane", &sizeof_plane);
    function ("sizeof_triangle", &sizeof_triangle);
    function ("sizeof_line", &sizeof_line);
    function ("sizeof_memory_arena", &sizeof_memory_arena);
    function ("sizeof_asset_to_load", &sizeof_asset_to_load);
    function ("sizeof_asset_list", &sizeof_asset_list);
    function ("sizeof_float_mesh_attribute", &sizeof_float_mesh_attribute);
    function ("sizeof_int_mesh_attribute", &sizeof_int_mesh_attribute);
    function ("sizeof_loaded_mesh_asset", &sizeof_loaded_mesh_asset);
    function ("sizeof_loaded_texture_asset", &sizeof_loaded_texture_asset);
    function ("sizeof_render_command_header", &sizeof_render_command_header);
    function ("sizeof_render_command_model", &sizeof_render_command_model);
    function ("sizeof_render_command_lines", &sizeof_render_command_lines);
    function ("sizeof_render_command_sprite", &sizeof_render_command_sprite);
    function ("sizeof_render_sprite", &sizeof_render_sprite);
    function ("sizeof_render_command_sprite_list", &sizeof_render_command_sprite_list);
    function ("sizeof_render_command_set_camera", &sizeof_render_command_set_camera);
    function ("sizeof_render_command_list", &sizeof_render_command_list);
    function ("sizeof_game_input", &sizeof_game_input);
    function ("sizeof_game_memory", &sizeof_game_memory);
}

