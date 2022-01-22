Name:							microemacs
%define version 	2.9
%define _binddir	/usr/bin
Version:					%{version}
Release:					0
Summary:					Microemacs program code editor
Group:						Applications/Development
License:					GPL 3.0 License
URL:							.
Vendor:						.
Source:						microemacs-%{version}.tar.gz
Prefix:						%{_prefix}
Packager:					questaware
BuildRoot:				%{_tmppath}/%{name}-root

%description
A text editor supporting
	+ Piping input into it.
	+ Editting Directories and their contents.
	+ Taking text from the buffer and search into the command line.

%prep
%setup -cq

%build
echo Hello
pwd
cd ./unix
make

%install
mkdir -p %{buildroot}/usr/bin/
install -m 755 unix/microemacs   %{buildroot}%{_bindir}/
install -m 644 src/microemacs.md %{buildroot}%{_bindir}/
install -m 644 macros/.emacsrc  %{buildroot}%{_bindir}/

%clean

%files
%defattr(-,root,root)
%{_bindir}/microemacs
%{_bindir}/microemacs.md
%{_bindir}/.emacsrc

%changelog
