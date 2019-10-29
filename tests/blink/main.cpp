/* main.cpp
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


#include "../board/usb2can_rev1.hpp"

using namespace Board;

// ----------------------------------------------------------------------------
// Set the log level
#undef	XPCC_LOG_LEVEL
#define	XPCC_LOG_LEVEL xpcc::log::DEBUG

/* Minimal test program to test core, GPIO and UART
 *
 * Flash this program by using either SWD or DFU.
 *
 * Observe:
 * - LED flashing in pairs: green/white and red/blue at about 2 Hz
 * - Frequency increases to 5 Hz when button MODE is pressed
 * - Observe ASCII characters "** RCA ** CAN<->USB " at 115200 baud 8N1 at test point 3
 */

int
main(void)
{
	Board::initialize();

	static constexpr char rca[] = "** RCA ** CAN<->USB ";
	uint8_t ii = 0;

	while (true) {
		xpcc::delayMilliseconds(ButtonMode::read() ? 100 : 300);
		LedRed::set();
		LedBlue::set();
		xpcc::delayMilliseconds(1);
		LedRed::reset();
		LedBlue::reset();

		xpcc::delayMilliseconds(ButtonMode::read() ? 100 : 300);
		LedGreen::set();
		LedWhite::set();
		xpcc::delayMilliseconds(1);
		LedGreen::reset();
		LedWhite::reset();

		XPCC_LOG_DEBUG << rca[ii];
		if (++ii >= strlen(rca)) {
			ii = 0;
		}
	}
}
