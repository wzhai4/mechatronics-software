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

#include <stdlib.h>
#include <unistd.h>
#include <cmath>
#include <iostream>
#include <sstream>
#include <fstream>

#include <Amp1394/AmpIORevision.h>

#if Amp1394_HAS_RAW1394

#include "FirewirePort.h"

#endif
#if Amp1394_HAS_PCAP
#include "Eth1394Port.h"
#endif

#include "AmpIO.h"

const unsigned long POT_TEST_ADC_COUNT[2] = {0x4000, 0x8000};
const unsigned long POT_TEST_ADC_ERROR_TOLERANCE = 0x500;


bool TestAnalogInputs(AmpIO **Board, BasePort *Port, std::ofstream &logFile) {
    if (Port->ReadAllBoards()) {
        for (int board_index = 0; board_index < 2; board_index++) {
            for (int channel_index = 0; channel_index < 4; channel_index++) {
                unsigned long reading = Board[board_index]->GetAnalogInput(channel_index);
                std::cout << "board " << board_index << " channel " << channel_index << " - " << " expected "
                          << POT_TEST_ADC_COUNT[board_index] << " measured " << reading;
                if (std::abs((long) reading - (long) POT_TEST_ADC_COUNT[board_index]) > POT_TEST_ADC_ERROR_TOLERANCE) {
                    std::cout << " ... [FAIL]" << std::endl;
                    // check for swapped cable
                    if (std::abs((long) reading - (long) POT_TEST_ADC_COUNT[~board_index]) <
                        POT_TEST_ADC_ERROR_TOLERANCE) {
                        std::cout << "is the 68-pin cable swapped?" << std::endl;
                    }
                    return false;
                } else {
                    std::cout << " ... [PASS]" << std::endl;
                }
            }
        }
    }
    return true;
}

//bool TestDigitalInputs(AmpIO *Board, BasePort *Port, std::ofstream &logFile) {
//    if (Port->ReadAllBoards()) {
//        for (int board_index = 0; board_index < 2; board_index++) {
////            digital_in = Board[board_index].
//        }
//    }
//    return true;
//}


void PrintDebugStream(std::stringstream &debugStream) {
    std::cerr << debugStream.str() << std::endl;
    debugStream.clear();
    debugStream.str("");
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
    bool requireQLA_SN = true;
    int args_found = 0;
    for (i = 1; i < (unsigned int) argc; i++) {
	if ((argv[i][0] == '-') && (argv[i][1] == 'k')) {
	    requireQLA_SN = false;
	} else 
        if ((argv[i][0] == '-') && (argv[i][1] == 'p')) {
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
            PrintDebugStream(debugStream);
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


    std::string logFilename("QLA_");

    for (j = 0; j < BoardList.size(); j++) {
        std::string QLA_SN = BoardList[j]->GetQLASerialNumber();
std::cout << QLA_SN <<std::endl;
        if (QLA_SN.empty()) {
            if (requireQLA_SN) {
                std::cerr << "Failed to get QLA serial number (specify -k command line option to continue test)"
                          << std::endl;
                return -1;
            }
            QLA_SN.assign("Unknown");
        }
        logFilename.append(QLA_SN);
        if (j != BoardList.size() - 1) {
            logFilename.append("_");
        }
    }

    logFilename.append(".log");
    std::ofstream logFile(logFilename.c_str());
    if (!logFile.good()) {
        std::cerr << "Failed to open log file " << logFilename << std::endl;
        return -1;
    }
    logFile << "====== TEST REPORT ======" << std::endl << std::endl;


    for (j = 0; j < BoardList.size(); j++) {
        std::cout << "Boards: " << board1 << " " << board2 << std::endl;
        std::string QLA_SN = BoardList[j]->GetQLASerialNumber();
        logFile << "QLA S/N: " << QLA_SN << std::endl;
        std::cout << "QLA S/N: " << QLA_SN << ". ";
        std::string FPGA_SN = BoardList[j]->GetFPGASerialNumber();
        logFile << "FPGA S/N: " << FPGA_SN << std::endl;
        std::cout << "FPGA S/N: " << FPGA_SN << ". ";
        uint32_t FPGA_VER = BoardList[j]->GetFirmwareVersion();
        logFile << "FPGA Firmware Version: " << FPGA_VER << std::endl;
        std::cout << "FPGA Firmware Version: " << FPGA_VER << std::endl;
    }

    TestAnalogInputs(&BoardList[0], Port, logFile);


    for (j = 0; j < BoardList.size(); j++) {
        Port->RemoveBoard(BoardList[j]);
    }

}
