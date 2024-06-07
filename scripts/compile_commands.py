#!/bin/python3

import os
Import("env")

# include toolchain paths
env.Replace(COMPILATIONDB_INCLUDE_TOOLCHAIN=True)

# set build directory
env.Replace(BUILD_DIR="./assets/")

# override compilation DB path
env.Replace(
        COMPILATIONDB_PATH=os.path.join(
            "$BUILD_DIR",
            "compile_commands.json")
        )
