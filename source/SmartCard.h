/*
 * SmartCard.h
 *
 *  Created on: Nov 20, 2021
 *      Author: mrras
 */

#ifndef MYDRIVERS_SMARTCARD_H_
#define MYDRIVERS_SMARTCARD_H_

#include <SLE4442.h>
#include "stdint.h"

namespace smartCards {

typedef enum {
	OK, NotInitiated, NotAuthenticated, CardNotFound
} SmartCard_Results;

typedef enum : uint8_t {
	CardIsLocked = 0x00,	//Card is locked
	InvalidPSC1 = 0x01,		//Invalid PSC, one try remaining
	InvalidPSC2 = 0x03,		//Invalid PSC, two tries remaining
	PSCVerified = 0x07,	//PSC verified, you may now write data to the card (Authentication is OK)
	NotInitiatedCard = 0xFF,
	Error = 0xFE			//Unable to Authenticate
} SmartCardAuth_Results;

class smartCard: public SLE4442 {
public:
	smartCard(void (*const delayFunction)(),
			void (*const setClockOutputFunction)(),
			void (*const setResetOutputFunction)(),
			void (*const setIOInputFunction)(),
			void (*const setIOOutputFunction)(),
			void (*const writeClockFunction)(bool value),
			void (*const writeIOFunction)(bool value),
			void (*const writeResetFunction)(bool value),
			bool (*const readIOFunction)(),
			void (*const oneMiliDelayFunction)());
	SmartCard_Results initialize(uint8_t *data);
	SmartCardAuth_Results authenticate(uint8_t password1 = 0xFF,
			uint8_t password2 = 0xFF, uint8_t password3 = 0xFF);

	SmartCard_Results readMainMemory(uint8_t location, uint8_t *value);
	SmartCard_Results writeMainMemory(uint8_t location, uint8_t value);

	SmartCard_Results readProtectedMemory(uint8_t *values);
	SmartCard_Results writeProtectedMemory(uint8_t location, uint8_t value);

	SmartCard_Results changePassword(uint8_t password1, uint8_t password2,
			uint8_t password3);
private:
	bool m_isAuthenticated = false, m_isInitiated = false;
};

} /* namespace smartCards */

#endif /* MYDRIVERS_SMARTCARD_H_ */
