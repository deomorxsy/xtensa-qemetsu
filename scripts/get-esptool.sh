#!/usr/bin/sh

cd ./assets/ || return
git clone git@github.com:espressif/esptool.git
python3 -m venv venv
cd - || return

# source(POSIX sh) venv relative to the repo root directory
. ./assets/venv/bin/activate
pip3 install --upgrade pip
pip3 install -r ./assets/requirements.txt

deactivate


