/*
 * SLE4442.h
 *
 *  Created on: ۲ آذر ۱۴۰۰
 *      Author: mrras
 */

#ifndef MYDRIVERS_SLE4442_H_
#define MYDRIVERS_SLE4442_H_

#include "stdint.h"

#define DEVICE_ADDRESS 0xA0		// Device Address
#define ACK  0        			// ACK
#define NAK  1        			// NAK

#define ReadMain           0x30  // Read main memory
#define ReadProtection     0x34  // Read protection bit
#define ReadSecurity       0x31  // Read PSC
#define UpdateMain         0x38  // Update main memory
#define UpdateSecurity     0x39  // Update PSC
#define UpdateProtection   0x3C  // Update Protection bits
#define CompareData        0x33  // Compare Verification Data

namespace smartCards {

class SLE4442 {
public:
	SLE4442();

protected:
	void (*m_delay)();
	void (*m_setClockOutput)();
	void (*m_setResetOutput)();
	void (*m_setIOInput)();
	void (*m_setIOOutput)();
	void (*m_writeClock)(bool value);
	void (*m_writeIO)(bool value);
	void (*m_writeReset)(bool value);
	bool (*m_readIO)();
	void (*m_oneMiliDelay)();


	bool initialize(uint8_t *data) const;
	uint8_t authenticate(uint8_t password1 = 0xFF, uint8_t password2 = 0xFF,
			uint8_t password3 = 0xFF) const;

	uint8_t readMainMemory(uint8_t location);
	void writeMainMemory(uint8_t location, uint8_t value);

	void readProtectedMemory(uint8_t *values);
	void writeProtectedMemory(uint8_t location, uint8_t value);

	void writeSecuredMemory(uint8_t location, uint8_t value);

private:
	bool m_readRefresh;
	uint8_t m_readBuffer[256];
	uint8_t read() const;
	void write(uint8_t data) const;
	void process() const;
	void sendCommand(uint8_t command, uint8_t address, uint8_t data) const;
	uint8_t readSecurity() const;
};

} /* namespace smartCards */

#endif /* MYDRIVERS_SLE4442_H_ */
