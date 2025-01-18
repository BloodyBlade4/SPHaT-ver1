SOURCES = src/lm75a.chip.c
TARGET  = src/lm75a.wasm



$(TARGET): $(SOURCES) src/wokwi-api.h
	docker run -v /home/cole/Documents/ArduinoProject/SimulationSoftware:/src --user root wokwi/builder-clang-wasm \
		clang --target=wasm32-unknown-wasi --sysroot /opt/wasi-libc -nostartfiles -Wl,--import-memory -Wl,--export-table -Wl,--no-entry -Werror -o $(TARGET) $(SOURCES)

.PHONY: clean
clean:
	rm -f $(TARGET)