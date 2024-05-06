#!/usr/bin/sh

cd ./assets/ || return
git clone git@github.com:espressif/esptool.git
python3 -m venv venv

# source(POSIX sh) venv relative to the repo root directory
. ./assets/venv/bin/activate
pip3 install --upgrade pip
pip3 install -r ./requirements.txt

# source(POSIX sh) venv relative to the repo root directory
. ./assets/venv/bin/activate

deactivate

. ./assets/venv/bin/activate
./assets/esptool/esptool.py --chip esp32 merge_bin --fill-flash-size 4MB -o .pio/build/upesy_wroom/firmware.bin @flash_args

