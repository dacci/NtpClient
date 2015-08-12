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

#include "app/ntp_client.h"

#include <ws2tcpip.h>

#include "misc/ntp_util.h"
#include "ui/main_dialog.h"

CAppModule _Module;

void CALLBACK SimpleCallback(PTP_CALLBACK_INSTANCE instance, PVOID context) {
  REQUEST_INFO* request_info = static_cast<REQUEST_INFO*>(context);
  bool succeeded = false;
  int error = 0;
  SOCKET sock = INVALID_SOCKET;

  if (request_info->Format == NET_ADDRESS_DNS_NAME) {
    if (request_info->PortNumber == 0)
      ::wcscpy_s(request_info->NamedAddress.Port, L"ntp");

    ADDRINFOW hints = { 0, AF_UNSPEC, SOCK_DGRAM }, *end_points = NULL;
    error = ::GetAddrInfoW(request_info->NamedAddress.Address,
                           request_info->NamedAddress.Port,
                           &hints, &end_points);
    if (error == 0) {
      for (ADDRINFOW* end_point = end_points; end_point;
            end_point = end_point->ai_next) {
        sock = ::socket(end_point->ai_family, end_point->ai_socktype,
                        end_point->ai_protocol);
        if (sock == INVALID_SOCKET) {
          error = ::WSAGetLastError();
          continue;
        }

        error = ::connect(sock, end_point->ai_addr, end_point->ai_addrlen);
        if (error == 0)
          break;

        error = ::WSAGetLastError();
        ::closesocket(sock);
        sock = INVALID_SOCKET;
      }

      ::FreeAddrInfoW(end_points);
    }
  } else {
    int address_length = 0;

    if (request_info->Format == NET_ADDRESS_IPV4) {
      address_length = sizeof(sockaddr_in);
      if (request_info->Ipv4Address.sin_port == 0)
        request_info->Ipv4Address.sin_port = ::htons(123);
    } else if (request_info->Format == NET_ADDRESS_IPV6) {
      address_length = sizeof(sockaddr_in6);
      if (request_info->Ipv6Address.sin6_port == 0)
        request_info->Ipv6Address.sin6_port = ::htons(123);
    }

    sock = ::socket(request_info->IpAddress.sa_family, SOCK_DGRAM, 0);
    if (sock != INVALID_SOCKET) {
      if (::connect(sock, &request_info->IpAddress, address_length) != 0) {
        error = ::WSAGetLastError();
        ::closesocket(sock);
        sock = INVALID_SOCKET;
      }
    } else {
      error = ::WSAGetLastError();
    }
  }

  NTP_PACKET* response = NULL;

  do {
    if (sock == INVALID_SOCKET)
      break;

    SYSTEMTIME system_time;
    ::GetSystemTime(&system_time);

    NTP_PACKET request = {};
    request.leap = NTP_LEAP_UNKNOWN;
    request.version = 3;
    request.mode = request_info->mode;

    ::SystemTimeToNtpTimestamp(system_time, &request.transmit_timestamp);
    request.transmit_timestamp.seconds =
        ::_byteswap_ulong(request.transmit_timestamp.seconds);
    request.transmit_timestamp.fraction =
        ::_byteswap_ulong(request.transmit_timestamp.fraction);

    error = ::send(sock, reinterpret_cast<char*>(&request), sizeof(request), 0);
    if (error <= 0) {
      error = ::WSAGetLastError();
      break;
    }

    response = new NTP_PACKET;
    WSABUF buffer = { sizeof(NTP_PACKET), reinterpret_cast<char*>(response) };
    OVERLAPPED overlapped = {};
    overlapped.hEvent = request_info->event;
    DWORD flags = 0;

    int result = ::WSARecv(sock, &buffer, 1, NULL, &flags, &overlapped, NULL);
    error = ::WSAGetLastError();
    if (result != 0 && error != WSA_IO_PENDING)
      break;

    error = ::WaitForSingleObject(overlapped.hEvent, 3000);
    if (error != WAIT_OBJECT_0) {
      if (error != WAIT_TIMEOUT)
        error = ::GetLastError();
      break;
    }

    DWORD length = 0;
    if (!::GetOverlappedResult(reinterpret_cast<HANDLE>(sock), &overlapped,
                               &length, FALSE)) {
      error = ::GetLastError();
      break;
    }

    succeeded = true;
  } while (0);

  if (sock != INVALID_SOCKET) {
    ::closesocket(sock);
    sock = INVALID_SOCKET;
  }

  if (!succeeded && response != NULL) {
    delete response;
    response = NULL;
  }

  if (succeeded)
    request_info->window->SendMessage(WM_COMMAND, MAKEWPARAM(IDYES, 0),
                                      reinterpret_cast<LPARAM>(response));
  else
    request_info->window->SendMessage(WM_COMMAND, MAKEWPARAM(IDNO, 0), error);

  delete request_info;
}

int __stdcall wWinMain(HINSTANCE hInstance, HINSTANCE, wchar_t*, int) {
  WSADATA wsa_data;
  ::WSAStartup(WINSOCK_VERSION, &wsa_data);
  ::AtlInitCommonControls(0xFFFF);
  ::InitNetworkAddressControl();
  _Module.Init(NULL, hInstance);

  {
    CMessageLoop message_loop;
    _Module.AddMessageLoop(&message_loop);

    MainDialog dialog;
    if (dialog.Create(NULL)) {
      dialog.ShowWindow(SW_SHOWNORMAL);
      dialog.UpdateWindow();
    }

    message_loop.Run();

    _Module.RemoveMessageLoop();
  }

  _Module.Term();
  ::WSACleanup();

  return 0;
}
