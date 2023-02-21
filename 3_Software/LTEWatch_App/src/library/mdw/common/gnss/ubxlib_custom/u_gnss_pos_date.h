/*
 * Copyright 2019-2022 u-blox
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _U_GNSS_POS_DATE_H_
#define _U_GNSS_POS_DATE_H_

/* Only header files representing a direct and unavoidable
 * dependency between the API of this module and the API
 * of another module should be included here; otherwise
 * please keep #includes to your .c files. */

#include "ubxlib.h"
#include "u_device.h"

/** \addtogroup _GNSS
 *  @{
 */

/** @file
 * @brief This header file defines the GNSS APIs to read position.
 */

#ifdef __cplusplus
extern "C" {
#endif

int32_t uGnssPosDateGetStart(uDeviceHandle_t gnssHandle,
                         void (*pCallback) (uDeviceHandle_t gnssHandle,
                                            int32_t errorCode,
                                            int32_t latitudeX1e7,
                                            int32_t longitudeX1e7,
                                            int32_t altitudeMillimetres,
                                            int32_t radiusMillimetres,
                                            int32_t speedMillimetresPerSecond,
                                            int32_t svs,
                                            int64_t timeUtc,
                                            uint16_t yearVal,
                                            uint8_t  monthVal,
                                            uint8_t  dayVal,
                                            uint8_t  hourVal,
                                            uint8_t  minuteVal,
                                            uint8_t  secondVal));

/** Cancel a uGnssPosGetStart(); after this function has returned the
 * callback passed to uGnssPosGetStart() will not be called until another
 * uGnssPosGetStart() is begun.  uGnssPosGetStart() also creates a mutex
 * for thread safety which will remain in the system even after
 * pCallback has been called; this will free the memory it occupies.
 *
 * @param gnssHandle  the handle of the GNSS instance.
 */
void uGnssPosDateGetStop(uDeviceHandle_t gnssHandle);

/** Get the binary RRLP information directly from the GNSS chip,
 * as returned by the UBX-RXM-MEASX command of the UBX protocol.  This
 * is more efficient, both in terms of power and time, than asking
 * for position: the RRLP information may be sent to the u-blox
 * Cloud Locate service where the exact position of the device can
 * be determined and made available, e.g. for trackers.
 *
 * @param gnssHandle                    the handle of the GNSS instance to use.
 * @param pBuffer                       a place to store the binary RRLP
 *                                      information; cannot be NULL.  The
 *                                      storage required for each RRLP data set
 *                                      is: 8 + 44 + 24 * [number of satellites]
 *                                      so for, e.g. 32 satellites, 820 bytes
 *                                      would be sufficient.  Note that what
 *                                      is written to pBuffer includes the six
 *                                      bytes of message header of the UBX
 *                                      protocol, i.e. 0xB5 62 02 14 AA BB, where
 *                                      0xAA BB is the [little-endian coded] 16-bit
 *                                      length of the RRLP data that follows;
 *                                      the two CRC bytes from the UBX protocol are
 *                                      ALSO written to pBuffer because Cloud Locate
 *                                      not only expects them it requires them AND it
 *                                      checks them.
 * @param sizeBytes                     the number of bytes of storage at pBuffer.
 * @param svsThreshold                  the minimum number of satellites that must
 *                                      be visible to return the RRLP information;
 *                                      specify -1 for "don't care"; the recommended
 *                                      value to use for the Cloud Locate service is 5.
 * @param cNoThreshold                  the minimum carrier to noise value that must
 *                                      be met to return the RRLP information, range
 *                                      0 to 63; specify -1 for "don't care".  The
 *                                      ideal value to use for the Cloud Locate service
 *                                      is 35 but that requires clear sky and a good
 *                                      antenna, hence the recommended value is 30;
 *                                      lower threshold values may work, just less
 *                                      reliably.
 * @param multipathIndexLimit           the maximum multipath index that must be
 *                                      met to return the RRLP information, 1 = low,
 *                                      2 = medium, 3 = high; specify -1 for "don't
 *                                      care".  The recommended value for the Cloud
 *                                      Locate service is 1.
 * @param pseudorangeRmsErrorIndexLimit the maximum pseudorange RMS error index that
 *                                      must be met to return the RRLP information;
 *                                      specify -1 for "don't care".  The recommended
 *                                      value for the Cloud Locate service is 3.
 * @param[in] pKeepGoingCallback        a callback function that governs the wait. This
 *                                      This function is called while waiting for RRLP
 *                                      data that meets the criteria; the API will
 *                                      only continue to wait while the callback function
 *                                      returns true.  This allows the caller to terminate
 *                                      the process at their convenience. The function may
 *                                      also be used to feed any watchdog timer that
 *                                      might be running. May be NULL, in which case
 *                                      this function will stop when
 *                                      #U_GNSS_POS_TIMEOUT_SECONDS have elapsed.  The
 *                                      single int32_t parameter is the GNSS handle.
 * @return                              on success the number of bytes returned, else
 *                                      negative error code.
 */
#ifdef __cplusplus
}
#endif

/** @}*/

#endif // _U_GNSS_POS_H_

// End of file
