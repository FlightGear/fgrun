%define name fgrun
%define version 0.3.3
%define release 1mdk

Summary: Graphical launcher for the FlightGear flight simulator
Name: %{name}
Version: %{version}
Release: %{release}
Source0: %{name}-%{version}.tar.bz2
License: GPL
Group: Games/Other
URL: http://sourceforge.net/projects/fgrun
BuildRoot: %{_tmppath}/%{name}-buildroot
BuildRequires: fltk-devel >= 1.1.0 autoconf2.5
Requires: fltk >= 1.1.0
Prefix: %{_prefix}

%description
fgrun is a graphical launcher for the FlightGear flight simulator.

%prep
rm -rf $RPM_BUILD_ROOT

%setup -q

%build
%configure2_5x --program-prefix=""
%make

%install
rm -rf $RPM_BUILD_ROOT
%makeinstall

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)
%doc README COPYING
%attr(755,root,root) %{_bindir}/%{name}

%changelog
* Tue Oct 28 2003 Bernie Bright <bbright@users.sourceforge.net> 0.3.2-1mdk
- first Mandrake package.


# end of file
