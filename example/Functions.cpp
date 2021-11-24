/*
 * Functions.c
 *
 *  Created on: Nov 20, 2021
 *      Author: mrras
 */
#include "Functions.h"
#include "SmartCard.h"
#include "main.h"

static void delay();
static void setClockOutput();
static void setIOInput();
static void setIOOutput();
static void setResetOutput();
static bool readIO();
static void writeIO(bool value);
static void writeClock(bool value);
static void writeReset(bool value);
static void oneMiliDelay();

using namespace smartCards;

smartCard sc(delay, setClockOutput, setResetOutput, setIOInput, setIOOutput,
		writeClock, writeIO, writeReset, readIO, oneMiliDelay);

void initSmartCard(uint8_t *data) {
	uint8_t d;
	auto t = sc.initialize(data);
	if (t == SmartCard_Results::OK) {
		auto r = sc.authenticate(0xFF, 0xFF, 0xFF);
		if (r == SmartCardAuth_Results::PSCVerified) {
			for (int i = 0; i < 256; ++i) {
				t = sc.writeMainMemory(i, i);
			}
			for (int i = 0; i < 256; ++i) {
				t = sc.readMainMemory(i, &d);
			}
		}
	}
}

static void delay() {
	asm("nop");
}

static void setClockOutput() {
	//TODO: set clock pin as output
}

static void setIOInput() {
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };
	GPIO_InitStruct.Pin = SMARTCARD_IO_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(SMARTCARD_IO_GPIO_Port, &GPIO_InitStruct);
}

static void setIOOutput() {
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };
	GPIO_InitStruct.Pin = SMARTCARD_IO_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(SMARTCARD_IO_GPIO_Port, &GPIO_InitStruct);
}

static void setResetOutput() {
	//TODO:	set reset pin as output
}

static bool readIO() {
	return (SMARTCARD_IO_GPIO_Port->IDR & SMARTCARD_IO_Pin) != 0x00U;
}

static void writeIO(bool value) {
	if (value)
		SMARTCARD_IO_GPIO_Port->BSRR = SMARTCARD_IO_Pin;
	else
		SMARTCARD_IO_GPIO_Port->BSRR = (uint32_t) SMARTCARD_IO_Pin << 16;
}

static void writeClock(bool value) {
	if (value)
		SMARTCARD_CLOCK_GPIO_Port->BSRR = SMARTCARD_CLOCK_Pin;
	else
		SMARTCARD_CLOCK_GPIO_Port->BSRR = (uint32_t) SMARTCARD_CLOCK_Pin << 16;
}

static void writeReset(bool value) {
	if (value)
		SMARTCARD_RESET_GPIO_Port->BSRR = SMARTCARD_RESET_Pin;
	else
		SMARTCARD_RESET_GPIO_Port->BSRR = (uint32_t) SMARTCARD_RESET_Pin << 16;
}

static void oneMiliDelay() {
	HAL_Delay(1);
}
