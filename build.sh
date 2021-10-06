#
# Copyright (c) 2021.
# License: CC0 1.0 Universal
# Permissions:
# - Commercial use
# - Modification
# - Distribution
# - Private use
#
# Limitations:
# - Liability
# - Trademark use
# - Patent use
# - Warranty
#
# Author: Danil Andreev | danssg08@gmail.com | https://github.com/DanilAndreev
#

# Remove previous build directory
rm -rf cmake-build
# Create build directory
mkdir cmake-build
cd cmake-build || exit

# Setup cmake files
cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_DEPENDS_USE_COMPILER=FALSE -G "CodeBlocks - Unix Makefiles" ..
cmake --build . --target image-morphing-tool -- -j 3