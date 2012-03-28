# spec file to build the camwatch rpm
%define  ver  0.2.0
%define  rel  1
%define prefix  /usr

Summary: Small app to view a remote cam by updating a window with pictures from a web server.
Name: camwatch
Version: %ver
Release: %rel
Requires: imlib libcurl
Copyright: GPL
Group: X11/Applications/Graphics
Source: http://www.hayseed.net/~emerson/camwatch-0.2.tar.gz
URL: http://www.hayseed.net/~emerson/camwatch.html
Vendor: R Pickett
Packager: R Pickett <emerson@hayseed.net>
BuildRoot: /var/tmp/camwatch
Prefix: %prefix

%description
camwatch is a small app that will allow you to view remote webcams on web 
servers within a small window which automatically fetches and updates an 
image from an http server.  It can optionally archive these images using
timestamped filenames.

%prep
%setup

%build
make

%install
export PREFIX=${RPM_BUILD_ROOT}%{prefix}
mkdir -p $PREFIX/bin
mkdir -p -m755 $PREFIX/man/man1
make -e install

%post

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(0644,root,root)
%doc README CHANGES BUGS TODO AUTHORS COPYING
%doc %{prefix}/man/man1/camwatch.1
%attr(0755,root,root) %{prefix}/bin/camwatch
