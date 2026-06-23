/**
 ******************************************************************************
 * Xenia : Xbox 360 Emulator Research Project                                 *
 ******************************************************************************
 * Copyright 2026 Ben Vanik. All rights reserved.                             *
 * Released under the BSD license - see LICENSE in the root for more details. *
 ******************************************************************************
 */
#include "xenia/ui/gpu_resolve_experiment.h"
#include "xenia/base/cvar.h"
#include "xenia/base/math.h"
#include "xenia/config.h"
#include "xenia/emulator.h"
#include "xenia/gpu/gpu_flags.h"
#include "xenia/gpu/graphics_system.h"
namespace xe {
namespace ui {
namespace {
template <typename T>
void OverrideCvar(const char* name, T value) {
  if (!cvar::ConfigVars) {
    return;
  }
  auto it = cvar::ConfigVars->find(name);
  if (it == cvar::ConfigVars->end()) {
    return;
  }
  auto* config_var = dynamic_cast<cvar::ConfigVar<T>*>(it->second);
  if (!config_var) {
    return;
  }
  config_var->SetGameConfigValue(value);
}
void SaveBool(Emulator* emulator, const char* name, bool value) {
  config::SaveGameConfigSetting(emulator, "GPU", name, value);
}
void SaveInt32(Emulator* emulator, const char* name, int32_t value) {
  config::SaveGameConfigSetting(emulator, "GPU", name, value);
}
void ClearGpuCaches(Emulator* emulator) {
  if (!emulator) {
    return;
  }
  gpu::GraphicsSystem* graphics_system = emulator->graphics_system();
  if (!graphics_system) {
    return;
  }
  graphics_system->ClearCaches();
  graphics_system->InvalidateGpuMemory();
}
void SetExperimentOverrideInt32(Emulator* emulator, const char* name,
                                int32_t value) {
  OverrideCvar<int32_t>(name, value);
  SaveInt32(emulator, name, value);
}
void SetExperimentOverrideBool(Emulator* emulator, const char* name,
                                bool value) {
  OverrideCvar<bool>(name, value);
  SaveBool(emulator, name, value);
}
constexpr GpuResolveExperimentEdramFormatOption kEdramFormatOptions[] = {
    {-1, "Auto (RB)"},
    {0, "0 k_8_8_8_8"},
    {1, "1 k_8_8_8_8_GAMMA"},
    {2, "2 k_2_10_10_10"},
    {3, "3 k_2_10_10_10_FLOAT (7e3)"},
    {4, "4 k_16_16"},
    {5, "5 k_16_16_16_16"},
    {6, "6 k_16_16_FLOAT"},
    {7, "7 k_16_16_16_16_FLOAT"},
    {10, "10 k_2_10_10_10_AS_10_10_10_10"},
    {12, "12 k_2_10_10_10_FLOAT_AS_16_16_16_16"},
    {14, "14 k_32_FLOAT"},
    {15, "15 k_32_32_FLOAT"},
};
constexpr GpuResolveExperimentTriOption kEdram64bppOptions[] = {
    {-1, "Auto"},
    {0, "32bpp layout"},
    {1, "64bpp layout"},
};
constexpr GpuResolveExperimentTriOption kForcePathOptions[] = {
    {-1, "Auto"},
    {0, "Force Fast"},
    {1, "Force Full"},
};
constexpr GpuResolveExperimentTriOption kDestBppOptions[] = {
    {-1, "Auto"},
    {32, "32bpp Full"},
    {64, "64bpp Full"},
};
constexpr GpuResolveExperimentTriOption kEdramMsaaOptions[] = {
    {-1, "Auto (RB)"},
    {0, "1x MSAA"},
    {1, "2x MSAA"},
    {2, "4x MSAA"},
};
constexpr GpuResolveExperimentTriOption kEdramFillHalfPixelOptions[] = {
    {-1, "Auto"},
    {0, "Off"},
    {1, "On"},
};
constexpr GpuResolveExperimentEdramFormatOption kDestFormatOptions[] = {
    {-1, "Auto (computed)"},
    {6, "6 k_8_8_8_8"},
    {7, "7 k_2_10_10_10"},
    {25, "25 k_16_16"},
    {26, "26 k_16_16_16_16"},
    {31, "31 k_16_16_FLOAT"},
    {32, "32 k_16_16_16_16_FLOAT"},
    {50, "50 k_8_8_8_8_AS_16_16_16_16"},
    {54, "54 k_2_10_10_10_AS_16_16_16_16"},
};
constexpr GpuResolveExperimentTriOption kCopySampleOptions[] = {
    {-1, "Auto (sanitized RB)"},
    {0, "k0"},
    {1, "k1"},
    {2, "k2"},
    {3, "k3"},
    {4, "k01"},
    {5, "k23"},
    {6, "k0123"},
};
}  // namespace
size_t GetGpuResolveExperimentEdramFormatOptionCount() {
  return xe::countof(kEdramFormatOptions);
}
const GpuResolveExperimentEdramFormatOption&
GetGpuResolveExperimentEdramFormatOption(size_t index) {
  if (index >= xe::countof(kEdramFormatOptions)) {
    index = 0;
  }
  return kEdramFormatOptions[index];
}
size_t GetGpuResolveExperimentEdram64bppOptionCount() {
  return xe::countof(kEdram64bppOptions);
}
const GpuResolveExperimentTriOption& GetGpuResolveExperimentEdram64bppOption(
    size_t index) {
  if (index >= xe::countof(kEdram64bppOptions)) {
    index = 0;
  }
  return kEdram64bppOptions[index];
}
size_t GetGpuResolveExperimentForcePathOptionCount() {
  return xe::countof(kForcePathOptions);
}
const GpuResolveExperimentTriOption& GetGpuResolveExperimentForcePathOption(
    size_t index) {
  if (index >= xe::countof(kForcePathOptions)) {
    index = 0;
  }
  return kForcePathOptions[index];
}
size_t GetGpuResolveExperimentDestBppOptionCount() {
  return xe::countof(kDestBppOptions);
}
const GpuResolveExperimentTriOption& GetGpuResolveExperimentDestBppOption(
    size_t index) {
  if (index >= xe::countof(kDestBppOptions)) {
    index = 0;
  }
  return kDestBppOptions[index];
}
size_t GetGpuResolveExperimentEdramMsaaOptionCount() {
  return xe::countof(kEdramMsaaOptions);
}
const GpuResolveExperimentTriOption& GetGpuResolveExperimentEdramMsaaOption(
    size_t index) {
  if (index >= xe::countof(kEdramMsaaOptions)) {
    index = 0;
  }
  return kEdramMsaaOptions[index];
}
size_t GetGpuResolveExperimentEdramFillHalfPixelOptionCount() {
  return xe::countof(kEdramFillHalfPixelOptions);
}
const GpuResolveExperimentTriOption&
GetGpuResolveExperimentEdramFillHalfPixelOption(size_t index) {
  if (index >= xe::countof(kEdramFillHalfPixelOptions)) {
    index = 0;
  }
  return kEdramFillHalfPixelOptions[index];
}
size_t GetGpuResolveExperimentDestFormatOptionCount() {
  return xe::countof(kDestFormatOptions);
}
const GpuResolveExperimentEdramFormatOption&
GetGpuResolveExperimentDestFormatOption(size_t index) {
  if (index >= xe::countof(kDestFormatOptions)) {
    index = 0;
  }
  return kDestFormatOptions[index];
}
size_t GetGpuResolveExperimentCopySampleOptionCount() {
  return xe::countof(kCopySampleOptions);
}
const GpuResolveExperimentTriOption& GetGpuResolveExperimentCopySampleOption(
    size_t index) {
  if (index >= xe::countof(kCopySampleOptions)) {
    index = 0;
  }
  return kCopySampleOptions[index];
}
int FindGpuResolveExperimentEdramFormatComboIndex(int32_t value) {
  for (size_t i = 0; i < xe::countof(kEdramFormatOptions); ++i) {
    if (kEdramFormatOptions[i].value == value) {
      return int(i);
    }
  }
  return 0;
}
int FindGpuResolveExperimentEdram64bppComboIndex(int32_t value) {
  for (size_t i = 0; i < xe::countof(kEdram64bppOptions); ++i) {
    if (kEdram64bppOptions[i].value == value) {
      return int(i);
    }
  }
  return 0;
}
int FindGpuResolveExperimentForcePathComboIndex(int32_t value) {
  for (size_t i = 0; i < xe::countof(kForcePathOptions); ++i) {
    if (kForcePathOptions[i].value == value) {
      return int(i);
    }
  }
  return 0;
}
int FindGpuResolveExperimentDestBppComboIndex(int32_t value) {
  for (size_t i = 0; i < xe::countof(kDestBppOptions); ++i) {
    if (kDestBppOptions[i].value == value) {
      return int(i);
    }
  }
  return 0;
}
int FindGpuResolveExperimentEdramMsaaComboIndex(int32_t value) {
  for (size_t i = 0; i < xe::countof(kEdramMsaaOptions); ++i) {
    if (kEdramMsaaOptions[i].value == value) {
      return int(i);
    }
  }
  return 0;
}
int FindGpuResolveExperimentEdramFillHalfPixelComboIndex(int32_t value) {
  for (size_t i = 0; i < xe::countof(kEdramFillHalfPixelOptions); ++i) {
    if (kEdramFillHalfPixelOptions[i].value == value) {
      return int(i);
    }
  }
  return 0;
}
int FindGpuResolveExperimentDestFormatComboIndex(int32_t value) {
  for (size_t i = 0; i < xe::countof(kDestFormatOptions); ++i) {
    if (kDestFormatOptions[i].value == value) {
      return int(i);
    }
  }
  return 0;
}
int FindGpuResolveExperimentCopySampleComboIndex(int32_t value) {
  for (size_t i = 0; i < xe::countof(kCopySampleOptions); ++i) {
    if (kCopySampleOptions[i].value == value) {
      return int(i);
    }
  }
  return 0;
}
void ResetGpuResolveExperimentOverrides(Emulator* emulator) {
  SetExperimentOverrideInt32(emulator, "resolve_experiment_edram_format", -1);
  SetExperimentOverrideInt32(emulator, "resolve_experiment_edram_64bpp", -1);
  SetExperimentOverrideInt32(emulator, "resolve_experiment_dest_exp_bias",
                             kGpuResolveExperimentDestExpBiasAuto);
  SetExperimentOverrideInt32(emulator, "resolve_experiment_force_path", -1);
  SetExperimentOverrideInt32(emulator, "resolve_experiment_dest_bpp", -1);
  SetExperimentOverrideInt32(emulator, "resolve_experiment_edram_base_tiles",
                             kGpuResolveExperimentAuto);
  SetExperimentOverrideInt32(emulator, "resolve_experiment_edram_pitch_tiles",
                             kGpuResolveExperimentAuto);
  SetExperimentOverrideInt32(emulator, "resolve_experiment_edram_msaa",
                             kGpuResolveExperimentAuto);
  SetExperimentOverrideInt32(
      emulator, "resolve_experiment_edram_fill_half_pixel",
      kGpuResolveExperimentAuto);
  SetExperimentOverrideInt32(emulator, "resolve_experiment_edram_offset_x_div_8",
                             kGpuResolveExperimentAuto);
  SetExperimentOverrideInt32(emulator, "resolve_experiment_edram_offset_y_div_8",
                             kGpuResolveExperimentAuto);
  SetExperimentOverrideInt32(emulator, "resolve_experiment_dest_offset_x_div_8",
                             kGpuResolveExperimentAuto);
  SetExperimentOverrideInt32(emulator, "resolve_experiment_dest_offset_y_div_8",
                             kGpuResolveExperimentAuto);
  SetExperimentOverrideInt32(emulator, "resolve_experiment_copy_dest_swap",
                             kGpuResolveExperimentAuto);
  SetExperimentOverrideInt32(emulator, "resolve_experiment_dest_format",
                             kGpuResolveExperimentAuto);
  SetExperimentOverrideInt32(emulator, "resolve_experiment_copy_sample_select",
                             kGpuResolveExperimentAuto);
  SetExperimentOverrideInt32(emulator, "resolve_experiment_color_exp_bias",
                             kGpuResolveExperimentColorExpBiasAuto);
  SetExperimentOverrideBool(emulator,
                          "resolve_experiment_force_fast_ignore_guards", false);
  SetExperimentOverrideBool(emulator,
                          "resolve_experiment_disable_float16_unorm_full",
                          false);
  ClearGpuCaches(emulator);
}
void ApplyGpuResolveExperimentBaseline(Emulator* emulator) {
  ResetGpuResolveExperimentOverrides(emulator);
  // Same-storage-bpp fast resolve is auto-selected in GetCopyShader; keep the
  // explicit override for titles that still need to bypass fast-path guards.
  SetExperimentOverrideInt32(emulator, "resolve_experiment_force_path", 0);
  SetExperimentOverrideBool(emulator, "resolve_clear_exp_bias_on_zero", true);
  SetExperimentOverrideBool(emulator, "skip_repeat_resolve_to_same_dest", false);
  SetExperimentOverrideBool(emulator, "reload_textures_after_resolve", true);
  SetExperimentOverrideBool(emulator,
                          "resolve_experiment_scene_export_tile_patch", true);
  SetExperimentOverrideBool(emulator,
                          "resolve_experiment_allow_repeat_full64_overwrite",
                          true);
  ClearGpuCaches(emulator);
}
void ApplyGpuResolveExperimentLegacyExpBias(Emulator* emulator) {
  ResetGpuResolveExperimentOverrides(emulator);
  SetExperimentOverrideBool(emulator, "resolve_clear_exp_bias_on_zero", false);
  ClearGpuCaches(emulator);
}
void ApplyGpuResolveExperimentForceFast(Emulator* emulator) {
  ResetGpuResolveExperimentOverrides(emulator);
  SetExperimentOverrideInt32(emulator, "resolve_experiment_force_path", 0);
  SetExperimentOverrideBool(emulator, "resolve_clear_exp_bias_on_zero", true);
  ClearGpuCaches(emulator);
}
}  // namespace ui
}  // namespace xe
