/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/******************************************************************************
 *
 * This program is used to test the FPGA1394+QLA board, assuming that it is
 * connected to the FPGA1394-QLA-Test board. It relies on the curses library
 * and the AmpIO library (which depends on libraw1394 and/or pcap).
 *
 * Usage: qlatest [-pP] <board num>
 *        where P is the Firewire port number (default 0),
 *        or a string such as ethP and fwP, where P is the port number
 *
 ******************************************************************************/

//#include <stdlib.h>
//#include <unistd.h>
#include <cmath>
#include <iostream>
#include <sstream>
#include <fstream>
#include <chrono>
#include <thread>

#include <Amp1394/AmpIORevision.h>

#if Amp1394_HAS_RAW1394

#include "FirewirePort.h"

#endif
#if Amp1394_HAS_PCAP
#include "Eth1394Port.h"
#endif

#include "AmpIO.h"

enum Result {
    pass, fail, fatal_fail
};

const unsigned long POT_TEST_ADC_COUNT[2] = {0x4000, 0x8000};
const unsigned long POT_TEST_ADC_ERROR_TOLERANCE = 0x500;

const unsigned long DIGITAL_IN_TEST_HIGH_STATE[2] = {0x0fff, 0x0fff};
const unsigned int DIGITAL_IN_TEST_MASK[2] = {0x0fff, 0x0fff};
const unsigned long DIGITAL_IN_TEST_WAIT_TIME_MS = 200;
const unsigned long ENCODER_TEST_WAIT_TIME_MS = 2000;

const unsigned long ENCODER_TEST_INCREMENT = 10;
const unsigned long ENCODER_TEST_INCREMENT_TOLERANCE = 1;

const unsigned long POWER_AMP_TEST_DAC = 0x8500;
const unsigned long POWER_AMP_TEST_TOLERANCE = 0x200;

const auto PASS = " ... \033[0;32m[PASS]\033[0m";
const auto FAIL = " ... \033[1;91m[FAIL]\033[0m";

const std::string COLOR_OFF = "\033[0m";
const std::string COLOR_ERROR = "\033[0;91m";

Result TestAnalogInputs(AmpIO **Board, BasePort *Port) {
    Result result = pass;
    if (Port->ReadAllBoards()) {
        for (int board_index = 0; board_index < 2; board_index++) {
            for (int channel_index = 0; channel_index < 4; channel_index++) {
                unsigned long reading = Board[board_index]->GetAnalogInput(channel_index);
                std::cout << std::hex << "board " << board_index << " pot channel " << channel_index << " - "
                          << " expected="
                          << POT_TEST_ADC_COUNT[board_index] << " measured=" << reading;
                if (std::fabs((long) reading - (long) POT_TEST_ADC_COUNT[board_index]) > POT_TEST_ADC_ERROR_TOLERANCE) {
                    std::cout << FAIL << std::endl;
                    result = fail;
                    // check for swapped cable
                    if (std::fabs((long) reading - (long) POT_TEST_ADC_COUNT[1 ^ board_index]) <
                        POT_TEST_ADC_ERROR_TOLERANCE) {
                        std::cout << COLOR_ERROR << "(Looks like the 68-pin cable is crossed!)" << COLOR_OFF
                                  << std::endl;
                        result = fatal_fail;
                    }

                } else {
                    std::cout << PASS << std::endl;
                }
            }
        }
    }
    return result;
}

Result TestDigitalInputs(AmpIO **Board, BasePort *Port) {
    Result result = pass;
    unsigned long before[2];
    unsigned long after[2];
    if (Port->ReadAllBoards()) {
        for (int board_index = 0; board_index < 2; board_index++) {
            before[board_index] = Board[board_index]->GetDigitalInput() & DIGITAL_IN_TEST_MASK[board_index];
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(DIGITAL_IN_TEST_WAIT_TIME_MS));
        if (Port->ReadAllBoards()) {
            for (int board_index = 0; board_index < 2; board_index++) {
                after[board_index] = Board[board_index]->GetDigitalInput() & DIGITAL_IN_TEST_MASK[board_index];
            }
        }
    }

    for (int board_index = 0; board_index < 2; board_index++) {
        std::cout << std::hex << "board " << board_index << " din - expected="
                  << DIGITAL_IN_TEST_HIGH_STATE[board_index]
                  << " before=" << before[board_index] << " after=" << after[board_index];
        if (((before[board_index] == 0) && (after[board_index] == DIGITAL_IN_TEST_HIGH_STATE[board_index])) ||
            ((before[board_index] == DIGITAL_IN_TEST_HIGH_STATE[board_index]) && (after[board_index] == 0))) {
            std::cout << PASS << std::endl;
        } else {
            result = fail;
            std::cout << FAIL << std::endl;
        }
    }

    return result;

}

Result TestEncoders(AmpIO **Board, BasePort *Port) {
    Result result = pass;

    int64_t encoder_count_before[8];
    int64_t encoder_count_after[8];

    if (Port->ReadAllBoards()) {
        for (int board_index = 0; board_index < 2; board_index++) {
            for (int channel_index = 0; channel_index < 4; channel_index++) {
                encoder_count_before[board_index * 4 + channel_index] = Board[board_index]->GetEncoderPosition(
                        channel_index);
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(ENCODER_TEST_WAIT_TIME_MS));

        if (Port->ReadAllBoards()) {
            for (int board_index = 0; board_index < 2; board_index++) {
                for (int channel_index = 0; channel_index < 4; channel_index++) {
                    encoder_count_after[board_index * 4 + channel_index] = Board[board_index]->GetEncoderPosition(
                            channel_index);
                }
            }
        }
    }

    for (int i = 0; i < 7; i++) {
        auto increment = encoder_count_after[i] - encoder_count_before[i];

        std::cout << std::dec << "encoder " << i << " increment - expected=10 measured=" << increment;
        if (std::fabs(increment - ENCODER_TEST_INCREMENT) < ENCODER_TEST_INCREMENT_TOLERANCE) {
            std::cout << PASS << std::endl;
        } else {
            std::cout << FAIL << std::endl;
            result = fail;
        }
    }


    return result;
}

Result TestMotorPowerControl(AmpIO **Board, BasePort *Port) {
    Result result = pass;
    int mv_good_fail_count = 0;

    for (int board_index = 0; board_index < 2; board_index++) {
        Board[board_index]->WriteSafetyRelay(true);
        Board[board_index]->WritePowerEnable(true);
        Board[board_index]->WriteAmpEnable(0x0f, 0x0f);
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    Port->ReadAllBoards();

    for (int board_index = 0; board_index < 2; board_index++) {
        auto status = Board[board_index]->GetStatus();
        auto safety_relay_status = Board[board_index]->GetSafetyRelayStatus();
        std::cout << std::hex << "board " << board_index << " power - status=" << status << " safety_relay="
                  << safety_relay_status;

        if (safety_relay_status) {
            if ((status & 0x000cffff) == 0x000c0000) {
                std::cout << " mv_good=ok";
                if ((status & 0x000cffff) == 0x000c0f0f) {
                    std::cout << " amp_power=ok";
                    std::cout << PASS << std::endl;

                } else {
                    std::cout << " amp_power=bad";
                    result = fatal_fail;
                    std::cout << FAIL << std::endl;
                }
            } else {
                std::cout << " mv_good=bad";
                mv_good_fail_count++;
                result = fatal_fail;
                std::cout << FAIL << std::endl;
            }
        } else {
            result = fatal_fail;
            std::cout << FAIL << std::endl;
        }
    }


    if (mv_good_fail_count == 2) {
        // When both boards don't have mv_good
        std::cout << COLOR_ERROR << "(None of the board has motor power. Is the safety chain open?)" << COLOR_OFF <<
                  std::endl;
    }

    return result;
}

Result TestPowerAmplifier(AmpIO **Board, BasePort *Port) {
    int crossed_db9_error_count = 0;
    Result result = pass;

    for (int board_index = 0; board_index < 2; board_index++) {
        for (int channel_index = 0; channel_index < 4; channel_index++) {
            if (board_index == 1 && channel_index == 3) break;

            Port->ReadAllBoards();

            if (std::fabs(Board[0]->GetAnalogInput(0) - POT_TEST_ADC_COUNT[0] / 2) < POT_TEST_ADC_ERROR_TOLERANCE) {
                std::cout << COLOR_ERROR << "(unexpected current detected on channel 0)" << COLOR_OFF << std::endl;
                return fatal_fail;
            }

            Board[board_index]->SetMotorCurrent(channel_index, POWER_AMP_TEST_DAC);
            Port->WriteAllBoards();
            std::this_thread::sleep_for(std::chrono::milliseconds(500));

            Port->ReadAllBoards();
            auto motor_current = Board[board_index]->GetMotorCurrent(channel_index);
            bool amp_working = std::fabs(motor_current - POWER_AMP_TEST_DAC) <
                               POWER_AMP_TEST_TOLERANCE;
            bool channel_0_load_driven =
                    std::fabs(Board[0]->GetAnalogInput(0) - POT_TEST_ADC_COUNT[0] / 2) < POT_TEST_ADC_ERROR_TOLERANCE;

            if (board_index == 0 && channel_index == 0 && amp_working && !channel_0_load_driven) {
                crossed_db9_error_count++;
            }

            if (board_index == 1 && channel_index == 0 && amp_working && channel_0_load_driven) {
                crossed_db9_error_count++;
            }

            std::cout << std::hex << "board " << board_index << " amp channel " << channel_index << " - "
                      << " expected="
                      << POWER_AMP_TEST_DAC << " measured=" << motor_current;

            if (!amp_working) {
                result = fail;
                std::cout << FAIL << std::endl;
            } else {
                std::cout << PASS << std::endl;
            }

            Board[board_index]->SetMotorCurrent(channel_index, 0x8000);
            Port->WriteAllBoards();

        }
    }
    if (crossed_db9_error_count == 2) {
        result = fatal_fail;
        std::cout << COLOR_ERROR << "(DB9 cables are likely crossed!)" << COLOR_OFF << std::endl;

    }

    return result;

}

int main(int argc, char **argv) {
    unsigned int i, j;
#if Amp1394_HAS_RAW1394
    bool useFireWire = true;
#else
    bool useFireWire = false;
#endif
    int port = 0;
    int board1 = BoardIO::MAX_BOARDS;
    int board2 = BoardIO::MAX_BOARDS;
    int args_found = 0;
    for (i = 1; i < (unsigned int) argc; i++) {
        if ((argv[i][0] == '-') && (argv[i][1] == 'k')) {
        } else if ((argv[i][0] == '-') && (argv[i][1] == 'p')) {
            // -p option can be -pN, -pfwN, or -pethN, where N
            // is the port number. -pN is equivalent to -pfwN
            // for backward compatibility.
            if (strncmp(argv[i] + 2, "fw", 2) == 0)
                port = atoi(argv[i] + 4);
            else if (strncmp(argv[i] + 2, "eth", 3) == 0) {
                useFireWire = false;
                port = atoi(argv[i] + 5);
            } else
                port = atoi(argv[i] + 2);
            if (useFireWire)
                std::cerr << "Selecting FireWire port " << port << std::endl;
            else
                std::cerr << "Selecting Ethernet port " << port << std::endl;
        } else {
            if (args_found == 0) {
                board1 = atoi(argv[i]);
                std::cerr << "Selecting board " << board1 << std::endl;
            } else if (args_found == 1) {
                board2 = atoi(argv[i]);
                std::cerr << "Selecting board " << board2 << std::endl;
            }
            args_found++;
        }
    }

    std::stringstream debugStream(std::stringstream::out|std::stringstream::in);
    BasePort *Port;
    if (useFireWire) {
#if Amp1394_HAS_RAW1394
        Port = new FirewirePort(port, debugStream);
        if (!Port->IsOK()) {
            std::cerr << "Failed to initialize firewire port " << port << std::endl;
            return -1;
        }
#else
        std::cerr << "FireWire not available (set Amp1394_HAS_RAW1394 in CMake)" << std::endl;
        return -1;
#endif
    } else {
#if Amp1394_HAS_PCAP
        Port = new Eth1394Port(port, debugStream);
        if (!Port->IsOK()) {
            PrintDebugStream(debugStream);
            std::cerr << "Failed to initialize ethernet port " << port << std::endl;
            return -1;
        }
        Port->SetProtocol(BasePort::PROTOCOL_SEQ_RW);  // PK TEMP
#else
        std::cerr << "Ethernet not available (set Amp1394_HAS_PCAP in CMake)" << std::endl;
        return -1;
#endif
    }

    std::vector<AmpIO *> BoardList;
    std::vector<AmpIO_UInt32> FirmwareVersionList;
    BoardList.push_back(new AmpIO(board1));
    Port->AddBoard(BoardList[0]);
    BoardList.push_back(new AmpIO(board2));
    Port->AddBoard(BoardList[1]);
    FirmwareVersionList.clear();
    for (j = 0; j < BoardList.size(); j++) {
        FirmwareVersionList.push_back(BoardList[j]->GetFirmwareVersion());
    }

    for (i = 0; i < 4; i++) {
        for (j = 0; j < BoardList.size(); j++)
            BoardList[j]->WriteEncoderPreload(i, 0x1000 * i + 0x1000);
    }

    std::cout << "Boards: " << board1 << " " << board2 << std::endl;

    for (j = 0; j < BoardList.size(); j++) {
        std::string QLA_SN = BoardList[j]->GetQLASerialNumber();
        std::cout << "QLA S/N: " << QLA_SN << ". ";
        std::string FPGA_SN = BoardList[j]->GetFPGASerialNumber();
        std::cout << "FPGA S/N: " << FPGA_SN << ". ";
        uint32_t FPGA_VER = BoardList[j]->GetFirmwareVersion();
        std::cout << "FPGA Firmware Version: " << FPGA_VER << std::endl;
    }

    std::vector<Result (*)(AmpIO **, BasePort *)> test_functions;
    test_functions.push_back(TestAnalogInputs);
    test_functions.push_back(TestEncoders);
    test_functions.push_back(TestMotorPowerControl);
    test_functions.push_back(TestPowerAmplifier);

    for (auto test_function : test_functions) {
        auto result = test_function(&BoardList[0], Port);
        if (result == fatal_fail) break;
    }

    for (j = 0; j < BoardList.size(); j++) {
        Port->RemoveBoard(BoardList[j]);
    }

}
