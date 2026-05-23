#!/usr/bin/env/ bash
set -e

THIS_DIR="$(cd "$(dirname "$0")" && pwd)"
SRC_DIR="$THIS_DIR/source"
BIN_DIR="$THIS_DIR/bin"


MAIN_FILE="$SRC_DIR/main.cpp"
BINARY_NAME="vulkan_renderer"


CXX=g++
CXXFLAGS="-std=c++17 -g"
INCLUDES=(
	"-I$SRC_DIR"
)

LIBS=(
	"$BIN_DIR/libglfw3.a"
    -lvulkan
    -lX11
    -ldl
    -lpthread
)


mkdir -p "$BIN_DIR"
cd "$BIN_DIR"

$CXX \
	$CXXFLAGS \
	"${INCLUDES[@]}" \
	"$MAIN_FILE" \
	"${LIBS[@]}" \
	-o "$BINARY_NAME"


echo "Build successful: $BIN_DIR/$BINARY_NAME"