//DO NOT EDIT MANUALLY, let automation work hard.

// auto-generated by PinoutLogic.java based on config/boards/core48/connectors/black.yaml
// auto-generated by PinoutLogic.java based on config/boards/core48/connectors/grey.yaml
// auto-generated by PinoutLogic.java based on config/boards/core48/connectors/main.yaml
#include "pch.h"

// see comments at declaration in pin_repository.h
const char * getBoardSpecificPinName(brain_pin_e brainPin) {
	switch(brainPin) {
		case Gpio::A10: return "USART1 RX (A10)";
		case Gpio::A8: return "Stepper Direction (A8)";
		case Gpio::A9: return "USART1 TX (A9)";
		case Gpio::B10: return "BARO SCL (B10)";
		case Gpio::B11: return "BARO SDA (B11)";
		case Gpio::B13: return "SPI2 SCK (B13)";
		case Gpio::B14: return "SPI2 MISO (B14)";
		case Gpio::B15: return "SPI2 MOSI (B15)";
		case Gpio::B3: return "SD CS1 (B3)";
		case Gpio::B4: return "SD CS2 (B4)";
		case Gpio::B5: return "CAN 2 RX (B5)";
		case Gpio::B6: return "CAN 2 TX (B6)";
		case Gpio::B7: return "DBW Disable (B7)";
		case Gpio::B8: return "DBW Control (B8)";
		case Gpio::B9: return "DBW Direction (B9)";
		case Gpio::C10: return "SPI3 SCK (C10)";
		case Gpio::C11: return "SPI3 MISO (C11)";
		case Gpio::C12: return "SPI3 MOSI (C12)";
		case Gpio::C6: return "Stepper Control (C6)";
		case Gpio::C7: return "Stepper Disable (C7)";
		case Gpio::C8: return "EGT1 CS (C8)";
		case Gpio::C9: return "EGT2 CS (C9)";
		case Gpio::D0: return "CAN 1 RX (D0)";
		case Gpio::D1: return "CAN 1 TX (D1)";
		case Gpio::D10: return "Low Side 5 (D10)";
		case Gpio::D11: return "Low Side 6 (D11)";
		case Gpio::D12: return "Low Side 7 (D12)";
		case Gpio::D13: return "Low Side 8 (D13)";
		case Gpio::D14: return "Low Side 9 (D14)";
		case Gpio::D15: return "Low Side 10 (D15)";
		case Gpio::D2: return "Digital Input 3 (D2)";
		case Gpio::D3: return "Digital Input 4 (D3)";
		case Gpio::D4: return "Digital Input 1 (D4)";
		case Gpio::D5: return "Digital Input 2 (D5)";
		case Gpio::D6: return "Low Side 1 (D6)";
		case Gpio::D7: return "Low Side 2 (D7)";
		case Gpio::D8: return "Low Side 3 (D8)";
		case Gpio::D9: return "Low Side 4 (D9)";
		case Gpio::E10: return "High Side 6 (E10)";
		case Gpio::E11: return "High Side 5 (E11)";
		case Gpio::E12: return "High Side 4 (E12)";
		case Gpio::E13: return "High Side 3 (E13)";
		case Gpio::E14: return "High Side 2 (E14)";
		case Gpio::E15: return "High Side 1 (E15)";
		case Gpio::E2: return "VR1 (E2)";
		case Gpio::E3: return "VR2 (E3)";
		case Gpio::E4: return "VR3 (E4)";
		case Gpio::E5: return "VR4 (E5)";
		case Gpio::E7: return "Hall2 (E7)";
		case Gpio::E8: return "Hall4 (E8)";
		case Gpio::E9: return "Hall3 (E9)";
		case Gpio::F13: return "Hall5 (F13)";
		case Gpio::F14: return "Hall6 (F14)";
		case Gpio::F15: return "Hall8 (F15)";
		case Gpio::G0: return "Hall7 (G0)";
		case Gpio::G1: return "Hall1 (G1)";
		case Gpio::G10: return "Running LED (G10)";
		case Gpio::G11: return "Warning LED (G11)";
		case Gpio::G12: return "Error LED (G12)";
		case Gpio::G13: return "Communication LED (G13)";
		case Gpio::G14: return "USART6 TX (G14)";
		case Gpio::G15: return "High Side 8 (G15)";
		case Gpio::G2: return "Low Side 11 (G2)";
		case Gpio::G3: return "Low Side 12 (G3)";
		case Gpio::G4: return "Low Side 13 (G4)";
		case Gpio::G5: return "Low Side 14 (G5)";
		case Gpio::G6: return "Low Side 15 (G6)";
		case Gpio::G7: return "Low Side 16 (G7)";
		case Gpio::G8: return "High Side 7 (G8)";
		case Gpio::G9: return "USART6 RX (G9)";
		default: return nullptr;
	}
	return nullptr;
}
