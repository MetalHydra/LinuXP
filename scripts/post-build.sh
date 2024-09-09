#!/bin/sh

BASE_DIR="/home/sekiro/embedded/Projekt"

# Orginalscript starten
echo "starting original post-build-script"
echo "---"
echo "${TARGET_DIR}/../../board/raspberrypi4/post-build.sh $*"

echo "copying modules"
cp -r /home/sekiro/embedded/raspi/target/lib/modules/6.3.1-v7l+/ ${TARGET_DIR}/lib/modules/
echo "finished copying modules"

echo "Target dir is: "
echo ${TARGET_DIR}
echo "---"

echo "copying network files to target"
install -D "$BASE_DIR/target/interfaces" "${TARGET_DIR}/etc/network/"
install -D "$BASE_DIR/target/resolv.conf" "${TARGET_DIR}/etc/"
echo "finished copying network files"

#echo "---"
install -m 755 "$BASE_DIR/target/resolv.conf" "${TARGET_DIR}/etc/"
install -m 755 "$BASE_DIR/target/etc/dnsmasq.conf" "${TARGET_DIR}/etc/"
install -m 755 "$BASE_DIR/target/etc/ntp.conf" "${TARGET_DIR}/etc/"
install "$BASE_DIR/target/mosquitto/mosquitto.conf" "${TARGET_DIR}/etc/mosquitto/"
install "$BASE_DIR/target/mosquitto/mosquitto_pub.sh" "${TARGET_DIR}/etc/mosquitto/"
install "$BASE_DIR/target/mosquitto/mosquitto_sub.sh" "${TARGET_DIR}/etc/mosquitto/"
install "$BASE_DIR/target/etc/wpa_supplicant.conf" "${TARGET_DIR}/etc/"
install "$BASE_DIR/target/S52ntp" "${TARGET_DIR}/etc/init.d/"
install "$BASE_DIR/target/S61wpa" "${TARGET_DIR}/etc/init.d/"
install "$BASE_DIR/target/S99Mosquitto_start" "${TARGET_DIR}/etc/init.d/"
install "$BASE_DIR/target/S02Loadmodule" "${TARGET_DIR}/etc/init.d/"
install -m 755 "$BASE_DIR/target/sysctl.conf" "${TARGET_DIR}/etc"
install -m 755 "$BASE_DIR/target/S99firewall" "${TARGET_DIR}/etc/init.d/"
install -m 755 "$BASE_DIR/driver/blinkdrvad.ko" "${TARGET_DIR}/lib/modules/"
echo "Done with everything"

