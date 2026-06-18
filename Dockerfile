# -------------------------
# --- Builder Container ---
# -------------------------
FROM ubuntu:resolute AS builder
ENV DEBIAN_FRONTEND=noninteractive

# --- Install Packages ---
RUN apt-get update && apt-get install -y \
    build-essential \
    clang \
    cmake \
    golang \
 && rm -rf /var/lib/apt/lists/*

# --- Set Working Directory ---
WORKDIR /app
COPY . .

# --- Override to LLVM Toolchain ---
ENV CC=clang
ENV CXX=clang++

# --- Build LetDX ---
RUN cmake -S . -B build -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release \
 && cmake --build build -j$(nproc)

# --- Run Tests ---
RUN ctest --test-dir build/LetDB --output-on-failure

# ------------------------
# --- Runner Container ---
# ------------------------
FROM gcr.io/distroless/cc AS runner

# --- Set Working Directory ---
WORKDIR /app

# --- Move to Distroless Container ---
COPY --from=builder /app/build/LetDB/LetDB .
COPY --from=builder /app/build/LetDD/LetDD .
