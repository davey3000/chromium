// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_CHROMEOS_LOGIN_MOUNT_MANAGER_H_
#define CHROME_BROWSER_CHROMEOS_LOGIN_MOUNT_MANAGER_H_

#include <map>
#include <string>

#include "base/files/file_path.h"
#include "base/memory/scoped_ptr.h"

class PrefRegistrySimple;

namespace chromeos {

// Keeps track of mount points for different users.
class MountManager {
 public:
  // Returns a shared instance of a MountManager. Not thread-safe,
  // should only be called from the main UI thread.
  static MountManager* Get();

  virtual ~MountManager();

  virtual bool IsMounted(const std::string& user_id);
  virtual base::FilePath GetPath(const std::string& user_id);

  virtual void SetPath(const std::string& user_id,
                                 const base::FilePath& path);
  virtual void DeletePath(const std::string& user_id);

 private:
  MountManager();

  typedef std::map<std::string, base::FilePath> UserToPathMap;

  UserToPathMap additional_mounts_;

  static scoped_ptr<MountManager> instance_;

  DISALLOW_COPY_AND_ASSIGN(MountManager);
};

}  // namespace chromeos

#endif  // CHROME_BROWSER_CHROMEOS_LOGIN_MOUNT_MANAGER_H_
