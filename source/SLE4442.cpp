/*
 * SLE4442.cpp
 *
 *  Created on: ۲ آذر ۱۴۰۰
 *      Author: mrras
 */

#include <SLE4442.h>

namespace smartCards {

/**
 * @brief  SLE4442 class (max clock : 50KHz, minimum pulse width: 9us)
 * @retval None
 */
SLE4442::SLE4442() :
		m_readRefresh(true) {
}

/**
 * @brief  Initialize the smart card
 * @param  data : returns the first four bytes in main memory
 * @retval None
 */
bool SLE4442::initialize(uint8_t *data) const {
	uint32_t *d = reinterpret_cast<uint32_t*>(data);
	uint8_t tmp;
	m_setResetOutput();
	m_setClockOutput();
	m_setIOInput();
	m_writeReset(false);
	m_writeClock(false);
	m_writeReset(true);
	m_oneMiliDelay(); // A little extra wait time for this
	m_writeClock(true);
	m_delay();
	m_writeClock(false);
	m_delay();
	m_oneMiliDelay(); // A little extra wait time for this
	m_writeReset(false);
	m_delay();
	*d = m_readIO();
	for (int index = 0; index < 4; index++) {
		tmp = read();
		*d |= static_cast<uint32_t>(tmp) << (index * 8 + 1);
	}
	return d[0] != 0xFF;
}

uint8_t SLE4442::read() const {
	uint8_t data = 0;
	m_setIOInput();
	m_writeClock(true);
	m_delay();
	for (int i = 0; i < 8; i++) {
		data = data >> 1;
		m_writeClock(false);
		m_delay();
		m_writeClock(true);
		m_delay();
		if (m_readIO()) {
			data += 0x80;
		}
	}
	return data;
}

void SLE4442::process() const {
	m_setIOInput();
	m_writeClock(false);
	m_delay();
	while (m_readIO() == false) {
		m_writeClock(true);
		m_delay();
		m_writeClock(false);
		m_delay();
	}
}

void SLE4442::sendCommand(uint8_t command, uint8_t address,
		uint8_t data) const {
	//Output(CLOCK);
	m_writeClock(true);
	m_delay();
	m_setIOOutput();
	m_writeIO(false);
	write(command);
	write(address);
	write(data);
	m_writeClock(false);
	m_delay();
	m_writeIO(false);
	m_writeClock(true);
	m_delay();
	m_writeIO(true);

	if (command == UpdateMain || command == UpdateProtection
			|| command == CompareData || command == UpdateSecurity)
		process();
}

void SLE4442::write(uint8_t data) const {
	uint8_t command = data;
	bool temp = 0;

	for (int i = 0; i < 8; i++) {
		temp = command & 0x01;
		command = command >> 1;
		m_writeClock(false);
		m_delay();
		if (temp != 0)
			m_writeIO(true);
		else
			m_writeIO(false);
		m_writeClock(true);
		m_delay();
	}
}

/**
 * @brief  Authenticate the smart card(authentication (valid or invalid) is remain until card has power)
 * @param  None
 * @retval The authentication code :
 * 		0x00 : Card is locked
 * 		0x01 :  Invalid PSC, one try remaining
 * 		0x03 : Invalid PSC, two tries remaining
 * 		0x07 : PSC verified, you may now write data to the card (Authentication is OK)
 * 		else : Unable to Authenticate
 */
uint8_t SLE4442::authenticate(uint8_t password1, uint8_t password2,
		uint8_t password3) const {
	uint8_t data;

	uint8_t errCounter = readSecurity();

	errCounter = errCounter & 0x07;
	data = errCounter >> 1;

	sendCommand(UpdateSecurity, 0x00, data);
	sendCommand(CompareData, 0x01, password1);
	sendCommand(CompareData, 0x02, password2);
	sendCommand(CompareData, 0x03, password3);

	sendCommand(UpdateSecurity, 0x00, 0x07);

	errCounter = readSecurity();

	return errCounter;
}

void SLE4442::writeMainMemory(uint8_t location, uint8_t value) {
	m_readRefresh = true;
	sendCommand(UpdateMain, location, value);
}

uint8_t SLE4442::readMainMemory(uint8_t location) {
	if (m_readRefresh) {
		m_readRefresh = false;
		sendCommand(ReadMain, 0x00, 0x00);
		for (int i = 0; i < 256; i++)
			m_readBuffer[i] = read();
	}
	return m_readBuffer[location];
}

void SLE4442::readProtectedMemory(uint8_t *values) {
	sendCommand(ReadProtection, 0x00, 0x00);
	for (int i = 0; i < 32; i++)
		values[i] = read();
}

void SLE4442::writeProtectedMemory(uint8_t location, uint8_t value) {
	sendCommand(UpdateProtection, location, value);
}

void SLE4442::writeSecuredMemory(uint8_t location, uint8_t value) {
	sendCommand(UpdateSecurity, location, value);
}

uint8_t SLE4442::readSecurity() const {
	sendCommand(ReadSecurity, 0x00, 0x00);
	uint8_t errCounter = read();

	sendCommand(ReadSecurity, 0x01, 0x00);
	read();

	sendCommand(ReadSecurity, 0x02, 0x00);
	read();

	sendCommand(ReadSecurity, 0x03, 0x00);
	read();

	return errCounter;
}

} /* namespace smartCards */
