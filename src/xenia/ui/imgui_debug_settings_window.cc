/**
 ******************************************************************************
 * Xenia : Xbox 360 Emulator Research Project                                 *
 ******************************************************************************
 * Copyright 2026 Ben Vanik. All rights reserved.                             *
 * Released under the BSD license - see LICENSE in the root for more details. *
 ******************************************************************************
 */

#include "xenia/ui/imgui_debug_settings_window.h"

#include "xenia/app/emulator_window.h"
#include "xenia/base/logging.h"
#include "xenia/emulator.h"
#include "xenia/gpu/graphics_system.h"
#include "xenia/ui/graphics_provider.h"
#include "xenia/ui/imgui_debug_dialog.h"
#include "xenia/ui/menu_item.h"
#include "xenia/ui/window_listener.h"

namespace xe {
namespace ui {

namespace {

constexpr std::string_view kWindowTitle = "Xenia Debug Settings";

}  // namespace

class ImGuiDebugSettingsWindow::WindowListener final
    : public xe::ui::WindowListener {
 public:
  explicit WindowListener(ImGuiDebugSettingsWindow& owner) : owner_(owner) {}

  void OnClosing(xe::ui::UIEvent& e) override { owner_.HandleWindowClosing(); }

 private:
  ImGuiDebugSettingsWindow& owner_;
};

ImGuiDebugSettingsWindow::ImGuiDebugSettingsWindow(
    app::EmulatorWindow* emulator_window,
    xe::ui::WindowedAppContext& app_context)
    : emulator_window_(emulator_window), app_context_(app_context) {}

ImGuiDebugSettingsWindow::~ImGuiDebugSettingsWindow() {
  app_context_.ExecutePendingFunctionsFromUIThread();

  if (debug_dialog_) {
    debug_dialog_->CloseDialog();
    debug_dialog_ = nullptr;
  }

  if (window_ && window_listener_) {
    window_->RemoveListener(window_listener_.get());
  }
  window_.reset();
  imgui_drawer_.reset();
  immediate_drawer_.reset();
  presenter_.reset();
}

std::unique_ptr<ImGuiDebugSettingsWindow> ImGuiDebugSettingsWindow::Create(
    app::EmulatorWindow* emulator_window,
    xe::ui::WindowedAppContext& app_context) {
  std::unique_ptr<ImGuiDebugSettingsWindow> settings_window(
      new ImGuiDebugSettingsWindow(emulator_window, app_context));
  if (!settings_window->Initialize()) {
    return nullptr;
  }
  return settings_window;
}

bool ImGuiDebugSettingsWindow::Initialize() {
  Emulator* emulator =
      emulator_window_ ? emulator_window_->emulator() : nullptr;
  gpu::GraphicsSystem* graphics_system =
      emulator ? emulator->graphics_system() : nullptr;
  if (!graphics_system || !graphics_system->provider()) {
    XELOGE("Cannot open debug settings window without a graphics system");
    return false;
  }

  window_listener_ =
      std::make_unique<WindowListener>(*this);
  window_ = xe::ui::Window::Create(app_context_, kWindowTitle, 720, 900);
  window_->AddListener(window_listener_.get());

  auto main_menu = MenuItem::Create(MenuItem::Type::kNormal);
  auto file_menu = MenuItem::Create(MenuItem::Type::kPopup, "&File");
  file_menu->AddChild(
      MenuItem::Create(MenuItem::Type::kString, "&Close", "Alt+F4",
                       [this]() { Close(); }));
  main_menu->AddChild(std::move(file_menu));
  window_->SetMainMenu(std::move(main_menu));

  if (!window_->Open()) {
    XELOGE("Failed to open the platform window for debug settings");
    return false;
  }

  xe::ui::GraphicsProvider& graphics_provider = *graphics_system->provider();

  presenter_ = graphics_provider.CreatePresenter();
  if (!presenter_) {
    XELOGE("Failed to initialize the presenter for debug settings");
    return false;
  }

  immediate_drawer_ = graphics_provider.CreateImmediateDrawer();
  if (!immediate_drawer_) {
    XELOGE("Failed to initialize the immediate drawer for debug settings");
    return false;
  }
  immediate_drawer_->SetPresenter(presenter_.get());

  imgui_drawer_ = std::make_unique<xe::ui::ImGuiDrawer>(window_.get(), 0);
  imgui_drawer_->SetPresenterAndImmediateDrawer(presenter_.get(),
                                                immediate_drawer_.get());

  hid::InputSystem* input_system =
      emulator ? emulator->input_system() : nullptr;
  debug_dialog_ =
      new ImGuiDebugDialog(imgui_drawer_.get(), emulator_window_, input_system,
                           /*external_window=*/true);
  debug_dialog_->SetOnCloseCallback([this]() { HandleDialogClosed(); });

  window_->SetPresenter(presenter_.get());
  return true;
}

void ImGuiDebugSettingsWindow::Close() {
  if (close_requested_) {
    return;
  }
  close_requested_ = true;

  if (debug_dialog_) {
    debug_dialog_->CloseDialog();
    debug_dialog_ = nullptr;
  }

  if (window_) {
    window_->RequestClose();
  }
}

void ImGuiDebugSettingsWindow::HandleWindowClosing() {
  if (debug_dialog_) {
    debug_dialog_->CloseDialog();
    debug_dialog_ = nullptr;
  }

  if (on_closed_callback_) {
    on_closed_callback_();
  }
}

void ImGuiDebugSettingsWindow::HandleDialogClosed() {
  debug_dialog_ = nullptr;
  Close();
}

}  // namespace ui
}  // namespace xe
