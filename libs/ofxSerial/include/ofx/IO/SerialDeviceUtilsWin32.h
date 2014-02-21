// =============================================================================
//
// Copyright (c) 2014 Takanobu Inafuku <http://www.inafact.net>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
// =============================================================================


#pragma once


#include "ofLog.h"
#include "ofx/IO/AbstractTypes.h"
#include "ofx/IO/DirectoryUtils.h"


namespace ofx {
    namespace IO {

#ifdef TARGET_WIN32

#include <winbase.h>
#include <tchar.h>
#include <iostream>
#include <string.h>
#include <setupapi.h>
#include <regstr.h>
#define MAX_SERIAL_PORTS 256
#include <winioctl.h>

#ifdef __MINGW32__
#define INITGUID
#include <initguid.h> // needed for dev-c++ & DEFINE_GUID
#endif

        // needed for serial bus enumeration:
        // 4d36e978-e325-11ce-bfc1-08002be10318}
        DEFINE_GUID (GUID_SERENUM_BUS_ENUMERATOR,
                     0x4D36E978,
                     0xE325,
                     0x11CE,
                     0xBF,
                     0xC1,
                     0x08,
                     0x00,
                     0x2B,
                     0xE1,
                     0x03,
                     0x18);
#endif

        class SerialDeviceUtilsWin32
        {
        public:

#ifdef TARGET_WIN32
            //! maybe, protected or private member..
            char 		** portNamesShort;//[MAX_SERIAL_PORTS];
            char 		** portNamesFriendly; ///[MAX_SERIAL_PORTS];
            HANDLE  	hComm;		// the handle to the serial port pc
            int	 		nPorts;
            bool 		bPortsEnumerated;
            COMMTIMEOUTS 	oldTimeout;	// we alter this, so keep a record
#endif

            static SerialDeviceUtilsWin32 &getInstance(){
                static SerialDeviceUtilsWin32 instance;
                return instance;
            };

            void enumerateWin32Ports(){
#ifdef TARGET_WIN32
                if (bPortsEnumerated == true) return;

                HDEVINFO hDevInfo = NULL;
                SP_DEVINFO_DATA DeviceInterfaceData;
                int i = 0;
                DWORD dataType, actualSize = 0;
                unsigned char dataBuf[MAX_PATH + 1];

                // Reset Port List
                nPorts = 0;
                // Search device set
                hDevInfo = SetupDiGetClassDevs((struct _GUID *)&GUID_SERENUM_BUS_ENUMERATOR,0,0,DIGCF_PRESENT);
                if ( hDevInfo ){
                    while (TRUE){
                        ZeroMemory(&DeviceInterfaceData, sizeof(DeviceInterfaceData));
                        DeviceInterfaceData.cbSize = sizeof(DeviceInterfaceData);
                        if (!SetupDiEnumDeviceInfo(hDevInfo, i, &DeviceInterfaceData)){
                            // SetupDiEnumDeviceInfo failed
                            break;
                        }

                        if (SetupDiGetDeviceRegistryPropertyA(hDevInfo,
                                                              &DeviceInterfaceData,
                                                              SPDRP_FRIENDLYNAME,
                                                              &dataType,
                                                              dataBuf,
                                                              sizeof(dataBuf),
                                                              &actualSize)){

                            sprintf(portNamesFriendly[nPorts], "%s", dataBuf);
                            portNamesShort[nPorts][0] = 0;

                            // turn blahblahblah(COM4) into COM4

                            char *   begin    = NULL;
                            char *   end    = NULL;
                            begin          = strstr((char *)dataBuf, "COM");


                            if (begin)
                                {
                                    end          = strstr(begin, ")");
                                    if (end)
                                        {
                                            *end = 0;   // get rid of the )...
                                            strcpy(portNamesShort[nPorts], begin);
                                        }
                                    if (nPorts++ > MAX_SERIAL_PORTS)
                                        break;
                                }
                        }
                        i++;
                    }
                }
                SetupDiDestroyDeviceInfoList(hDevInfo);

                bPortsEnumerated = false;  
#endif
            };

            ~SerialDeviceUtilsWin32(){
#ifdef TARGET_WIN32
                //---------------------------------------------
                nPorts 				= 0;
                bPortsEnumerated 	= false;

                for (int i = 0; i < MAX_SERIAL_PORTS; i++) {
                    delete [] portNamesShort[i];
                    delete [] portNamesFriendly[i];
                }
                delete [] portNamesShort;
                delete [] portNamesFriendly;

                //---------------------------------------------
#endif
            }

        private:
            SerialDeviceUtilsWin32(){
#ifdef TARGET_WIN32
                //---------------------------------------------
                nPorts 				= 0;
                bPortsEnumerated 	= false;

                portNamesShort = new char * [MAX_SERIAL_PORTS];
                portNamesFriendly = new char * [MAX_SERIAL_PORTS];
                for (int i = 0; i < MAX_SERIAL_PORTS; i++){
                    portNamesShort[i] = new char[10];
                    portNamesFriendly[i] = new char[MAX_PATH];
                }
                //---------------------------------------------
#endif
            }
            SerialDeviceUtilsWin32(const SerialDeviceUtilsWin32 &other){}
            SerialDeviceUtilsWin32 &operator=(const SerialDeviceUtilsWin32 &other){}

        protected:

// #ifdef TARGET_WIN32
//             char 		** portNamesShort;//[MAX_SERIAL_PORTS];
//             char 		** portNamesFriendly; ///[MAX_SERIAL_PORTS];
//             HANDLE  	hComm;		// the handle to the serial port pc
//             int	 		nPorts;
//             bool 		bPortsEnumerated;
//             COMMTIMEOUTS 	oldTimeout;	// we alter this, so keep a record
// #endif

        };
    }
} // namespace ofx::IO
