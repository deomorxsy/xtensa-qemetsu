.ONESHELL:

.DEFAULT_GOAL := all
SHELL = /bin/sh

VENV = /home/asari/.config/nvim/venv_nvim/neovim3/
BIN=$(VENV)/bin

all:
	build
	upload

venv:
	. /home/asari/.config/nvim/venv_nvim/neovim3/bin/activate

#
# Build rules
#

build: venv
	pio run -v

esp32: venv
	pio run -e esp32cam -v

nodemcu: venv
	pio run -e nodemcuv2 -v

#
# CI rules
#

# compilation unit metadata:
# generate compile_commands.json
lsp: venv
	pio run --target compiledb

upload: venv
	pio run --target upload

debug: venv
	pio device monitor --filter debug

clean: venv
	pio system prune


