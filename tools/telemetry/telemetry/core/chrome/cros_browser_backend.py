# Copyright (c) 2012 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.
import logging
import os
import subprocess

from telemetry.core import util
from telemetry.core.chrome import browser_backend
from telemetry.core.chrome import cros_util

class CrOSBrowserBackend(browser_backend.BrowserBackend):
  def __init__(self, browser_type, options, cri):
    super(CrOSBrowserBackend, self).__init__(is_content_shell=False,
        supports_extensions=True, options=options)
    # Initialize fields so that an explosion during init doesn't break in Close.
    self._options = options
    self._cri = cri
    self._browser_type = browser_type

    self._remote_debugging_port = self._cri.GetRemotePort()
    self._login_ext_dir = '/tmp/chromeos_login_ext'

    # Push a dummy login extension to the device.
    # This extension automatically logs in as test@test.test
    logging.info('Copying dummy login extension to the device')
    cri.PushFile(os.path.join(os.path.dirname(__file__), 'chromeos_login_ext'),
                 '/tmp/')
    cri.GetCmdOutput(['chown', '-R', 'chronos:chronos', self._login_ext_dir])

    # Copy local extensions to temp directories on the device.
    for e in options.extensions_to_load:
      output = cri.GetAllCmdOutput(['mktemp', '-d', '/tmp/extension_XXXXX'])
      extension_dir = output[0].rstrip()
      cri.PushFile(e.path, extension_dir)
      cri.GetCmdOutput(['chown', '-R', 'chronos:chronos', extension_dir])
      e.local_path = os.path.join(extension_dir, os.path.basename(e.path))

    # Ensure the UI is running and logged out.
    self._RestartUI()

    # Delete test@test.test's cryptohome vault (user data directory).
    if not options.dont_override_profile:
      logging.info('Deleting user\'s cryptohome vault (the user data dir)')
      self._cri.GetCmdOutput(
          ['cryptohome', '--action=remove', '--force', '--user=test@test.test'])

    # Restart Chrome with the login extension and remote debugging.
    logging.info('Restarting Chrome with flags and login')
    args = ['dbus-send', '--system', '--type=method_call',
            '--dest=org.chromium.SessionManager',
            '/org/chromium/SessionManager',
            'org.chromium.SessionManagerInterface.EnableChromeTesting',
            'boolean:true',
            'array:string:"%s"' % '","'.join(self.GetBrowserStartupArgs())]
    cri.GetCmdOutput(args)

    # Find a free local port.
    self._port = util.GetAvailableLocalPort()

    # Forward the remote debugging port.
    logging.info('Forwarding remote debugging port')
    self._forwarder = SSHForwarder(
      cri, 'L',
      util.PortPair(self._port, self._remote_debugging_port))

    # Wait for the browser to come up.
    logging.info('Waiting for browser to be ready')
    try:
      self._WaitForBrowserToComeUp()
      self._PostBrowserStartupInitialization()
    except:
      import traceback
      traceback.print_exc()
      self.Close()
      raise

    cros_util.NavigateLogin(self)
    logging.info('Browser is up!')

  def GetBrowserStartupArgs(self):
    self.webpagereplay_remote_http_port = self._cri.GetRemotePort()
    self.webpagereplay_remote_https_port = self._cri.GetRemotePort()

    args = super(CrOSBrowserBackend, self).GetBrowserStartupArgs()

    args.extend([
            '--enable-smooth-scrolling',
            '--enable-threaded-compositing',
            '--enable-per-tile-painting',
            '--force-compositing-mode',
            '--login-screen=login',
            '--remote-debugging-port=%i' % self._remote_debugging_port,
            '--auth-ext-path=%s' % self._login_ext_dir,
            '--start-maximized'])
    return args

  def GetRemotePort(self, _):
    return self._cri.GetRemotePort()

  def __del__(self):
    self.Close()

  def Close(self):
    super(CrOSBrowserBackend, self).Close()

    self._RestartUI() # Logs out.

    if self._forwarder:
      self._forwarder.Close()
      self._forwarder = None

    if self._login_ext_dir:
      self._cri.RmRF(self._login_ext_dir)
      self._login_ext_dir = None

    for e in self._options.extensions_to_load:
      self._cri.RmRF(os.path.dirname(e.local_path))

    self._cri = None

  def IsBrowserRunning(self):
    # On ChromeOS, there should always be a browser running.
    for _, process in self._cri.ListProcesses():
      if process.startswith('/opt/google/chrome/chrome'):
        return True
    return False

  def GetStandardOutput(self):
    return 'Cannot get standard output on CrOS'

  def CreateForwarder(self, *port_pairs):
    assert self._cri
    return SSHForwarder(self._cri, 'R', *port_pairs)

  def _RestartUI(self):
    if self._cri:
      logging.info('(Re)starting the ui (logs the user out)')
      if self._cri.IsServiceRunning('ui'):
        self._cri.GetCmdOutput(['restart', 'ui'])
      else:
        self._cri.GetCmdOutput(['start', 'ui'])


class SSHForwarder(object):
  def __init__(self, cri, forwarding_flag, *port_pairs):
    self._proc = None

    if forwarding_flag == 'R':
      self._host_port = port_pairs[0].remote_port
      command_line = ['-%s%i:localhost:%i' % (forwarding_flag,
                                              port_pair.remote_port,
                                              port_pair.local_port)
                      for port_pair in port_pairs]
    else:
      self._host_port = port_pairs[0].local_port
      command_line = ['-%s%i:localhost:%i' % (forwarding_flag,
                                              port_pair.local_port,
                                              port_pair.remote_port)
                      for port_pair in port_pairs]

    self._device_port = port_pairs[0].remote_port

    self._proc = subprocess.Popen(
      cri.FormSSHCommandLine(['sleep', '999999999'], command_line),
      stdout=subprocess.PIPE,
      stderr=subprocess.PIPE,
      stdin=subprocess.PIPE,
      shell=False)

    util.WaitFor(lambda: cri.IsHTTPServerRunningOnPort(self._device_port), 60)

  @property
  def url(self):
    assert self._proc
    return 'http://localhost:%i' % self._host_port

  def Close(self):
    if self._proc:
      self._proc.kill()
      self._proc = None
