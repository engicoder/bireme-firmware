# Bireme

A wireless split compact keyboard.

Keyboard Maintainer: [@engicoder](https://github.com/engicoder)  
Hardware Supported: Bireme PCB  
Hardware Availability: 

Make example for this keyboard (after setting up your build environment):

    make bireme:default

See [build environment setup](https://docs.qmk.fm/#/getting_started_build_tools) then the [make instructions](https://docs.qmk.fm/#/getting_started_make_guide) for more information.

## Notes

This keyboard uses a completely different 'matrix scan' system than most other keyboards supported by QMK. QMK runs on a receiver module. The receiver module includes an ATmega32U4 microcontrooller and a nRF51822 wireless module. QMK runs on the ATmega32u4, while the nRF51822 handles wireless communication with the keyboard halves. The nRF51822 and the ATmega32U4 communicate via a serial link. The wireless module receives matrix updates from the halves and merges them into a single matrix. The QMK functionality (found in matrix.c) on the ATmega32U4 polls the wireless module via the serial link to request matrix updates which are then processed normally by the QMK framework. 


[Bireme firmware](https://github.com/engicoder/bireme)
