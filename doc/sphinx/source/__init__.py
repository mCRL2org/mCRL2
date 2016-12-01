#!/usr/bin/env python

#~ Copyright 201? Sjoerd Cranen
#~           2015 Wieger Wesselink
#~ Distributed under the Boost Software License, Version 1.0.
#~ (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

import os
import re
import shutil
import subprocess
import logging
import sphinx

_LOG = logging.getLogger('util')

def read_text(filename):
    with open(filename, 'r') as f:
        return f.read()

def write_text(filename, text):
    with open(filename, 'w') as f:
        f.write(text)

def clone_rst(src, dst):
  if os.path.isdir(src):
    for f in os.listdir(src):
      if not f.startswith('.'):
        clone_rst(os.path.join(src, f), os.path.join(dst, f))
  else:
    if not os.path.exists(os.path.dirname(dst)):
      os.makedirs(os.path.dirname(dst))
    if not os.path.exists(dst) or (os.path.getmtime(src) > os.path.getmtime(dst)):
      _LOG.info('Copying changed file: {0}'.format(src))
      shutil.copyfile(src, dst)

#
# Utility functions for use by imported scripts
#
def call(name, cmdline, stdin=None):
  '''Runs cmdline and pipes stdin to the process' stdin. The standard output of
  the process is returned as a string. Any errors and warnings will be logged
  using the name provided. Raises a RuntimeError if the return code of the
  process was nonzero.'''
  _LOG.debug('Running "{0}"'.format(' '.join(cmdline)))
  proc = subprocess.Popen(cmdline,
           stdin=subprocess.PIPE,
           stdout=subprocess.PIPE,
           stderr=subprocess.PIPE)
  out, err = proc.communicate(stdin)
  if proc.returncode:
    _LOG.error('{0} returned with non-zero status.'.format(name))
    _LOG.error('{0} output: {1}'.format(name, out.strip()))
    _LOG.error('{0} error output: {1}'.format(name, err.strip()))
    raise RuntimeError('{0} returned with non-zero status.'.format(name))
  if err:
    _LOG.warning('{0} error output: {1}'.format(name, err.strip()))
  return out

def generate_download_inc_files(temp_dev, temp_usr, version, release_tag='@MCRL2_MAJOR_VERSION@', nightly_tag='@MCRL2_VERSION@'):
  '''Generates the files downloads-release.inc and downloads-nightly.inc. The mCRL2
  version number is substituted.
  '''
  release_file_template = os.path.join(temp_usr, 'downloads-release.inc.template')
  nightly_file_template = os.path.join(temp_usr, 'downloads-nightly.inc.template')
  release_text = read_text(release_file_template)
  nightly_text = read_text(nightly_file_template)
  version1 = re.sub(r'[\.M]', '', version[1])
  dest = '{}.{}'.format(version[0], version1)
  release_text = re.sub(release_tag, dest, release_text)
  nightly_text = re.sub(nightly_tag, dest, nightly_text)
  dev_release_file = os.path.join(temp_dev, 'downloads-release.inc')
  dev_nightly_file = os.path.join(temp_dev, 'downloads-nightly.inc')
  usr_release_file = os.path.join(temp_usr, 'downloads-release.inc')
  usr_nightly_file = os.path.join(temp_usr, 'downloads-nightly.inc')
  write_text(dev_release_file, release_text)
  write_text(dev_nightly_file, nightly_text)
  write_text(usr_release_file, release_text)
  write_text(usr_nightly_file, nightly_text)

def generate_rst(binpath, temppath, outpath, version):
  import developer_manual
  import user_manual
  html_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), '../html'))
  cfg_dev = os.path.join(os.path.dirname(__file__), 'developer_manual')
  cfg_usr = os.path.join(os.path.dirname(__file__), 'user_manual')
  temp_dev = os.path.join(temppath, 'rst','developer_manual')
  temp_usr = os.path.join(temppath, 'rst','user_manual')
  out_dev = os.path.join(outpath, 'developer_manual')
  out_usr = os.path.join(outpath, 'user_manual')
  if html_dir <> outpath:
    clone_rst(html_dir, outpath)
  clone_rst(cfg_dev, temp_dev)
  clone_rst(cfg_usr, temp_usr)
  developer_manual.generate_rst(temppath, outpath)
  user_manual.generate_rst(temppath, outpath, binpath)
  generate_download_inc_files(temp_dev, temp_usr, version)
  # Determine the typical green color for the upper and lower bars on the webpage.
  color='#406756'
  shared_opts = ['-bhtml',
               '-D', 'version={0}'.format('.'.join(version)),
               '-D', 'release={0}'.format(version[0]),
               '-D', 'html_theme_options.relbarbgcolor={0}'.format(color)]
  if _LOG.getEffectiveLevel() > logging.WARNING:
    shared_opts += ['-Q']
  elif _LOG.getEffectiveLevel() >= logging.WARNING:
    shared_opts += ['-q']

  options = shared_opts + ['-c', cfg_dev, temp_dev, out_dev]
  print '<OPTIONS>', ' '.join(options)

  sphinx.main( shared_opts +
               ['-c', cfg_dev,
               temp_dev, out_dev])
  sphinx.main( shared_opts +
               ['-c', cfg_usr,
               temp_usr, out_usr])
