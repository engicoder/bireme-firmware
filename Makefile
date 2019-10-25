OUTPUT_DIRECTORY := _build

all: 
	$(MAKE) -C keyboard bireme_keyboard_left
	$(MAKE) -C keyboard bireme_keyboard_right
	$(MAKE) -C receiver
	mkdir -p precompiled-hex
	cp keyboard/_build/*.hex precompiled-hex/ 
	cp receiver/_build/*.hex precompiled-hex/ 

clean:

	$(MAKE) -C keyboard clean
	$(MAKE) -C receiver clean
