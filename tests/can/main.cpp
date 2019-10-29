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

// Tested on hardware (Rev. 1 with V_ISOIN/V_ISOOUT pins on tranceiver connected)
// on 2016-01-11 by Kevin.

#include "../board/usb2can_rev1.hpp"
#include <xpcc/processing/timer.hpp>

int main(void)
{
	Board::initialize();

	Board::LedBlue::set();

	// Initialize Can
	GpioInputB8::connect(Can1::Rx);
	GpioOutputB9::connect(Can1::Tx, Gpio::OutputType::PushPull);
	Can1::initialize<Board::systemClock, Can1::Bitrate::kBps125>(9);

	// Receive every message
	CanFilter::setFilter(0, CanFilter::FIFO0,
			CanFilter::ExtendedIdentifier(0),
			CanFilter::ExtendedFilterMask(0));

	// Send a message
	xpcc::can::Message msg1(1, 1);
	msg1.setExtended(true);
	msg1.data[0] = 0x11;
	Can1::sendMessage(msg1);

	xpcc::ShortPeriodicTimer pTimer(1000);

	while (1) {
		if (Can1::isMessageAvailable()) {
			xpcc::can::Message message;
			Can1::getMessage(message);
			if(message.getLength() > 0) {
				message.data[0] = message.data[0] + 1;
				Can1::sendMessage(message);
			}
		}

		if (pTimer.execute()) {
			Board::LedBlue::toggle();

			static uint8_t idx = 0;
			xpcc::can::Message msg1(1, 1);
			msg1.setExtended(true);
			msg1.data[0] = idx;
			Can1::sendMessage(msg1);

			++idx;
		}
	}

	return 0;
}
