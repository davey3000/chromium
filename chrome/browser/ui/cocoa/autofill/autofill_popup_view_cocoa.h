// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_UI_COCOA_AUTOFILL_AUTOFILL_POPUP_CONTENT_VIEW_H_
#define CHROME_BROWSER_UI_COCOA_AUTOFILL_AUTOFILL_POPUP_CONTENT_VIEW_H_

#import <Cocoa/Cocoa.h>

#import "ui/base/cocoa/base_view.h"

class AutofillPopupController;
class AutofillPopupViewMac;

// Draws the native Autofill popup view on Mac.
@interface AutofillPopupViewCocoa : BaseView {
 @private
  // The cross-platform controller for this view.
  __weak AutofillPopupController* controller_;
}

// Designated initializer.
- (id)initWithController:(AutofillPopupController*)controller
                   frame:(NSRect)frame;

// Informs the view that its controller has been (or will imminently be)
// destroyed.
- (void)controllerDestroyed;

@end

#endif  // CHROME_BROWSER_UI_COCOA_AUTOFILL_AUTOFILL_POPUP_CONTENT_VIEW_H_
