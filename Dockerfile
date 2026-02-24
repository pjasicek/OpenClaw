FROM ubuntu:22.04 AS builder

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    build-essential cmake git ninja-build \
    libsdl2-dev libsdl2-image-dev libsdl2-mixer-dev \
    libsdl2-ttf-dev libsdl2-gfx-dev

COPY . /app
WORKDIR /app

RUN cmake -B /build -GNinja /app -DCMAKE_POLICY_VERSION_MINIMUM=3.5 && \
    cmake --build /build

FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive
ENV DISPLAY=:99

RUN apt-get update && apt-get install -y --no-install-recommends \
    libsdl2-2.0-0 libsdl2-image-2.0-0 libsdl2-mixer-2.0-0 \
    libsdl2-ttf-2.0-0 libsdl2-gfx-1.0-0 \
    xvfb x11vnc novnc websockify \
    && rm -rf /var/lib/apt/lists/*

COPY --from=builder /build/Build_Release /game

EXPOSE 6080

CMD Xvfb :99 -screen 0 1024x768x24 & \
    sleep 1 && \
    x11vnc -display :99 -forever -nopw -shared & \
    websockify --web /usr/share/novnc 6080 localhost:5900 & \
    cd /game && ./openclaw
