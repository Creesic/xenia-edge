/**
 ******************************************************************************
 * Xenia : Xbox 360 Emulator Research Project                                 *
 ******************************************************************************
 * Copyright 2026 Ben Vanik. All rights reserved.                             *
 * Released under the BSD license - see LICENSE in the root for more details. *
 ******************************************************************************
 */

#ifndef XENIA_UI_IMGUI_DEBUG_SETTINGS_WINDOW_H_
#define XENIA_UI_IMGUI_DEBUG_SETTINGS_WINDOW_H_

#include <functional>
#include <memory>

#include "xenia/ui/imgui_drawer.h"
#include "xenia/ui/immediate_drawer.h"
#include "xenia/ui/presenter.h"
#include "xenia/ui/window.h"
#include "xenia/ui/windowed_app_context.h"

namespace xe {
namespace app {
class EmulatorWindow;
}  // namespace app
}  // namespace xe

namespace xe {
namespace ui {

class ImGuiDebugDialog;

// Standalone OS window hosting the F8 debug settings UI.
class ImGuiDebugSettingsWindow {
 public:
  virtual ~ImGuiDebugSettingsWindow();

  static std::unique_ptr<ImGuiDebugSettingsWindow> Create(
      app::EmulatorWindow* emulator_window,
      xe::ui::WindowedAppContext& app_context);

  bool IsOpen() const { return window_ != nullptr; }

  void Close();

  void SetOnClosedCallback(std::function<void()> callback) {
    on_closed_callback_ = std::move(callback);
  }

 private:
  class WindowListener;

  ImGuiDebugSettingsWindow(app::EmulatorWindow* emulator_window,
                           xe::ui::WindowedAppContext& app_context);

  bool Initialize();
  void HandleWindowClosing();
  void HandleDialogClosed();

  app::EmulatorWindow* emulator_window_;
  xe::ui::WindowedAppContext& app_context_;
  std::unique_ptr<WindowListener> window_listener_;
  std::unique_ptr<xe::ui::Window> window_;
  std::unique_ptr<xe::ui::Presenter> presenter_;
  std::unique_ptr<xe::ui::ImmediateDrawer> immediate_drawer_;
  std::unique_ptr<xe::ui::ImGuiDrawer> imgui_drawer_;
  ImGuiDebugDialog* debug_dialog_ = nullptr;
  std::function<void()> on_closed_callback_;
  bool close_requested_ = false;
};

}  // namespace ui
}  // namespace xe

#endif  // XENIA_UI_IMGUI_DEBUG_SETTINGS_WINDOW_H_
