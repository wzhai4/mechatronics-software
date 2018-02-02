/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Zihan Chen, Peter Kazanzides, Jie Ying Wu

  (C) Copyright 2011-2017 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <iostream>
#include <sstream>

#include "AmpIO.h"
//#include "FirewirePort.h"
#include "Amp1394Time.h"

#ifdef _MSC_VER
#include <stdlib.h>
inline quadlet_t bswap_32(quadlet_t data) { return _byteswap_ulong(data); }
#else

#include <byteswap.h>

#endif

const AmpIO_UInt32 VALID_BIT = 0x80000000;  /*!< High bit of 32-bit word */
const AmpIO_UInt32 MIDRANGE_ADC = 0x00008000;  /*!< Midrange value of ADC bits */
const AmpIO_UInt32 ENC_PRELOAD = 0x007fffff;  /*!< Encoder position preload value */
const AmpIO_Int32 ENC_MIDRANGE = 0x00800000;  /*!< Encoder position midrange value */

const AmpIO_UInt32 PWR_ENABLE = 0x000c0000;  /*!< Turn pwr_en on             */
const AmpIO_UInt32 PWR_DISABLE = 0x00080000;  /*!< Turn pwr_en off            */
const AmpIO_UInt32 RELAY_ON = 0x00030000;  /*!< Turn safety relay on       */
const AmpIO_UInt32 RELAY_OFF = 0x00020000;  /*!< Turn safety relay off      */
const AmpIO_UInt32 ENABLE_MASK = 0x0000ffff;  /*!< Mask for power enable bits */
const AmpIO_UInt32 MOTOR_CURR_MASK = 0x0000ffff;  /*!< Mask for motor current adc bits */
const AmpIO_UInt32 ANALOG_POS_MASK = 0xffff0000;  /*!< Mask for analog pot ADC bits */
const AmpIO_UInt32 ADC_MASK = 0x0000ffff;  /*!< Mask for right aligned ADC bits */
const AmpIO_UInt32 DAC_MASK = 0x0000ffff;  /*!< Mask for 16-bit DAC values */
const AmpIO_UInt32 ENC_POS_MASK = 0x00ffffff;  /*!< Encoder position mask (24 bits) */
const AmpIO_UInt32 ENC_OVER_MASK = 0x01000000;  /*!< Encoder bit overflow mask */
const AmpIO_UInt32 ENC_VEL_MASK_16 = 0x0000ffff;  /*!< Mask for encoder velocity (period) bits, Firmware Version <=5 (16 bits) */
const AmpIO_UInt32 ENC_VEL_MASK_22 = 0x003fffff;  /*!< Mask for encoder velocity (period) bits, Firmware Version >=6 (22 bits) */
const AmpIO_UInt32 ENC_FRQ_MASK = 0x0000ffff;  /*!< Mask for encoder velocity (frequency) bits [not used] */

// Following offsets are for FPGA Firmware Version 6+ (22 bits)
const AmpIO_UInt32 ENC_LATCH_MASK = 0x80000000;  /*!< Velocity based on latched value (1) or running counter (0) */
const AmpIO_UInt32 ENC_DIR_MASK = 0x40000000;  /*!< Mask for encoder velocity (period) direction bit */
const AmpIO_UInt32 ENC_CHN_MASK = 0x18000000;  /*!< Mask for encoder velocity (period) channel bits */

const AmpIO_UInt32 DAC_WR_A = 0x00300000;  /*!< Command to write DAC channel A */

const double FPGA_sysclk_MHz = 49.152;      /* FPGA sysclk in MHz (from FireWire) */


// PROGRESS_CALLBACK: inform the caller when the software is busy waiting: in this case,
//                    the parameter is NULL, but the function returns an error if
//                    the callback returns false.
// ERROR_CALLBACK:    inform the caller of an error; in this case, the error message
//                    (char *) is passed as a parameter, and the return value is ignored.

#define PROGRESS_CALLBACK(CB, ERR)             \
    if (CB) { if (!(*CB)(0)) return ERR; }     \
    else std::cout << '.';

#define ERROR_CALLBACK(CB, MSG)         \
    if (CB) (*CB)(MSG.str().c_str());   \
    else { std::cerr << MSG.str() << std::endl; }


AmpIO_UInt8 BitReverse4[16] = {0x0, 0x8, 0x4, 0xC,         // 0000, 0001, 0010, 0011
                               0x2, 0xA, 0x6, 0xE,         // 0100, 0101, 0110, 0111
                               0x1, 0x9, 0x5, 0xD,         // 1000, 1001, 1010, 1011
                               0x3, 0xB, 0x7, 0xF};       // 1100, 1101, 1110, 1111

AmpIO::AmpIO(AmpIO_UInt8 board_id, unsigned int numAxes) : BoardIO(board_id), NumAxes(numAxes) {

}

AmpIO::~AmpIO() {

}

void AmpIO::InitWriteBuffer(quadlet_t *buf, size_t data_offset) {

}

bool AmpIO::WriteBufferResetsWatchdog(void) const {
    return 1;
}

AmpIO_UInt32 AmpIO::GetFirmwareVersion(void) const {
    return 42L;
}

std::string AmpIO::GetFPGASerialNumber(void) {
    return "FPGA 1234-56";
}

std::string AmpIO::GetQLASerialNumber(void) {
    return "QLA 1234-56";
}

void AmpIO::DisplayReadBuffer(std::ostream &out) const {

}

bool AmpIO::HasEthernet(void) const {
    return 0;
}

AmpIO_UInt32 AmpIO::GetStatus(void) const {
    return 0L;
}

AmpIO_UInt32 AmpIO::GetTimestamp(void) const {
    return 0;
}

AmpIO_UInt32 AmpIO::GetDigitalInput(void) const {
    return 0;
}

AmpIO_UInt8 AmpIO::GetDigitalOutput(void) const {
    // Starting with Version 1.3.0 of this library, the digital outputs are inverted
    // before being returned to the caller because they are inverted in hardware and/or firmware.
    // This way, the digital output state matches the hardware state (i.e., 0 means digital output
    // is at 0V).
    return 0;
}

AmpIO_UInt8 AmpIO::GetNegativeLimitSwitches(void) const {
    return 0;
}

AmpIO_UInt8 AmpIO::GetPositiveLimitSwitches(void) const {
    return 0;
}

AmpIO_UInt8 AmpIO::GetHomeSwitches(void) const {
    return 0;
}

AmpIO_UInt8 AmpIO::GetEncoderChannelA(void) const {
    return 0;
}

bool AmpIO::GetEncoderChannelA(unsigned int index) const {
    return 0;
}

AmpIO_UInt8 AmpIO::GetEncoderChannelB(void) const {
    return 0;
}

bool AmpIO::GetEncoderChannelB(unsigned int index) const {
    return 0;
}

AmpIO_UInt8 AmpIO::GetEncoderIndex(void) const {
    return 0;
}

bool AmpIO::GetEncoderOverflow(unsigned int index) const {
    return 0; // send error "code"
}

AmpIO_UInt8 AmpIO::GetAmpTemperature(unsigned int index) const {
    return 0;
}

AmpIO_UInt32 AmpIO::GetMotorCurrent(unsigned int index) const {

    if (index >= NUM_CHANNELS)
        return 0L;

    return 0xffff / 2;
}

AmpIO_UInt32 AmpIO::GetAnalogInput(unsigned int index) const {

    return 0;
}

AmpIO_Int32 AmpIO::GetEncoderPosition(unsigned int index) const {

    if (index < NUM_CHANNELS) {
        return index;
    }
    return 0;
}

// Returns encoder velocity in counts/sec.
// For clarity and efficiency, this duplicates some code rather than calling GetEncoderVelocity.
double AmpIO::GetEncoderVelocityCountsPerSecond(unsigned int index) const {

    return 0;
}

// Returns encoder period; encoder velocity is 4/period.
AmpIO_Int32 AmpIO::GetEncoderVelocity(unsigned int index) const {

    return 0L;
}

AmpIO_UInt32 AmpIO::GetEncoderVelocityRaw(unsigned int index) const {
    return 0;
}

bool AmpIO::GetIsVelocityLatched(unsigned int index) const {
    bool ret = true;
    if (GetFirmwareVersion() >= 6)
        ret = (GetEncoderVelocityRaw(index) & ENC_LATCH_MASK);
    return ret;
}

int AmpIO::GetEncoderVelocityChannel(unsigned int index) const {
    return 0;
}

AmpIO_Int32 AmpIO::GetEncoderMidRange(void) const {
    return ENC_MIDRANGE;
}

bool AmpIO::GetPowerStatus(void) const {
    // Bit 19: MV_GOOD
    return 1;
}

bool AmpIO::GetSafetyRelayStatus(void) const {
    // Bit 17
    return 1;
}

bool AmpIO::GetWatchdogTimeoutStatus(void) const {
    // Bit 23
    return 0;
}

bool AmpIO::GetAmpEnable(unsigned int index) const {
    return 1;
}

bool AmpIO::GetAmpStatus(unsigned int index) const {
    return 1;
}

AmpIO_UInt32 AmpIO::GetSafetyAmpDisable(void) const {
    return 0x00;
}


/*******************************************************************************
 * Set commands
 */

void AmpIO::SetPowerEnable(bool state) {
}

bool AmpIO::SetAmpEnable(unsigned int index, bool state) {
    if (index < NUM_CHANNELS) {
        return true;
    }
    return false;
}

void AmpIO::SetSafetyRelay(bool state) {
}

bool AmpIO::SetMotorCurrent(unsigned int index, AmpIO_UInt32 sdata) {
//    quadlet_t data = 0x00;
//    data = VALID_BIT | ((BoardId & 0x0F) << 24) | DAC_WR_A | (sdata & DAC_MASK);

    if (index < NUM_CHANNELS) {
//        WriteBufferData[index+WB_CURR_OFFSET] = bswap_32(data);
        return true;
    } else
        return false;
}

/*******************************************************************************
 * Read commands
 */

AmpIO_UInt32 AmpIO::ReadStatus(void) const {
    return 0xffffffff;
}

bool AmpIO::ReadPowerStatus(void) const {
    return 1;

}

bool AmpIO::ReadSafetyRelayStatus(void) const {
    return 1;
}

AmpIO_UInt32 AmpIO::ReadSafetyAmpDisable(void) const {
    return 0x00000000;
}

bool AmpIO::ReadEncoderPreload(unsigned int index, AmpIO_Int32 &sdata) const {
    sdata = 0;
    return false;
}

bool AmpIO::ReadDoutControl(unsigned int index, AmpIO_UInt16 &countsHigh, AmpIO_UInt16 &countsLow) {

    return true;
}

/*******************************************************************************
 * Write commands
 */

bool AmpIO::WritePowerEnable(bool state) {
    return 1;
}

bool AmpIO::WriteAmpEnable(AmpIO_UInt8 mask, AmpIO_UInt8 state) {
    return 1;

}

bool AmpIO::WriteSafetyRelay(bool state) {
    return 1;

}

bool AmpIO::WriteEncoderPreload(unsigned int index, AmpIO_Int32 sdata) {
    return 1;

}

bool AmpIO::WriteDigitalOutput(AmpIO_UInt8 mask, AmpIO_UInt8 bits) {
    return 1;

}

bool AmpIO::WriteWatchdogPeriod(AmpIO_UInt32 counts) {
    return 1;

}

bool AmpIO::WriteDoutControl(unsigned int index, AmpIO_UInt16 countsHigh, AmpIO_UInt16 countsLow) {
    return 1;

}

bool AmpIO::WritePWM(unsigned int index, double freq, double duty) {
    return 1;

}

AmpIO_UInt32 AmpIO::GetDoutCounts(double time) const {
    return static_cast<AmpIO_UInt32>((FPGA_sysclk_MHz * 1e6) * time + 0.5);
}

/*******************************************************************************
 * PROM commands (M25P16)
 *    data e.g. 0x9f000000 the higher 8-bit is M25P16 cmd
 *    see DataSheet Table 5: Command Set Codes
 */

AmpIO_UInt32 AmpIO::PromGetId(void) {
    return 0L;
}

bool AmpIO::PromGetStatus(AmpIO_UInt32 &status, PromType type) {

    return 1;
}

bool AmpIO::PromGetResult(AmpIO_UInt32 &result, PromType type) {

    return 1;
}

bool AmpIO::PromReadData(AmpIO_UInt32 addr, AmpIO_UInt8 *data,
                         unsigned int nbytes) {
    return true;
}

bool AmpIO::PromWriteEnable(PromType type) {
    return 1;

}

bool AmpIO::PromWriteDisable(PromType type) {
    return 1;

}

bool AmpIO::PromSectorErase(AmpIO_UInt32 addr, const ProgressCallback cb) {

    return true;
}

int AmpIO::PromProgramPage(AmpIO_UInt32 addr, const AmpIO_UInt8 *bytes,
                           unsigned int nbytes, const ProgressCallback cb) {
    return nbytes;
}


nodeaddr_t AmpIO::GetPromAddress(PromType type, bool isWrite) {
    if (type == PROM_M25P16 && isWrite)
        return 0x0008;
    else if (type == PROM_M25P16 && !isWrite)
        return 0x0009;
    else if (type == PROM_25AA128 && isWrite)
        return 0x3000;
    else if (type == PROM_25AA128 && !isWrite)
        return 0x3002;
    else
        std::cerr << "AmpIO::GetPromAddress: unsupported PROM type " << type << std::endl;

    return 0x00;
}


// ********************** QLA PROM ONLY Methods ***********************************
bool AmpIO::PromReadByte25AA128(AmpIO_UInt16 addr, AmpIO_UInt8 &data) {
    return 1;

}

bool AmpIO::PromWriteByte25AA128(AmpIO_UInt16 addr, const AmpIO_UInt8 &data) {
    return 1;

}


// Read block data (quadlet)
bool AmpIO::PromReadBlock25AA128(AmpIO_UInt16 addr, quadlet_t *data, unsigned int nquads) {
    return 1;

}


// Write block data (quadlet)
bool AmpIO::PromWriteBlock25AA128(AmpIO_UInt16 addr, quadlet_t *data, unsigned int nquads) {
    return 1;

}

// ********************** KSZ8851 Ethernet Controller Methods ***********************************

bool AmpIO::ResetKSZ8851() {
    return 1;

}

bool AmpIO::WriteKSZ8851Reg(AmpIO_UInt8 addr, const AmpIO_UInt8 &data) {
    return 1;

}

bool AmpIO::WriteKSZ8851Reg(AmpIO_UInt8 addr, const AmpIO_UInt16 &data) {
    return 1;

}

bool AmpIO::ReadKSZ8851Reg(AmpIO_UInt8 addr, AmpIO_UInt8 &rdata) {
    return 1;

}

bool AmpIO::ReadKSZ8851Reg(AmpIO_UInt8 addr, AmpIO_UInt16 &rdata) {
    return 1;

}

// DMA access (no address specified)
// This assumes that the chip has already been placed in DMA mode
// (e.g., by writing to register 0x82).

bool AmpIO::WriteKSZ8851DMA(const AmpIO_UInt16 &data) {
    return 1;

}

bool AmpIO::ReadKSZ8851DMA(AmpIO_UInt16 &rdata) {

    return true;
}

AmpIO_UInt16 AmpIO::ReadKSZ8851ChipID() {
    return 0;
}

AmpIO_UInt16 AmpIO::ReadKSZ8851Status() {
    return 1;
}

bool AmpIO::ValidRead() const { return 1; }