.PHONY: all build build-all rebuild clean mui build-mui mui-clean mui-rebuild test test-x86 test-x64

PS := powershell -ExecutionPolicy Bypass -File
LOCALE := all
BUILD := .\build.ps1
SHELL := cmd

all: build

default: all

build:
	$(PS) $(BUILD)

build-all:
	$(PS) $(BUILD) -Locale all

clean:
	$(PS) $(BUILD) -Clean -Locale $(LOCALE)

rebuild:
	$(PS) $(BUILD) -Rebuild -Locale $(LOCALE)

mui: build-mui

build-mui:
	$(PS) $(BUILD) -Mui -Locale $(LOCALE)

mui-clean:
	$(PS) $(BUILD) -Mui -Clean

mui-rebuild:
	$(PS) $(BUILD) -Mui -Rebuild

test:
	.\build\en-US\x86\REVERSI.exe --self-test

test-x86:
	.\build\en-US\x86\REVERSI.exe --self-test

test-x64:
	.\build\en-US\x64\REVERSI.exe --self-test
