// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#import "chrome/browser/ui/cocoa/one_click_signin_dialog_controller.h"

#include "base/bind.h"
#include "base/message_loop.h"
#import "chrome/browser/ui/cocoa/constrained_window/constrained_window_custom_sheet.h"
#include "chrome/browser/ui/cocoa/constrained_window/constrained_window_custom_window.h"
#import "chrome/browser/ui/cocoa/one_click_signin_view_controller.h"

OneClickSigninDialogController::OneClickSigninDialogController(
    content::WebContents* web_contents,
    const BrowserWindow::StartSyncCallback& sync_callback) {
  base::Closure close_callback = base::Bind(
      &OneClickSigninDialogController::PerformClose, base::Unretained(this));
  view_controller_.reset([[OneClickSigninViewController alloc]
      initWithNibName:@"OneClickSigninDialog"
          webContents:web_contents
         syncCallback:sync_callback
        closeCallback:close_callback]);
  scoped_nsobject<NSWindow> window([[ConstrainedWindowCustomWindow alloc]
      initWithContentRect:[[view_controller_ view] bounds]]);
  [[window contentView] addSubview:[view_controller_ view]];

  scoped_nsobject<CustomConstrainedWindowSheet> sheet(
      [[CustomConstrainedWindowSheet alloc]
          initWithCustomWindow:window]);
  constrained_window_.reset(new ConstrainedWindowMac(
      this, web_contents, sheet));
}

OneClickSigninDialogController::~OneClickSigninDialogController() {
}

void OneClickSigninDialogController::OnConstrainedWindowClosed(
    ConstrainedWindowMac* window) {
  [view_controller_ viewWillClose];
  MessageLoop::current()->DeleteSoon(FROM_HERE, this);
}

void OneClickSigninDialogController::PerformClose() {
  constrained_window_->CloseWebContentsModalDialog();
}
