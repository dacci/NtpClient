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

#include "misc/ntp_util.h"

void ConvertNtpTimestamp(const NTP_TIMESTAMP& timestamp, tm* time) {
  uint32_t temp = timestamp.seconds;

  time->tm_sec = temp % 60;
  temp /= 60;

  time->tm_min = temp % 60;
  temp /= 60;

  time->tm_hour = temp % 24 + 9;
  temp /= 24;

  time->tm_yday = temp % 365;
  temp /= 365;

  time->tm_year = temp;
  if (temp > 0)
    time->tm_yday -= (temp - 1) / 4 - (temp - 1) / 100 + (temp + 299) / 400;

  time->tm_mday = time->tm_yday + 1;
  if ((temp % 4) == 0 && (temp % 100) != 0 || (temp % 400) == 0 &&
      time->tm_mday > 31 + 28)
    --time->tm_mday;

  int mdays[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
  for (time->tm_mon = 0; time->tm_mon < 12; ++time->tm_mon) {
    if (time->tm_mday <= mdays[time->tm_mon])
      break;

    time->tm_mday -= mdays[time->tm_mon];
  }

  // calculate tm_wday by Zeller's congruence
  int y = time->tm_year + 1900;
  int m = time->tm_mon + 1;
  if (m < 3) {
    --y;
    m += 12;
  }

  int C = y / 100;
  int Y = y % 100;
  int G = 5 * C + C / 4;
  time->tm_wday = (time->tm_mday + 26 * (m + 1) / 10 + Y + Y / 4 + G + 6) % 7;

  time->tm_isdst = 0;
}

void FormatNtpTimestamp(const NTP_TIMESTAMP& timestamp, CString* result) {
  tm time;
  ::ConvertNtpTimestamp(timestamp, &time);

  CString fraction;
  fraction.Format(L"%.9lf",
                  static_cast<double>(timestamp.fraction) / 4294967296.0);
  fraction.Delete(0);

  const wchar_t* days[] = { L"Sun", L"Mon", L"Tue", L"Wed", L"Thu", L"Fri",
                            L"Sat" };
  const wchar_t* months[] = { L"Jan", L"Feb", L"Mar", L"Apr", L"May", L"Jun",
                              L"Jul", L"Aug", L"Sep", L"Oct", L"Nov", L"Dec"};

  result->Format(L"%s, %2d %s %d %02u:%02u:%02u%s",
                 days[time.tm_wday],
                 time.tm_mday,
                 months[time.tm_mon],
                 time.tm_year,
                 time.tm_hour,
                 time.tm_min,
                 time.tm_sec,
                 fraction);
}

void FormatNtpReferenceId(const uint8_t* ref_id, CString* result) {
  bool printable = true;
  bool null_id = true;
  for (int i = 0; i < 4; ++i) {
    if (ref_id[i] != 0) {
      null_id = false;
      if (!::isprint(ref_id[i])) {
        printable = false;
        break;
      }
    }
  }

  if (null_id) {
    *result = L"(null)";
    return;
  }

  result->Empty();

  for (int i = 0; i < 4; ++i) {
    if (printable) {
      result->AppendChar(ref_id[i]);
    } else {
      if (i)
        result->AppendChar(L'.');
      result->AppendFormat(L"%u", ref_id[i]);
    }
  }
}
