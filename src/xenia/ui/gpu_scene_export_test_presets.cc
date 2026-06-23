/**
 ******************************************************************************
 * Xenia : Xbox 360 Emulator Research Project                                 *
 ******************************************************************************
 * Copyright 2026 Ben Vanik. All rights reserved.                             *
 * Released under the BSD license - see LICENSE in the root for more details. *
 ******************************************************************************
 */
#include "xenia/ui/gpu_scene_export_test_presets.h"
#include "xenia/base/cvar.h"
#include "xenia/base/math.h"
#include "xenia/config.h"
#include "xenia/emulator.h"
#include "xenia/gpu/command_processor.h"
#include "xenia/gpu/gpu_flags.h"
#include "xenia/gpu/graphics_system.h"
#include "xenia/gpu/render_target_cache.h"
namespace xe {
namespace ui {
namespace {
constexpr int kUnset = -1;
template <typename T>
bool OverrideCvar(const char* name, T value) {
  if (!cvar::ConfigVars) {
    return false;
  }
  auto it = cvar::ConfigVars->find(name);
  if (it == cvar::ConfigVars->end()) {
    return false;
  }
  auto* config_var = dynamic_cast<cvar::ConfigVar<T>*>(it->second);
  if (!config_var) {
    return false;
  }
  config_var->SetGameConfigValue(value);
  return true;
}
void SaveBoolSetting(Emulator* emulator, const char* name, bool value) {
  config::SaveGameConfigSetting(emulator, "GPU", name, value);
}
void SaveStringSetting(Emulator* emulator, const char* name,
                       const std::string& value) {
  config::SaveGameConfigSetting(emulator, "GPU", name, value);
}
gpu::ReadbackResolveMode ReadbackModeFromIndex(int index) {
  switch (index) {
    case 0:
      return gpu::ReadbackResolveMode::kDisabled;
    case 1:
      return gpu::ReadbackResolveMode::kSome;
    case 3:
      return gpu::ReadbackResolveMode::kFull;
    default:
      return gpu::ReadbackResolveMode::kFast;
  }
}
const char* ReadbackModeName(int index) {
  switch (index) {
    case 0:
      return "none";
    case 1:
      return "some";
    case 3:
      return "full";
    default:
      return "fast";
  }
}
void ApplyOptionalBool(Emulator* emulator, const char* name, int value) {
  if (value < 0) {
    return;
  }
  const bool enabled = value != 0;
  OverrideCvar<bool>(name, enabled);
  SaveBoolSetting(emulator, name, enabled);
}
// Baseline reload + exp_bias for phase-2 presets (readback fast, sync off).
constexpr int kBaselineReload = 1;
constexpr int kBaselineExpBias = 1;
constexpr int kBaselineReadbackFast = 2;
// Spider-Man: Edge of Time white-scene investigation presets.
constexpr GpuSceneExportTestPreset kPresets[] = {
    {
        "01 Baseline (current fixes)",
        "Eager reload after resolve, clear stale resolve exp_bias, FSI accuracy "
        "path, readback fast. Start here.",
        "accuracy",
        kBaselineReadbackFast,
        kBaselineReload,
        kBaselineExpBias,
        kUnset,
        kUnset,
        1,
        1,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        0,
        false,
    },
    {
        "02 No eager texture reload",
        "Disable reload_textures_after_resolve. Composite may sample stale GPU "
        "scene texture before the deferred upload.",
        nullptr,
        kUnset,
        0,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        false,
    },
    {
        "03 Legacy resolve exp_bias",
        "Apply RB copy_dest_exp_bias even when color_exp_bias is 0 (pre-fix "
        "behavior). May blow out UNorm resolve exports.",
        nullptr,
        kUnset,
        kUnset,
        0,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        false,
    },
    {
        "04 No reload + legacy exp_bias",
        "Combines stale-texture reload timing with legacy exp_bias handling.",
        nullptr,
        kUnset,
        0,
        0,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        false,
    },
    {
        "05 Readback resolve: none",
        "Skip CPU readback after resolve. GPU guest buffer still updated by "
        "resolve shader.",
        nullptr,
        0,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        false,
    },
    {
        "06 Readback resolve: full",
        "Wait for GPU and copy resolve results to CPU guest every time.",
        nullptr,
        3,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        false,
    },
    {
        "07 Half-pixel offset off",
        "Disable PA_SU_VTX_CNTL half-pixel offset support.",
        nullptr,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        0,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        false,
    },
    {
        "08 Readback half-pixel offset on",
        "Sample scaled resolve blocks from center when resolution scaling is "
        "active.",
        nullptr,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        1,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        false,
    },
    {
        "09 Gamma UNorm16 off",
        "Store gamma RTs as 8-bit UNorm instead of 16-bit UNorm in FSI path.",
        nullptr,
        kUnset,
        kUnset,
        kUnset,
        0,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        false,
    },
    {
        "10 Linear shared memory",
        "Disable tiled/sparse guest memory (RenderDoc-friendly, may change "
        "addressing).",
        nullptr,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        0,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        false,
    },
    {
        "11 GPU debug markers on",
        "Force RenderDoc marker injection for capture correlation.",
        nullptr,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        1,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        false,
    },
    {
        "12 Performance RT path",
        "Host render targets (FBO/RTV) instead of FSI/ROV. Restart title "
        "after applying.",
        "performance",
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        true,
    },
    {
        "13 Accuracy RT path",
        "FSI/ROV manual EDRAM path (Spider-Man default). Restart title after "
        "applying.",
        "accuracy",
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        true,
    },
    {
        "14 All runtime fixes off",
        "No eager reload, legacy exp_bias, readback none.",
        nullptr,
        0,
        0,
        0,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        false,
    },
    {
        "15 Max runtime accuracy",
        "All runtime fixes on plus readback full and gamma UNorm16.",
        "accuracy",
        3,
        1,
        1,
        1,
        kUnset,
        1,
        1,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        false,
    },
    {
        "16 Sync after resolve reload",
        "Baseline fixes plus GPU barriers after post-resolve texture uploads "
        "(composite cannot sample in-flight data).",
        nullptr,
        kBaselineReadbackFast,
        kBaselineReload,
        kBaselineExpBias,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        1,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        false,
    },
    {
        "17 Await GPU after resolve",
        "Block PM4 until resolve + texture uploads finish on the GPU.",
        nullptr,
        kBaselineReadbackFast,
        kBaselineReload,
        kBaselineExpBias,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        1,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        false,
    },
    {
        "18 Reload on mark resolved",
        "Upload textures inside MarkRangeAsResolved (during resolve dispatch) "
        "instead of only after IssueCopy returns.",
        nullptr,
        kBaselineReadbackFast,
        kBaselineReload,
        kBaselineExpBias,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        1,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        false,
    },
    {
        "19 Invalidate bindings after resolve",
        "Force every texture slot to rebind/reload after each resolve.",
        nullptr,
        kBaselineReadbackFast,
        kBaselineReload,
        kBaselineExpBias,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        1,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        false,
    },
    {
        "20 End submission after resolve",
        "Close the GPU submission immediately after each resolve.",
        nullptr,
        kBaselineReadbackFast,
        kBaselineReload,
        kBaselineExpBias,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        1,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        false,
    },
    {
        "21 Aggressive GPU sync",
        "Barriers + full GPU wait + invalidate bindings after each resolve.",
        nullptr,
        kBaselineReadbackFast,
        kBaselineReload,
        kBaselineExpBias,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        1,
        1,
        kUnset,
        1,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        false,
    },
    {
        "22 Submit on primary buffer end",
        "Flush GPU work at every primary buffer boundary (PM4 ordering).",
        nullptr,
        kBaselineReadbackFast,
        kBaselineReload,
        kBaselineExpBias,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        1,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        false,
    },
    {
        "23 Scaled texture offsets off",
        "Disable draw_resolution_scaled_texture_offsets shader path.",
        nullptr,
        kBaselineReadbackFast,
        kBaselineReload,
        kBaselineExpBias,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        0,
        kUnset,
        kUnset,
        kUnset,
        false,
    },
    {
        "24 Mark reload + sync barriers",
        "Early reload at MarkRangeAsResolved plus post-reload barriers.",
        nullptr,
        kBaselineReadbackFast,
        kBaselineReload,
        kBaselineExpBias,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        1,
        kUnset,
        1,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        false,
    },
    {
        "25 Async shader compile off",
        "Compile pipelines synchronously (eliminates async reordering).",
        nullptr,
        kBaselineReadbackFast,
        kBaselineReload,
        kBaselineExpBias,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        0,
        kUnset,
        kUnset,
        false,
    },
    {
        "26 No discard stencil transfers",
        "Keep stencil in transfer pipelines (no_discard_stencil_in_transfer).",
        nullptr,
        kBaselineReadbackFast,
        kBaselineReload,
        kBaselineExpBias,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        1,
        kUnset,
        false,
    },
    {
        "27 Full pipeline flush",
        "Await GPU + end submission + all reload/sync/invalidate options.",
        nullptr,
        kBaselineReadbackFast,
        kBaselineReload,
        kBaselineExpBias,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        1,
        1,
        1,
        1,
        1,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        false,
    },
    {
        "28 Baseline + sync + invalidate",
        "Most likely fix if composite samples stale bound textures.",
        nullptr,
        kBaselineReadbackFast,
        kBaselineReload,
        kBaselineExpBias,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        1,
        kUnset,
        kUnset,
        1,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        false,
    },
    {
        "29 Readback full + await GPU",
        "Full CPU readback plus GPU drain after every resolve.",
        nullptr,
        3,
        kBaselineReload,
        kBaselineExpBias,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        1,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        false,
    },
    {
        "30 Nuclear sync (all on)",
        "Every reload/sync/invalidate/submit option enabled at once.",
        nullptr,
        3,
        1,
        1,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        1,
        1,
        1,
        1,
        1,
        1,
        0,
        0,
        1,
        kUnset,
        false,
    },
    {
        "31 Skip 2nd resolve to same dest",
        "Skip repeat EDRAM copy only; first resolve keeps guest export and "
        "its GPU texture upload (no reload on skipped repeats).",
        "accuracy",
        kBaselineReadbackFast,
        kBaselineReload,
        kBaselineExpBias,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        1,
        false,
    },
    {
        "32 Skip repeat + await sync reload",
        "Preset 31 plus await/sync after each actual resolve copy (not on "
        "skipped repeats). Try 01 first if 31 is still black.",
        "accuracy",
        kBaselineReadbackFast,
        kBaselineReload,
        kBaselineExpBias,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        1,
        1,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        kUnset,
        1,
        false,
    },
};
static_assert(xe::countof(kPresets) > 0, "Need at least one GPU test preset");
}  // namespace
size_t GetGpuSceneExportTestPresetCount() { return xe::countof(kPresets); }
const GpuSceneExportTestPreset& GetGpuSceneExportTestPreset(size_t index) {
  if (index >= xe::countof(kPresets)) {
    index = 0;
  }
  return kPresets[index];
}
GpuSceneExportTestApplyResult ApplyGpuSceneExportTestPreset(size_t index,
                                                            Emulator* emulator) {
  GpuSceneExportTestApplyResult result;
  const GpuSceneExportTestPreset& preset = GetGpuSceneExportTestPreset(index);
  result.title = preset.name;
  if (preset.render_target_path) {
    OverrideCvar<std::string>("render_target_path", preset.render_target_path);
    SaveStringSetting(emulator, "render_target_path", preset.render_target_path);
  }
  ApplyOptionalBool(emulator, "reload_textures_after_resolve",
                    preset.reload_textures_after_resolve);
  ApplyOptionalBool(emulator, "resolve_clear_exp_bias_on_zero",
                    preset.resolve_clear_exp_bias_on_zero);
  ApplyOptionalBool(emulator, "gamma_render_target_as_unorm16",
                    preset.gamma_render_target_as_unorm16);
  ApplyOptionalBool(emulator, "readback_resolve_half_pixel_offset",
                    preset.readback_resolve_half_pixel_offset);
  ApplyOptionalBool(emulator, "half_pixel_offset", preset.half_pixel_offset);
  ApplyOptionalBool(emulator, "tiled_shared_memory", preset.tiled_shared_memory);
  ApplyOptionalBool(emulator, "gpu_debug_markers", preset.gpu_debug_markers);
  ApplyOptionalBool(emulator, "sync_after_resolve_texture_reload",
                    preset.sync_after_resolve_texture_reload);
  ApplyOptionalBool(emulator, "await_gpu_after_resolve",
                    preset.await_gpu_after_resolve);
  ApplyOptionalBool(emulator, "reload_textures_on_mark_resolved",
                    preset.reload_textures_on_mark_resolved);
  ApplyOptionalBool(emulator, "invalidate_texture_bindings_after_resolve",
                    preset.invalidate_texture_bindings_after_resolve);
  ApplyOptionalBool(emulator, "end_submission_after_resolve",
                    preset.end_submission_after_resolve);
  ApplyOptionalBool(emulator, "submit_on_primary_buffer_end",
                    preset.submit_on_primary_buffer_end);
  ApplyOptionalBool(emulator, "draw_resolution_scaled_texture_offsets",
                    preset.draw_resolution_scaled_texture_offsets);
  ApplyOptionalBool(emulator, "async_shader_compilation",
                    preset.async_shader_compilation);
  ApplyOptionalBool(emulator, "no_discard_stencil_in_transfer_pipelines",
                    preset.no_discard_stencil_in_transfer_pipelines);
  ApplyOptionalBool(emulator, "skip_repeat_resolve_to_same_dest",
                    preset.skip_repeat_resolve_to_same_dest);
  if (preset.readback_resolve_mode >= 0 && emulator) {
    gpu::GraphicsSystem* graphics_system = emulator->graphics_system();
    if (graphics_system && graphics_system->command_processor()) {
      graphics_system->command_processor()->SetReadbackResolveMode(
          ReadbackModeFromIndex(preset.readback_resolve_mode));
    }
  }
  if (emulator) {
    gpu::GraphicsSystem* graphics_system = emulator->graphics_system();
    if (graphics_system) {
      graphics_system->ClearCaches();
      graphics_system->InvalidateGpuMemory();
      if (gpu::CommandProcessor* command_processor =
              graphics_system->command_processor()) {
        command_processor->CallInThread([command_processor]() {
          command_processor->ClearReadbackBuffers();
        });
      }
    }
  }
  result.detail = preset.summary;
  if (preset.notes_restart) {
    result.detail +=
        "\n\nNote: render_target_path changed — restart the title for it to "
        "take full effect.";
  }
  if (preset.readback_resolve_mode >= 0) {
    result.detail += std::string("\nreadback_resolve=") +
                     ReadbackModeName(preset.readback_resolve_mode);
  }
  result.success = true;
  return result;
}
}  // namespace ui
}  // namespace xe
