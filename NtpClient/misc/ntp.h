/*
 * Copyright (c) 2013 dacci.org
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef NTPCLIENT_MISC_NTP_H_
#define NTPCLIENT_MISC_NTP_H_

#include <stdint.h>

#include <pshpack1.h>

typedef struct tagNTP_SHORT_TIME {
  uint16_t seconds;
  uint16_t fraction;
} NTP_SHORT_TIME;

typedef struct tagNTP_TIMESTAMP {
  uint32_t seconds;
  uint32_t fraction;
} NTP_TIMESTAMP;

enum NTP_LEAP {
  NTP_LEAP_NO_WARNING,
  NTP_LEAP_ADD,
  NTP_LEAP_DEL,
  NTP_LEAP_UNKNOWN,
};

enum NTP_MODE {
  NTP_MODE_RESERVED,
  NTP_MODE_SYMMETRIC_ACTIVE,
  NTP_MODE_SYMMETRIC_PASSIVE,
  NTP_MODE_CLIENT,
  NTP_MODE_SERVER,
  NTP_MODE_BROADCAST,
  NTP_MODE_NTP_CONTROL,
  NTP_MODE_PRIVATE,
};

enum NTP_REF {
  // Geosynchronous Orbit Environment Satellite
  NTP_REF_GOES = 0x53454F47,

  // Global Position System
  NTP_REF_GPS  = 0x00535047,

  // Galileo Positioning System
  NTP_REF_GAL  = 0x004C4147,

  // Generic pulse-per-second
  NTP_REF_PPS  = 0x00535050,

  // Inter-Range Instrumentation Group
  NTP_REF_IRIG = 0x47495249,

  // LF Radio WWVB Ft. Collins, CO 60 kHz
  NTP_REF_WWVB = 0x42565757,

  // LF Radio DCF77 Mainflingen, DE 77.5 kHz
  NTP_REF_DCF  = 0x00464344,

  // LF Radio HBG Prangins, HB 75 kHz
  NTP_REF_HBG  = 0x00474248,

  // LF Radio MSF Anthorn, UK 60 kHz
  NTP_REF_MSF  = 0x0046534D,

  // LF Radio JJY Fukushima, JP 40 kHz, Saga, JP 60 kHz
  NTP_REF_JJY  = 0x00594A4A,

  // MF Radio LORAN C station, 100 kHz
  NTP_REF_LORC = 0x43524F4C,

  // MF Radio Allouis, FR 162 kHz
  NTP_REF_TDF  = 0x00464454,

  // HF Radio CHU Ottawa, Ontario
  NTP_REF_CHU  = 0x00554843,

  // HF Radio WWV Ft. Collins, CO
  NTP_REF_WWV  = 0x00565757,

  // HF Radio WWVH Kauai, HI
  NTP_REF_WWVH = 0x48565757,

  // NIST telephone modem
  NTP_REF_NIST = 0x5453494E,

  // NIST telephone modem
  NTP_REF_ACTS = 0x53544341,

  // USNO telephone modem
  NTP_REF_USNO = 0x4F4E5355,

  // European telephone modem
  NTP_REF_PTB  = 0x00425450,
};


typedef struct tagNTP_PACKET {
  struct {
    uint8_t mode : 3;
    uint8_t version: 3;
    uint8_t leap : 2;
  };
  uint8_t stratum;
  int8_t poll;
  int8_t precision;
  int root_delay;
  int root_dispersion;
  uint8_t reference_id[4];
  NTP_TIMESTAMP reference_timestamp;
  NTP_TIMESTAMP origin_timestamp;
  NTP_TIMESTAMP receive_timestamp;
  NTP_TIMESTAMP transmit_timestamp;
} NTP_PACKET;

#include <poppack.h>

#endif  // NTPCLIENT_MISC_NTP_H_
