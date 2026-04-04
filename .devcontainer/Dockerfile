FROM mcr.microsoft.com/devcontainers/base:ubuntu-24.04

# Build arguments
ARG BINUTILS_VERSION=2.46.0
ARG GCC_VERSION=15.2.0
ARG TARGET=i686-elf

# Environment
ENV DEBIAN_FRONTEND=noninteractive \
    TOOLS=/opt/cross \
    PATH=/opt/cross/bin:$PATH

# Dependencies
RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    clangd \
    bison \
    flex \
    libgmp3-dev \
    libmpc-dev \
    libmpfr-dev \
    texinfo \
    wget \
    xz-utils \
    ca-certificates \
    cmake \
    ninja-build \
    gdb-multiarch \
    clang-format \
    grub-common \
    xorriso \
    grub-pc-bin \
    && rm -rf /var/lib/apt/lists/*

# Download and extract sources
WORKDIR /tmp/src

RUN wget -q https://ftp.gnu.org/gnu/binutils/binutils-${BINUTILS_VERSION}.tar.xz && \
    tar -xf binutils-${BINUTILS_VERSION}.tar.xz
RUN wget -q https://ftp.gnu.org/gnu/gcc/gcc-${GCC_VERSION}/gcc-${GCC_VERSION}.tar.xz && \
    tar -xf gcc-${GCC_VERSION}.tar.xz

# Build binutils
RUN mkdir build-binutils && cd build-binutils && \
    ../binutils-${BINUTILS_VERSION}/configure \
        --target=${TARGET} \
        --prefix="$TOOLS" \
        --with-sysroot \
        --disable-nls \
        --disable-werror && \
    make -j$(nproc) && \
    make install

# Build GCC
RUN mkdir build-gcc && cd build-gcc && \
    ../gcc-${GCC_VERSION}/configure \
        --target=${TARGET} \
        --prefix="$TOOLS" \
        --disable-nls \
        --enable-languages=c,c++ \
        --without-headers \
        --disable-hosted-libstdcxx && \
    make -j$(nproc) all-gcc all-target-libgcc all-target-libstdc++-v3 && \
    make install-gcc install-target-libgcc install-target-libstdc++-v3

# Cleanup sources
RUN rm -rf /tmp/src

# Convenience symlink
RUN ln -sf /usr/bin/gdb-multiarch /usr/local/bin/${TARGET}-gdb

# Permissions
RUN chown -R vscode:vscode "$TOOLS"

WORKDIR /workspace