Name:		openclaw
Version:	1.0
Release:	1
Summary:	"Open source reimplementation of Captain Claw (1997) platformer game"
License:	GPLv3+
URL:		https://github.com/pjasicek/OpenClaw
Requires:	mono-winforms

%description
The openclaw program is a reimplementation of original Captain Claw (1997) platformer
game developed by Monolith.

%files 
/usr/bin/openclaw
/usr/bin/ClawLauncher.exe
/usr/bin/clawlauncher
/usr/share/openclaw/CLAW.REZ
/usr/share/openclaw/ASSETS.ZIP
/usr/share/openclaw/clacon.ttf
/usr/share/openclaw/console02.tga
/usr/share/openclaw/config.xml
/usr/share/openclaw/SAVES.XML

%changelog
* Wed May 24 2017 "Petr Jasicek" <pjasicek@gmail.com> - 1.0
- Initial version of the package
