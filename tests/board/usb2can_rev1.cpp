/* Copyright (c) 2017, Sascha Schade (strongly-typed)
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 * ------------------------------------------------------------------------ */
#include "usb2can_rev1.hpp"

// Create an IODeviceWrapper around the Uart Peripheral we want to use
Board::LoggerDevice loggerDevice;

// Set all four logger streams to use the UART
xpcc::log::Logger xpcc::log::debug(loggerDevice);
xpcc::log::Logger xpcc::log::info(loggerDevice);
xpcc::log::Logger xpcc::log::warning(loggerDevice);
xpcc::log::Logger xpcc::log::error(loggerDevice);

xpcc_extern_c void
xpcc_abandon(const char * module,
			 const char * location,
			 const char * failure,
			 uintptr_t context)
{
	XPCC_LOG_ERROR << "Assertion '" << module << "." << location << "." << failure << "'";
	if (context) { XPCC_LOG_ERROR << " @ " << (void *) context << " (" << (uint32_t) context << ")"; }
	XPCC_LOG_ERROR << " failed! Abandoning..." << xpcc::endl;

	Board::LedGreen::setOutput();
	while(true) {
		Board::LedRed::set();
		xpcc::delayMilliseconds(20);
		Board::LedRed::reset();
		xpcc::delayMilliseconds(180);
	}
}
