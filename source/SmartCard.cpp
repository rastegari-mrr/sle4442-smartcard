/*
 * SmartCard.cpp
 *
 *  Created on: Nov 20, 2021
 *      Author: mrras
 */

#include "SmartCard.h"

namespace smartCards {

/**
 * @brief  Smart Card class (max clock : 50KHz, minimum pulse width: 9us)
 * @param  delayFunction : function that makes 10us delay
 * @param  setClockOutputFunction : function that sets Clock pin as output
 * @param  setResetOutputFunction :  function that sets Reset pin as output
 * @param  setIOInputFunction :  function that sets IO pin as input
 * @param  setIOOutputFunction :  function that sets IO pin as output
 * @param  writeClockFunction :  function that writes to Clock pin
 * @param  writeIOFunction :  function that writes to IO pin
 * @param  writeResetFunction :  function that writes to Reset pin
 * @param  readIOFunction :  function that reads IO pin
 * @param  oneMiliDelayFunction :  function that makes 1ms delay
 * @retval None
 */
smartCard::smartCard(void (*const delayFunction)(),
		void (*const setClockOutputFunction)(),
		void (*const setResetOutputFunction)(),
		void (*const setIOInputFunction)(), void (*const setIOOutputFunction)(),
		void (*const writeClockFunction)(bool value),
		void (*const writeIOFunction)(bool value),
		void (*const writeResetFunction)(bool value),
		bool (*const readIOFunction)(), void (*const oneMiliDelayFunction)()) {
	SLE4442::m_delay = delayFunction;
	SLE4442::m_setClockOutput = setClockOutputFunction;
	SLE4442::m_setResetOutput = setResetOutputFunction;
	SLE4442::m_setIOInput = setIOInputFunction;
	SLE4442::m_setIOOutput = setIOOutputFunction;
	SLE4442::m_writeClock = writeClockFunction;
	SLE4442::m_writeIO = writeIOFunction;
	SLE4442::m_writeReset = writeResetFunction;
	SLE4442::m_readIO = readIOFunction;
	SLE4442::m_oneMiliDelay = oneMiliDelayFunction;
}

/**
 * @brief  Initialize the smart card
 * @param  data : returns the first four bytes in main memory
 * @retval None
 */
SmartCard_Results smartCard::initialize(uint8_t *data) {
	auto ret = SLE4442::initialize(data);
	m_isInitiated = ret;
	return ret ? SmartCard_Results::OK : SmartCard_Results::CardNotFound;
}

SmartCardAuth_Results smartCard::authenticate(uint8_t password1,
		uint8_t password2, uint8_t password3) {
	if (!m_isInitiated)
		return SmartCardAuth_Results::NotInitiatedCard;
	m_isAuthenticated = false;
	auto ret = SLE4442::authenticate(password1, password2, password3);
	switch (ret) {
	case 0x00:
		return SmartCardAuth_Results::CardIsLocked;
	case 0x01:
		return SmartCardAuth_Results::InvalidPSC1;
	case 0x03:
		return SmartCardAuth_Results::InvalidPSC2;
	case 0x07:
		m_isAuthenticated = true;
		return SmartCardAuth_Results::PSCVerified;
	default:
		return SmartCardAuth_Results::Error;
	}
}

SmartCard_Results smartCard::readMainMemory(uint8_t location, uint8_t *value) {
	if (!m_isInitiated)
		return SmartCard_Results::NotInitiated;
	auto d = SLE4442::readMainMemory(location);
	*value = d;
	return SmartCard_Results::OK;
}

SmartCard_Results smartCard::writeMainMemory(uint8_t location, uint8_t value) {
	if (!m_isInitiated)
		return SmartCard_Results::NotInitiated;
	if (!m_isAuthenticated)
		return SmartCard_Results::NotAuthenticated;
	SLE4442::writeMainMemory(location, value);
	return SmartCard_Results::OK;
}

SmartCard_Results smartCard::readProtectedMemory(uint8_t *values) {
	if (!m_isInitiated)
		return SmartCard_Results::NotInitiated;
	SLE4442::readProtectedMemory(values);
	return SmartCard_Results::OK;
}

SmartCard_Results smartCard::writeProtectedMemory(uint8_t location,
		uint8_t value) {
	if (!m_isInitiated)
		return SmartCard_Results::NotInitiated;
	if (!m_isAuthenticated)
		return SmartCard_Results::NotAuthenticated;
	SLE4442::writeProtectedMemory(location, value);
	return SmartCard_Results::OK;
}

SmartCard_Results smartCard::changePassword(uint8_t password1,
		uint8_t password2, uint8_t password3) {
	if (!m_isInitiated)
		return SmartCard_Results::NotInitiated;
	if (!m_isAuthenticated)
		return SmartCard_Results::NotAuthenticated;
	SLE4442::writeSecuredMemory(1, password1);
	SLE4442::writeSecuredMemory(2, password2);
	SLE4442::writeSecuredMemory(3, password3);
	m_isAuthenticated = false;
	return SmartCard_Results::OK;
}

} /* namespace smartCards */
