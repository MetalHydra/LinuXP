#!/bin/sh

# Orginalscript starten
echo "starting original post-image-script"
echo "---"
echo "${TARGET_DIR}/../../board/raspberrypi4/post-image.sh $*"
echo "---"
${TARGET_DIR}/../../board/raspberrypi4/post-image.sh $*
echo "---"

echo "copying rootfs.cpio to tftp server"
install -D ${TARGET_DIR}/../images/rootfs.cpio /srv/tftp/
echo "copying Done"



