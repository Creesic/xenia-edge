/**
 ******************************************************************************
 * Xenia : Xbox 360 Emulator Research Project                                 *
 ******************************************************************************
 * Copyright 2026 Ben Vanik. All rights reserved.                             *
 * Released under the BSD license - see LICENSE in the root for more details. *
 ******************************************************************************
 */
#ifndef XENIA_UI_GPU_RESOLVE_EXPERIMENT_H_
#define XENIA_UI_GPU_RESOLVE_EXPERIMENT_H_
#include <cstddef>
#include <cstdint>
namespace xe {
class Emulator;
}  // namespace xe
namespace xe {
namespace ui {
struct GpuResolveExperimentEdramFormatOption {
  int32_t value;
  const char* label;
};
struct GpuResolveExperimentTriOption {
  int32_t value;
  const char* label;
};
constexpr int32_t kGpuResolveExperimentAuto = -1;
constexpr int32_t kGpuResolveExperimentExpBiasAuto = -999;
constexpr int32_t kGpuResolveExperimentDestExpBiasAuto =
    kGpuResolveExperimentExpBiasAuto;
constexpr int32_t kGpuResolveExperimentColorExpBiasAuto =
    kGpuResolveExperimentExpBiasAuto;
size_t GetGpuResolveExperimentEdramFormatOptionCount();
const GpuResolveExperimentEdramFormatOption&
GetGpuResolveExperimentEdramFormatOption(size_t index);
size_t GetGpuResolveExperimentEdram64bppOptionCount();
const GpuResolveExperimentTriOption& GetGpuResolveExperimentEdram64bppOption(
    size_t index);
size_t GetGpuResolveExperimentForcePathOptionCount();
const GpuResolveExperimentTriOption& GetGpuResolveExperimentForcePathOption(
    size_t index);
size_t GetGpuResolveExperimentDestBppOptionCount();
const GpuResolveExperimentTriOption& GetGpuResolveExperimentDestBppOption(
    size_t index);
size_t GetGpuResolveExperimentEdramMsaaOptionCount();
const GpuResolveExperimentTriOption& GetGpuResolveExperimentEdramMsaaOption(
    size_t index);
size_t GetGpuResolveExperimentEdramFillHalfPixelOptionCount();
const GpuResolveExperimentTriOption&
GetGpuResolveExperimentEdramFillHalfPixelOption(size_t index);
size_t GetGpuResolveExperimentDestFormatOptionCount();
const GpuResolveExperimentEdramFormatOption&
GetGpuResolveExperimentDestFormatOption(size_t index);
size_t GetGpuResolveExperimentCopySampleOptionCount();
const GpuResolveExperimentTriOption& GetGpuResolveExperimentCopySampleOption(
    size_t index);
int FindGpuResolveExperimentEdramFormatComboIndex(int32_t value);
int FindGpuResolveExperimentEdram64bppComboIndex(int32_t value);
int FindGpuResolveExperimentForcePathComboIndex(int32_t value);
int FindGpuResolveExperimentDestBppComboIndex(int32_t value);
int FindGpuResolveExperimentEdramMsaaComboIndex(int32_t value);
int FindGpuResolveExperimentEdramFillHalfPixelComboIndex(int32_t value);
int FindGpuResolveExperimentDestFormatComboIndex(int32_t value);
int FindGpuResolveExperimentCopySampleComboIndex(int32_t value);
void ResetGpuResolveExperimentOverrides(Emulator* emulator);
void ApplyGpuResolveExperimentBaseline(Emulator* emulator);
void ApplyGpuResolveExperimentLegacyExpBias(Emulator* emulator);
void ApplyGpuResolveExperimentForceFast(Emulator* emulator);
}  // namespace ui
}  // namespace xe
#endif  // XENIA_UI_GPU_RESOLVE_EXPERIMENT_H_
