command -v wayland-scanner

git clone https://gitlab.freedesktop.org/wayland/wayland-protocols.git

git clone https://gitlab.freedesktop.org/wlroots/wlroots

pkg-config --modversion wayland-protocols
find /usr/share/wayland-protocols -name 'wlr-output-management-unstable-v1.xml'


cd /home/alessandro/dev/OpenGLStarter && for url in \
  https://raw.githubusercontent.com/swaywm/wlroots/main/protocol/wlr-output-management-unstable-v1.xml \
  https://raw.githubusercontent.com/swaywm/wlroots/main/wayland-protocols/wlr-output-management-unstable-v1.xml \
  https://raw.githubusercontent.com/swaywm/wlroots/master/protocol/wlr-output-management-unstable-v1.xml \
  https://raw.githubusercontent.com/swaywm/wlroots/master/wayland-protocols/wlr-output-management-unstable-v1.xml \
  https://raw.githubusercontent.com/swaywm/wlroots/main/../protocol/wlr-output-management-unstable-v1.xml \
; do echo "URL=$url"; curl -fsSLI "$url" | head -4 || echo "FAILED"; done

curl -fsSL https://raw.githubusercontent.com/swaywm/wlroots/master/protocol/wlr-output-management-unstable-v1.xml > wlr-output-management-unstable-v1.xml && wayland-scanner client-header wlr-output-management-unstable-v1.xml wlr-output-management-unstable-v1-client-protocol.h && ls -l wlr-output-management-unstable-v1-client-protocol.h

wayland-scanner private-code wlr-output-management-unstable-v1.xml wlr-output-management-unstable-v1-client-protocol.c && ls -l wlr-output-management-unstable-v1-client-protocol.c



find /usr/share/wayland-protocols /usr/share -name 'xdg-output*.xml' 2>/dev/null | head -20 || true

curl -fsSL https://raw.githubusercontent.com/wayland-project/wayland-protocols/main/stable/xdg-output/xdg-output-v1.xml > xdg-output-v1.xml && test -s xdg-output-v1.xml && echo OK || echo FAIL

cd /home/alessandro/dev/OpenGLStarter/src/test-open-window && for url in \
  https://raw.githubusercontent.com/wayland-project/wayland-protocols/main/unstable/xdg-output/xdg-output-unstable-v1.xml \
  https://raw.githubusercontent.com/wayland-project/wayland-protocols/main/stable/xdg-output/xdg-output-v1.xml \
  https://raw.githubusercontent.com/wayland-project/wayland-protocols/master/unstable/xdg-output/xdg-output-unstable-v1.xml \
  https://raw.githubusercontent.com/wayland-project/wayland-protocols/master/stable/xdg-output/xdg-output-v1.xml ; do echo "TRY $url"; curl -fsSL "$url" | head -5 || echo NO; done

  curl -fsSL https://gitlab.freedesktop.org/wayland/wayland-protocols/-/raw/master/unstable/xdg-output/xdg-output-unstable-v1.xml | head -5 >/dev/null && echo OK || echo FAIL


  curl -fsSL -o xdg-output-unstable-v1.xml https://gitlab.freedesktop.org/wayland/wayland-protocols/-/raw/master/unstable/xdg-output/xdg-output-unstable-v1.xml && test -s xdg-output-unstable-v1.xml && echo SAVED || echo FAIL

  wayland-scanner client-header xdg-output-unstable-v1.xml xdg-output-unstable-v1-client-protocol.h && wayland-scanner private-code xdg-output-unstable-v1.xml xdg-output-unstable-v1-client-protocol.c && ls -l xdg-output-unstable-v1-client-protocol.*

  