// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// From dev/ppb_device_ref_dev.idl modified Tue Jan 22 12:22:52 2013.

#include "ppapi/c/dev/ppb_device_ref_dev.h"
#include "ppapi/c/pp_errors.h"
#include "ppapi/shared_impl/tracked_callback.h"
#include "ppapi/thunk/enter.h"
#include "ppapi/thunk/ppb_device_ref_api.h"
#include "ppapi/thunk/ppb_instance_api.h"
#include "ppapi/thunk/resource_creation_api.h"
#include "ppapi/thunk/thunk.h"

namespace ppapi {
namespace thunk {

namespace {

PP_Bool IsDeviceRef(PP_Resource resource) {
  EnterResource<PPB_DeviceRef_API> enter(resource, false);
  return PP_FromBool(enter.succeeded());
}

PP_DeviceType_Dev GetType(PP_Resource device_ref) {
  EnterResource<PPB_DeviceRef_API> enter(device_ref, true);
  if (enter.failed())
    return PP_DEVICETYPE_DEV_INVALID;
  return enter.object()->GetType();
}

struct PP_Var GetName(PP_Resource device_ref) {
  EnterResource<PPB_DeviceRef_API> enter(device_ref, true);
  if (enter.failed())
    return PP_MakeUndefined();
  return enter.object()->GetName();
}

const PPB_DeviceRef_Dev_0_1 g_ppb_deviceref_dev_thunk_0_1 = {
  &IsDeviceRef,
  &GetType,
  &GetName
};

}  // namespace

const PPB_DeviceRef_Dev_0_1* GetPPB_DeviceRef_Dev_0_1_Thunk() {
  return &g_ppb_deviceref_dev_thunk_0_1;
}

}  // namespace thunk
}  // namespace ppapi
