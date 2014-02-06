import os
import sys
from util import *

os.chdir(builddir)

#
# Source distribution
#
if label in ["ubuntu-amd64"]:
  source_command = ['make', 'package_source']
  source_result = call('Source', source_command)
  if source_result:
    log('Make returned ' + str(source_result))

#
# Binary Package
#
cpack_options = []
if label in ["fedora-amd64"]:
  cpack_options = ["-G", "RPM"]
elif label in ["ubuntu-x86", "ubuntu-amd64"]:
  cpack_options = ["-G", "DEB"]
elif label in ["windows-x86"]:
  # On Windows, a path can be a maximum of 256 characters. Due to the structure
  # of the Jenkins paths, we outrun this limit when generating a package.
  # As a workaround, we generate the package to C:\Temp\pkg.
  cpack_options = ["-G", "NSIS", "-B", "C:\Temp\pkg"]
elif label in ["windows-amd64"]:
  cpack_options = ["-G", "NSIS64", "-B", "C:\Temp\pkg"]
elif label in ["macosx-x86", "macosx-amd64"]:
  cpack_options = ["-G", "PackageMaker"] 

cpack_command = ['cpack'] + cpack_options
cpack_result = call('CPack', cpack_command)
if cpack_result:
  if label in ["macosx-x86", "macosx-amd64"]:
    # Workaround: currently, CPack may fail because of the following error:
    #   CPack Error: Problem running hdiutil command
    # This seems to be a bug in hdiutil or CPack, see:
    #   https://discussions.apple.com/thread/4712306
    # The fix proposed there is to add -stretch 1g to the hdiutil command line.
    # This is a command run by CPack, so we only have the option to let the
    # original command fail, and then re-run it with this extra parameter.
    try:
      cpackdir = os.path.join(os.getcwd(), '_CPack_Packages')
      buildname = [f for f in os.listdir(cpackdir) if not f.startswith('.')][0]
      packagemakerdir = os.path.join(cpackdir, buildname, 'PackageMaker')
      pkgdir = [f for f in os.listdir(packagemakerdir) if f.endswith('.pkg')][0]
      pkgdir = os.path.join(packagemakerdir, pkgdir)
      dmgfile = os.path.join(packagemakerdir, os.path.splitext(pkgdir)[0] + '.dmg')
      call('hdiutil (dirty fix)', ['/usr/bin/hdiutil', 'create', '-ov', '-format', 'UDZO', '-stretch', '1g', '-srcfolder', pkgdir, dmgfile])
      os.rename(dmgfile, os.path.join(builddir, os.path.basename(dmgfile)))
    except:
      log('CPack returned ' + str(cpack_result) + ' and MacOS dirty fix did not work.')
      sys.exit(cpack_result)
  else:
    log('CPack returned ' + str(cpack_result))
    sys.exit(cpack_result)

# Due to the Windows workaround, we need to copy the generated package (.exe) 
# from the temporary path, and remove the generated package.
if label in ["windows-x86", "windows-amd64"]:
  copy_files("{0}\*.exe".format(cpack_temp_path), builddir)
  shutil.rmtree(cpack_temp_path)

os.chdir(workspace)

