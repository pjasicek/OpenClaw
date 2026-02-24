FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    build-essential cmake git \
    libsdl2-dev libsdl2-image-dev libsdl2-mixer-dev \
    libsdl2-ttf-dev libsdl2-gfx-dev

COPY . /app
WORKDIR /app/Build
RUN cmake .. && make -j$(nproc)

CMD ["./captainclaw"]
