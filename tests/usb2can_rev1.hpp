/* usb2can_rev1.hpp
 *
 * Copyright (C) 2016 Kevin Laeufer <kevin.laeufer@rwth-aachen.de>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


//
// USB2CAN Rev. 1
// Open hardware, free software CAN interface for personal computers.
// https://github.com/roboterclubaachen/usb2can
//

#ifndef RCA_USB2CAN_REV1_HPP
#define RCA_USB2CAN_REV1_HPP

#include <xpcc/architecture/platform.hpp>

using namespace xpcc::stm32;


namespace Board
{

/// STM32F072 running at 48MHz generated from the internal 48MHz clock
// TODO: use xpcc driver to enable and switch to internal 48MHz clock
//using DefaultSystemClock = SystemClock<InternalClock<MHz48>, MHz48>;

template<int F = 8 * 1000 * 1000>
struct DummyClock {
	static constexpr int Frequency = F;
	static constexpr int Usart1 = Frequency;
	static constexpr int Can1 = Frequency;
	static constexpr int Spi2 = Frequency;
	static constexpr int Timer2 = Frequency;
};

using DefaultSystemClock = DummyClock<48 * 1000 * 1000>;

// led names from schematic
using Led2 = xpcc::GpioInverted<GpioOutputA1>;
using Led3 = xpcc::GpioInverted<GpioOutputA2>;
using Led4 = xpcc::GpioInverted<GpioOutputA0>;
using Led5 = xpcc::GpioInverted<GpioOutputB3>;

// colors from schematic
using LedBlue  = Led2;
using LedRed   = Led3;
using LedWhite = Led4;
using LedGreen = Led5;

// functions from silk screen
using LedRx = Led3;
using LedTx = Led5;
using LedMode0 = Led2;
using LedMode1 = Led4;
using LedMode  = xpcc::SoftwareGpioPort<LedMode1, LedMode0>;

// button name from schematic
using Button3 = xpcc::GpioInverted<GpioInputB5>;	// has external 10k pullup
using ButtonMode = Button3;

inline bool
enableHSI48()
{
	RCC->CR2 |= RCC_CR2_HSI48ON;
	uint32_t t = 1500;
	while (!(RCC->CR2 & RCC_CR2_HSI48RDY) and --t) {
	}
	return static_cast<bool>(RCC->CR2 & RCC_CR2_HSI48RDY);
}

inline bool
switchSystemClockToHSI48()
{
	// use HSI48, all prescalers set to 1
	RCC->CFGR = RCC_CFGR_SW_HSI48;
	uint32_t t = 1500;
	// Wait till the main PLL is used as system clock source
	while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI48 and --t) {
	}

	const bool success = ((RCC->CFGR & RCC_CFGR_SWS) == RCC_CFGR_SWS_HSI48);

	if(success) {
		xpcc::clock::fcpu_MHz = 48;
		xpcc::clock::fcpu_kHz = 48 * 1000;
	}

	return success;
}

inline void
initialize()
{
	// TODO: use xpcc driver to enable and switch to internal 48MHz clock
	// DefaultSystemClock::enable();
	enableHSI48();
	switchSystemClockToHSI48();
	xpcc::cortex::SysTickTimer::initialize<DefaultSystemClock>();

	Led2::setOutput(xpcc::Gpio::Low);
	Led3::setOutput(xpcc::Gpio::Low);
	Led4::setOutput(xpcc::Gpio::Low);
	Led5::setOutput(xpcc::Gpio::Low);

	Button3::setInput();
}

} // namespace Board

#endif	// RCA_USB2CAN_REV1_HPP
