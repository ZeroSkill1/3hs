#!/usr/bin/env bash

make clean
make RELEASE=1 -j$(nproc)
3dstool -ctf romfs cia_stuff/romfs.bin --romfs-dir romfs
makerom -f ncch -o 3hs.cxi -target t -elf 3hs.elf -icon cia_stuff/icon.smdh -banner cia_stuff/banner.bnr -rsf cia_stuff/3hs.rsf -romfs cia_stuff/romfs.bin
makerom -f cia -o 3hs.cia -i 3hs.cxi:0:0 -ignoresign
