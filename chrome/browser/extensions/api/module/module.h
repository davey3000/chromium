// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_EXTENSIONS_API_MODULE_MODULE_H__
#define CHROME_BROWSER_EXTENSIONS_API_MODULE_MODULE_H__

#include "chrome/browser/extensions/extension_function.h"

namespace extensions {

class ExtensionPrefs;

class ExtensionSetUpdateUrlDataFunction : public SyncExtensionFunction {
 public:
  DECLARE_EXTENSION_FUNCTION("extension.setUpdateUrlData",
                             EXTENSION_SETUPDATEURLDATA)

 protected:
  virtual ~ExtensionSetUpdateUrlDataFunction() {}

  // ExtensionFunction:
  virtual bool RunImpl() OVERRIDE;

 private:
  ExtensionPrefs* extension_prefs();
};

class ExtensionIsAllowedIncognitoAccessFunction : public SyncExtensionFunction {
 public:
  DECLARE_EXTENSION_FUNCTION("extension.isAllowedIncognitoAccess",
                             EXTENSION_ISALLOWEDINCOGNITOACCESS)

 protected:
  virtual ~ExtensionIsAllowedIncognitoAccessFunction() {}

  // ExtensionFunction:
  virtual bool RunImpl() OVERRIDE;
};

class ExtensionIsAllowedFileSchemeAccessFunction
    : public SyncExtensionFunction {
 public:
  DECLARE_EXTENSION_FUNCTION("extension.isAllowedFileSchemeAccess",
                             EXTENSION_ISALLOWEDFILESCHEMEACCESS)

 protected:
  virtual ~ExtensionIsAllowedFileSchemeAccessFunction() {}

  // ExtensionFunction:
  virtual bool RunImpl() OVERRIDE;
};

}  // namespace extensions

#endif  // CHROME_BROWSER_EXTENSIONS_API_MODULE_MODULE_H__
