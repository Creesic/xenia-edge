/**
 ******************************************************************************
 * Xenia : Xbox 360 Emulator Research Project                                 *
 ******************************************************************************
 * Copyright 2026 Ben Vanik. All rights reserved.                             *
 * Released under the BSD license - see LICENSE in the root for more details. *
 ******************************************************************************
 */
#ifndef XENIA_UI_GPU_SCENE_EXPORT_TEST_PRESETS_H_
#define XENIA_UI_GPU_SCENE_EXPORT_TEST_PRESETS_H_
#include <cstddef>
#include <string>
namespace xe {
class Emulator;
}  // namespace xe
namespace xe {
namespace ui {
// Runtime-tweakable GPU settings exercised by each preset. -1 = leave unchanged.
struct GpuSceneExportTestPreset {
  const char* name;
  const char* summary;
  const char* render_target_path;  // nullptr = unchanged
  int readback_resolve_mode;       // -1 unchanged; 0 none, 1 some, 2 fast, 3 full
  int reload_textures_after_resolve;
  int resolve_clear_exp_bias_on_zero;
  int gamma_render_target_as_unorm16;
  int readback_resolve_half_pixel_offset;
  int half_pixel_offset;
  int tiled_shared_memory;
  int gpu_debug_markers;
  int sync_after_resolve_texture_reload;
  int await_gpu_after_resolve;
  int reload_textures_on_mark_resolved;
  int invalidate_texture_bindings_after_resolve;
  int end_submission_after_resolve;
  int submit_on_primary_buffer_end;
  int draw_resolution_scaled_texture_offsets;
  int async_shader_compilation;
  int no_discard_stencil_in_transfer_pipelines;
  int skip_repeat_resolve_to_same_dest;
  bool notes_restart;
};
struct GpuSceneExportTestApplyResult {
  bool success = false;
  std::string title;
  std::string detail;
};
size_t GetGpuSceneExportTestPresetCount();
const GpuSceneExportTestPreset& GetGpuSceneExportTestPreset(size_t index);
GpuSceneExportTestApplyResult ApplyGpuSceneExportTestPreset(size_t index,
                                                          Emulator* emulator);
}  // namespace ui
}  // namespace xe
#endif  // XENIA_UI_GPU_SCENE_EXPORT_TEST_PRESETS_H_
