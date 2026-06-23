/**
 ******************************************************************************
 * Xenia : Xbox 360 Emulator Research Project                                 *
 ******************************************************************************
 * Copyright 2020 Ben Vanik. All rights reserved.                             *
 * Released under the BSD license - see LICENSE in the root for more details. *
 ******************************************************************************
 */

#ifndef XENIA_GPU_GPU_FLAGS_H_
#define XENIA_GPU_GPU_FLAGS_H_
#include "xenia/base/cvar.h"

DECLARE_path(trace_gpu_prefix);
DECLARE_bool(trace_gpu_stream);

DECLARE_path(dump_shaders);

DECLARE_bool(guest_display_refresh_cap);

DECLARE_uint32(framerate_limit);

void SetGuestDisplayRefreshCap(bool value);
void SetFramerateLimit(uint32_t value);

DECLARE_bool(gpu_allow_invalid_fetch_constants);

DECLARE_bool(non_seamless_cube_map);

DECLARE_bool(half_pixel_offset);

DECLARE_string(occlusion_query);

DECLARE_int32(occlusion_query_fake_lower_threshold);

DECLARE_int32(occlusion_query_fake_upper_threshold);

DECLARE_bool(occlusion_query_log);

DECLARE_int32(occlusion_query_querybatch_range);

DECLARE_double(occlusion_query_saturation);

// Returns the guest vblank rate in Hz (50 for PAL, 60 for NTSC).
// Based on use_50Hz_mode cvar.
uint32_t GetGuestVblankRateHz();

DECLARE_int32(anisotropic_override);

DECLARE_bool(disassemble_pm4);

DECLARE_bool(gpu_debug_markers);

// Returns true if GPU debug markers should be enabled.
// Checks the CVAR and also detects if RenderDoc is attached.
// Result is cached after first call for efficiency.
bool IsGpuDebugMarkersEnabled();

DECLARE_string(render_target_path);

DECLARE_bool(no_discard_stencil_in_transfer_pipelines);

DECLARE_bool(submit_on_primary_buffer_end);

DECLARE_bool(async_shader_compilation);

DECLARE_bool(readback_resolve_half_pixel_offset);

DECLARE_bool(reload_textures_after_resolve);

DECLARE_bool(reload_textures_on_mark_resolved);

DECLARE_bool(sync_after_resolve_texture_reload);

DECLARE_bool(await_gpu_after_resolve);

DECLARE_bool(invalidate_texture_bindings_after_resolve);

DECLARE_bool(end_submission_after_resolve);

DECLARE_bool(skip_repeat_resolve_to_same_dest);

DECLARE_bool(resolve_clear_exp_bias_on_zero);

// Live resolve/scene-export experiments (Debug Settings panel). -1/-999 = off.
DECLARE_int32(resolve_experiment_edram_format);
DECLARE_int32(resolve_experiment_edram_64bpp);
DECLARE_int32(resolve_experiment_dest_exp_bias);
DECLARE_int32(resolve_experiment_force_path);
DECLARE_int32(resolve_experiment_dest_bpp);
DECLARE_int32(resolve_experiment_edram_base_tiles);
DECLARE_int32(resolve_experiment_edram_pitch_tiles);
DECLARE_int32(resolve_experiment_edram_msaa);
DECLARE_int32(resolve_experiment_edram_fill_half_pixel);
DECLARE_int32(resolve_experiment_edram_offset_x_div_8);
DECLARE_int32(resolve_experiment_edram_offset_y_div_8);
DECLARE_int32(resolve_experiment_dest_offset_x_div_8);
DECLARE_int32(resolve_experiment_dest_offset_y_div_8);
DECLARE_int32(resolve_experiment_copy_dest_swap);
DECLARE_int32(resolve_experiment_dest_format);
DECLARE_int32(resolve_experiment_copy_sample_select);
DECLARE_int32(resolve_experiment_color_exp_bias);
DECLARE_bool(resolve_experiment_force_fast_ignore_guards);
DECLARE_bool(resolve_experiment_disable_float16_unorm_full);
DECLARE_bool(resolve_experiment_scene_export_tile_patch);
DECLARE_bool(resolve_experiment_allow_repeat_full64_overwrite);

DECLARE_bool(gpu_3d_to_2d_texture);

DECLARE_bool(ac6_ground_fix);

DECLARE_bool(use_fuzzy_alpha_epsilon);

#define XE_GPU_FINE_GRAINED_DRAW_SCOPES 1

#endif  // XENIA_GPU_GPU_FLAGS_H_
