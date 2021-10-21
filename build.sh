#!/bin/bash

sudo apt-get update
sudo apt-get install -y gcc-multilib g++-multilib

set -e

EXT_DIR=$(pwd)

#clone ambuild
echo "Installing ambuild ..."
git clone https://github.com/alliedmodders/ambuild $HOME/ambuild
pushd $HOME/ambuild
sudo python setup.py install
popd

#clone sourcemod / metamod / hl2sdk
echo "Installing Sourcemod/Metamod/HL2SDK ..."
git clone https://github.com/alliedmodders/sourcemod --recursive --branch $SMBRANCH --single-branch "$EXT_DIR/sourcemod"
git clone https://github.com/alliedmodders/metamod-source --recursive --branch $MMBRANCH --single-branch "$EXT_DIR/metamod"
git clone https://github.com/alliedmodders/hl2sdk --recursive --branch csgo --single-branch "$EXT_DIR/hl2sdk-csgo"

#build
mkdir -p "$EXT_DIR/build"
pushd "$EXT_DIR/build"
export CC=clang;export CXX=clang++;
python "$EXT_DIR/configure.py" --enable-optimize --sm-path "$EXT_DIR/sourcemod" --mms-path "$EXT_DIR/metamod" --hl2sdk-root "$EXT_DIR" --sdks=csgo
ambuild
popd