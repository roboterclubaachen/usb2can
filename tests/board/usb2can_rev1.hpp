/* usb2can_rev1.hpp
 *
 * Copyright (C) 2016 Kevin Laeufer <kevin.laeufer@rwth-aachen.de>
 * Copyright (C) 2017 strongly-typed (Sascha Schade) <stronglytyp3d@gmail.com>
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
// USB2CAN Rev. 3
// Open hardware, free software CAN interface for personal computers.
// https://github.com/roboterclubaachen/usb2can
//

#ifndef RCA_USB2CAN_REV1_HPP
#define RCA_USB2CAN_REV1_HPP

#include <xpcc/architecture/platform.hpp>
#include <xpcc/debug/logger.hpp>
#define XPCC_BOARD_HAS_LOGGER

using namespace xpcc::stm32;

namespace Board
{

/// STM32F072 running at 48MHz generated from either 
/// internal 8MHz clock with PLL or internal 48MHz clock without PLL.

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

struct systemClock
{
	static constexpr int Frequency = MHz48;
	static constexpr int Can1 = Frequency;
	static constexpr int Usart1 = Frequency;
	static constexpr int Usart2 = Frequency;
	static constexpr int Spi2 = Frequency;

	// Choose between HSI 8 MHz plus PLL and 48 MHz clock
	static constexpr bool use_hsi_8 = false;

	static bool inline
	enable()
	{
		// set flash latency for 48MHz before switching clock
		ClockControl::setFlashLatency(Frequency);

		if (use_hsi_8) {
			// enable internal 8 MHz HSI RC clock
			bool ret = ClockControl::enableInternalClock();
			xpcc_assert(ret, "clock", "init", "hsi8en", 1);

			// (internal clock / 2) * 12 = 48MHz
			ret = ClockControl::enablePll(ClockControl::PllSource::InternalClock, xpcc::stm32::ClockControl::UsbPrescaler::Div1, 12, 1);
			xpcc_assert(ret, "clock", "init", "pllon", 1);

			// switch system clock to PLL output
			ret = ClockControl::enableSystemClock(ClockControl::SystemClockSource::Pll);
			xpcc_assert(ret, "clock", "init", "pllsw", 1);
		} else {
			bool ret = ClockControl::enableInternalClockMHz48();
			xpcc_assert(ret, "clock", "init", "hsi48en", 1);

			ret = ClockControl::enableSystemClock(ClockControl::SystemClockSource::InternalClockMHz48);
			xpcc_assert(ret, "clock", "init", "hsi48sw", 1);

		}

		ClockControl::setAhbPrescaler(ClockControl::AhbPrescaler::Div1);
		ClockControl::setApbPrescaler(ClockControl::ApbPrescaler::Div1);
		// update frequencies for busy-wait delay functions
		xpcc::clock::fcpu     = Frequency;
		xpcc::clock::fcpu_kHz = Frequency / 1000;
		xpcc::clock::fcpu_MHz = Frequency / 1000000;
		xpcc::clock::ns_per_loop = ::round(4000.f / (Frequency / 1000000));

		return true;
	}
};

// Create an IODeviceWrapper around the Uart Peripheral we want to use
using LoggerDevice = xpcc::IODeviceWrapper< Usart1, xpcc::IOBuffer::BlockIfFull >;

inline void
initialize()
{
	Led2::setOutput(xpcc::Gpio::Low);
	Led3::setOutput(xpcc::Gpio::Low);
	Led4::setOutput(xpcc::Gpio::Low);
	Led5::setOutput(xpcc::Gpio::Low);

	systemClock::enable();
	xpcc::cortex::SysTickTimer::initialize<systemClock>();

	GpioOutputB6::connect(Usart1::Tx);
	Usart1::initialize<systemClock, 115200>(12);

	Button3::setInput();
}

} // namespace Board

#endif	// RCA_USB2CAN_REV1_HPP
