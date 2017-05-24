Name:		captainclaw
Version:	1.0
Release:	1
Summary:	"Open source reimplementation of Captain Claw (1997) platformer game"
License:	GPLv3+
URL:		https://github.com/pjasicek/CaptainClaw
Requires:	libSDL2
Requires:	libSDL2-image
Requires:	libSDL2-mixer
Requires:	libSDL2-ttf
Requires:	libSDL2-gfx
Requires:	libtinyxml

%description
The captainclaw program is a reimplementation of original Captain Claw (1997) platformer
game developed by Monolith.

%files 
/usr/bin/captainclaw
/usr/share/captainclaw/CLAW.REZ
/usr/share/captainclaw/ASSETS.ZIP
/usr/share/captainclaw/clacon.ttf
/usr/share/captainclaw/console02.tga
/usr/share/captainclaw/config.xml
/usr/share/captainclaw/SAVES.XML

%changelog
* Wed May 24 2017 "Petr Jasicek" <pjasicek@gmail.com> - 1.0
- Initial version of the package
