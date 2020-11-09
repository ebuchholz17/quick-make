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
int sizeof_asset_pack_data () {
    return sizeof(asset_pack_data);
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
int sizeof_loaded_animated_mesh_asset () {
    return sizeof(loaded_animated_mesh_asset);
}  
int sizeof_loaded_texture_asset () {
    return sizeof(loaded_texture_asset);
}  
int sizeof_loaded_atlas_asset () {
    return sizeof(loaded_atlas_asset);
}  
int sizeof_render_command_header () {
    return sizeof(render_command_header);
}  
int sizeof_render_command_model () {
    return sizeof(render_command_model);
}  
int sizeof_render_command_animated_model () {
    return sizeof(render_command_animated_model);
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
int sizeof_render_command_background_visualization () {
    return sizeof(render_command_background_visualization);
}
int sizeof_render_command_list () {
    return sizeof(render_command_list);
}  
int sizeof_input_key () {
    return sizeof(input_key);
}  
int sizeof_game_controller_input () {
    return sizeof(game_controller_input);
}  
int sizeof_game_input () {
    return sizeof(game_input);
}  
int sizeof_sound_sample () {
    return sizeof(sound_sample);
}  
int sizeof_platform_options () {
    return sizeof(platform_options);
}  
int sizeof_platform_triggers () {
    return sizeof(platform_triggers);
}  
int sizeof_game_sound_output () {
    return sizeof(game_sound_output);
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
    function ("sizeof_asset_pack_data", &sizeof_asset_pack_data);
    function ("sizeof_float_mesh_attribute", &sizeof_float_mesh_attribute);
    function ("sizeof_int_mesh_attribute", &sizeof_int_mesh_attribute);
    function ("sizeof_loaded_mesh_asset", &sizeof_loaded_mesh_asset);
    function ("sizeof_loaded_animated_mesh_asset", &sizeof_loaded_animated_mesh_asset);
    function ("sizeof_loaded_texture_asset", &sizeof_loaded_texture_asset);
    function ("sizeof_loaded_atlas_asset", &sizeof_loaded_atlas_asset);
    function ("sizeof_render_command_header", &sizeof_render_command_header);
    function ("sizeof_render_command_model", &sizeof_render_command_model);
    function ("sizeof_render_command_animated_model", &sizeof_render_command_animated_model);
    function ("sizeof_render_command_lines", &sizeof_render_command_lines);
    function ("sizeof_render_command_sprite", &sizeof_render_command_sprite);
    function ("sizeof_render_sprite", &sizeof_render_sprite);
    function ("sizeof_render_command_sprite_list", &sizeof_render_command_sprite_list);
    function ("sizeof_render_command_set_camera", &sizeof_render_command_set_camera);
    function ("sizeof_render_command_background_visualization", &sizeof_render_command_background_visualization);
    function ("sizeof_render_command_list", &sizeof_render_command_list);
    function ("sizeof_input_key", &sizeof_input_key);
    function ("sizeof_game_controller_input", &sizeof_game_controller_input);
    function ("sizeof_game_input", &sizeof_game_input);
    function ("sizeof_sound_sample", &sizeof_sound_sample);
    function ("sizeof_platform_options", &sizeof_platform_options);
    function ("sizeof_platform_triggers", &sizeof_platform_triggers);
    function ("sizeof_game_sound_output", &sizeof_game_sound_output);
    function ("sizeof_game_memory", &sizeof_game_memory);
}

