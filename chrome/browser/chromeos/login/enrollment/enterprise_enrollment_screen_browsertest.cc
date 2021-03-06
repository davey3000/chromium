// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/basictypes.h"
#include "base/run_loop.h"
#include "chrome/browser/chromeos/login/enrollment/enterprise_enrollment_screen.h"
#include "chrome/browser/chromeos/login/mock_screen_observer.h"
#include "chrome/browser/chromeos/login/wizard_controller.h"
#include "chrome/browser/chromeos/login/wizard_in_process_browser_test.h"
#include "content/public/test/test_utils.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"

using testing::InvokeWithoutArgs;
using testing::Mock;

namespace chromeos {

class EnterpriseEnrollmentScreenTest : public WizardInProcessBrowserTest {
 public:
  EnterpriseEnrollmentScreenTest()
      : WizardInProcessBrowserTest(
            WizardController::kEnterpriseEnrollmentScreenName) {}

 private:
  DISALLOW_COPY_AND_ASSIGN(EnterpriseEnrollmentScreenTest);
};

IN_PROC_BROWSER_TEST_F(EnterpriseEnrollmentScreenTest, TestCancel) {
  ASSERT_TRUE(WizardController::default_controller() != NULL);

  EnterpriseEnrollmentScreen* enterprise_enrollment_screen =
      WizardController::default_controller()->GetEnterpriseEnrollmentScreen();
  ASSERT_TRUE(enterprise_enrollment_screen != NULL);

  base::RunLoop run_loop;
  MockScreenObserver mock_screen_observer;
  static_cast<WizardScreen*>(enterprise_enrollment_screen)->screen_observer_ =
      &mock_screen_observer;

  ASSERT_EQ(WizardController::default_controller()->current_screen(),
            enterprise_enrollment_screen);

  EXPECT_CALL(mock_screen_observer,
              OnExit(ScreenObserver::ENTERPRISE_ENROLLMENT_COMPLETED))
      .WillOnce(InvokeWithoutArgs(&run_loop, &base::RunLoop::Quit));
  enterprise_enrollment_screen->OnCancel();
  content::RunThisRunLoop(&run_loop);
  Mock::VerifyAndClearExpectations(&mock_screen_observer);

  static_cast<WizardScreen*>(enterprise_enrollment_screen)->screen_observer_ =
      WizardController::default_controller();
}

}  // namespace chromeos
