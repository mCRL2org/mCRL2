

Summary:   MCRL2
Name:      MCRL2
Version:   1211
Release:   3
Group:     Development/Tools
Packager:  mcrl
License:   GPL
URL:       http://www.win.tue.nl/mcrl2

Source0: %name-%version.tar.gz

BuildRoot: /var/tmp/%{name}-buildroot

BuildRequires: wxGTK-devel


%description

Micro Common Representation Language


%define prefix /usr


%prep

  ### prep ###

%setup -n MCRL2

  # Without -n MCRL2, rpm assumes MCRL2-%version


%build

  ### build ###

  cd %_builddir/MCRL2/trunk
    # Restore the lost executable status from
    # the viewcvs/subversion tarball download
      chmod 755 configure
      chmod 755 src/boost/tools/jam/build.sh
    ./configure --prefix=%prefix --mandir=%prefix/share/man
    make

%install

  ### install ###

  umask 022
  /bin/rm -rf %buildroot
  mkdir -p %buildroot

  cd %_builddir/MCRL2/trunk/src
    make prefix=%buildroot%prefix mandir=%buildroot%prefix/share/man install

  cd %buildroot%prefix
    gzip share/man/*/*

  mkdir -p %buildroot/usr/share/doc/%name-%version
  cd %_builddir/MCRL2/trunk
    cp -a \
      articles \
      conventions \
      COPYING \
      examples \
      INSTALL \
      README \
      specs \
      tests \
	%buildroot/usr/share/doc/%name-%version



%files
%defattr(-,root,root)
%prefix/bin/*
%prefix/lib/*
%prefix/share/man/*/*
%doc /usr/share/doc/%name-%version


%changelog


* Thu Mar  9 2006 1211.3
- make install from src/boost no longer needed

* Sun Mar  5 2006 1211.2
- Replace /usr/local by %%prefix
- include /usr/share/doc/%%name-%%version itself, not only subdirectories

* Thu Mar  2 2006 1211.1
- Rename Packager to mcrl
- Start configure and make from trunk instead of trunk/src

* Thu Mar  2 2006 1012-2.bcf
- setup -n MCRL2, instead of the explicit rm,cd,tar commands

* Wed Jan 18 2006 1012-1.bcf
- Upgrade to version 1012

* Thu Jan  11 2006 999-1.bcf
- Initial version

