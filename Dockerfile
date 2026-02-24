FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    build-essential cmake git ninja-build \
    libsdl2-dev libsdl2-image-dev libsdl2-mixer-dev \
    libsdl2-ttf-dev libsdl2-gfx-dev

COPY . /app
WORKDIR /app

RUN cmake -B /build -GNinja /app -DCMAKE_POLICY_VERSION_MINIMUM=3.5 && \
    cmake --build /build

CMD ["/build/captainclaw"]
