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

#ifndef NTPCLIENT_APP_NTP_CLIENT_H_
#define NTPCLIENT_APP_NTP_CLIENT_H_

#include <atlbase.h>
#include <atlstr.h>
#include <atlwin.h>

#include <atlapp.h>

#include <winsock2.h>
#include <ws2ipdef.h>
#include <windns.h>
#include <iphlpapi.h>
#include <shellapi.h>

struct REQUEST_INFO : NET_ADDRESS_INFO, NC_ADDRESS {
  REQUEST_INFO() : NET_ADDRESS_INFO(), NC_ADDRESS(), window(), event() {
    pAddrInfo = this;
  }

  CWindow* window;
  HANDLE event;
};

extern CAppModule _Module;

void CALLBACK SimpleCallback(PTP_CALLBACK_INSTANCE instance, PVOID context);

#endif  // NTPCLIENT_APP_NTP_CLIENT_H_
