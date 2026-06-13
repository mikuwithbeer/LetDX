FROM ubuntu:resolute AS builder

ENV DEBIAN_FRONTEND=noninteractive

# --- Install Packages ---
RUN apt-get update && apt-get install -y \
    build-essential \
    clang \
    cmake \
    golang \
 && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY . .

# --- Override to LLVM Toolchain ---
ENV CC=clang
ENV CXX=clang++

# --- Build LetDX ---
RUN cmake -S . -B build -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release \
 && cmake --build build -j$(nproc)

FROM gcr.io/distroless/cc AS runner

WORKDIR /app

# --- Move to Distroless Container ---
COPY --from=builder /app/build/LetDB/LetDB .
COPY --from=builder /app/build/LetDD/LetDD .
