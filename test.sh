#!/bin/bash

LUA=lua
LUA_ORIG=lua_orig

echo "Building Base Lua"
make clean > /dev/null 2>&1
make OPCODE="shuffle/base/" linux-readline > /dev/null 2>&1
cp ${LUA} ${LUA_ORIG}

# Ensure
mkdir -p shuffle/shuffled

for i in {1..256} ; do
    echo "Shuffling ${i}"

    SHUFFLE_DIR="shuffle/shuffled/${i}"
    mkdir -p ${SHUFFLE_DIR}
    ./${LUA_ORIG} shuffle/sort.lua --output=${SHUFFLE_DIR} --seed=${i}

    # Ensure build
    if ! (make clean && make OPCODE="${SHUFFLE_DIR}" linux-readline) > /dev/null 2>&1 ; then
        rm ${LUA_ORIG} # Ensure base Lua is disposed of
        echo "Compilation Failure: Seed = ${i}"
        exit 1

    # Run tests
    elif ! (cd testes && ./all.lua) > /dev/null 2>&1 ; then
        rm ${LUA_ORIG}
        echo "Tests Failure: Seed = ${i}"
        exit 2
    fi

    cp ${LUA} ${SHUFFLE_DIR}/${LUA}
done

echo "Success"
