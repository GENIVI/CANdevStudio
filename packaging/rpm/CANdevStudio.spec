#
# spec file for package CANdevStudio
#
# Copyright (c) 2017 SUSE LINUX Products GmbH, Nuernberg, Germany.
#
# All modifications and additions to the file contributed by third parties
# remain the property of their copyright owners, unless otherwise agreed
# upon. The license for this file, and modifications and additions to the
# file, is the same license as for the pristine package itself (unless the
# license for the pristine package is not an Open Source License, in which
# case the license is the MIT License). An "Open Source License" is a
# license that conforms to the Open Source Definition (Version 1.9)
# published by the Open Source Initiative.

# Please submit bugfixes or comments via http://bugs.opensuse.org/
#

Name:           CANdevStudio
Version:        0.1
Release:        1
License:        MPL-2.0
Summary:        Tools used to simulate CAN bus traffic and support ECU development
Url:            https://github.com/GENIVI/CANdevStudio
Group:          AGL
Source:         %{name}-%{version}.tar.gz

BuildRequires:  pkgconfig(Qt5Widgets) >= 5.8
BuildRequires:  pkgconfig(Qt5SerialBus) >= 5.8
BuildRequires:  pkgconfig(Qt5Test) >= 5.8
BuildRequires:  pkgconfig(Qt5OpenGL) >= 5.8
BuildRequires:  pkgconfig(Qt5Network) >= 5.8
BuildRequires:  cmake

%if 0%{?suse_version}
BuildRequires:  libqt5-qtserialbus >= 5.8
%endif

%if 0%{?sle_version} == 120300 && 0%{?is_opensuse}
BuildRequires:  gcc7
BuildRequires:  gcc7-c++
%endif

%if 0%{?fedora_version}
BuildRequires:  qt5-qtserialbus >= 5.8
%endif

BuildRoot:      %{_tmppath}/%{name}-%{version}-build

%description
CANdevStudio is a software tool that is meant to support ECU development  (IVI especially) by providing means of CAN BUS simulation.

%package devel
Summary:        CANdevStudio support ECU development devel
Group:          Development/Libraries/C and C++
Requires:       %{name} = %{version}
Provides:       pkgconfig(%{name}) = %{version}

%description devel
app-framework-binder-devel

%prep
%setup -q

%build
%if 0%{?fedora_version}
LDFLAGS='-Wl,--as-needed -Wl,-z,now -Wl,-z,relro -specs=/usr/lib/rpm/redhat/redhat-hardened-ld'
export LDFLAGS
mkdir build && cd build
%cmake -DCMAKE_C_FLAGS=-fno-strict-aliasing -DCMAKE_INSTALL_LIBDIR=%{_libdir} ..
%endif

%if 0%{?sle_version} == 120300 && 0%{?is_opensuse}
CC=gcc-7
CXX=g++-7
export CC
export CXX
%endif
%if 0%{?suse_version}
%cmake '-DCMAKE_SHARED_LINKER_FLAGS=-Wl,--as-needed -Wl,-z,now' -DCMAKE_C_FLAGS=-fno-strict-aliasing
%endif

make %{?_smp_mflags}

%install
[ -d build ] && cd build
make install DESTDIR=%{buildroot} %{?_smp_mflags}

%files
%defattr(-,root,root)
%doc README.md LICENSE
%{_bindir}/CANdevStudio
%{_libdir}/CANdevStudio/
%{_libdir}/CANdevStudio/*.so
%{_libdir}/qt5/plugins/canbus/libqtCannelloniCanBusPlugin.so

%files devel
%defattr(-,root,root)

%changelog
* Tue Jan 23 2018 Romain
- initial creation

