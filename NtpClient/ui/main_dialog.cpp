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

#include "ui/main_dialog.h"

#include "app/ntp_client.h"
#include "misc/ntp.h"
#include "misc/ntp_util.h"

MainDialog::MainDialog() : event_(::CreateEvent(NULL, TRUE, FALSE, NULL)) {
}

MainDialog::~MainDialog() {
  ::CloseHandle(event_);
  event_ = NULL;
}

BOOL MainDialog::PreTranslateMessage(MSG* message) {
  return IsDialogMessage(message);
}

BOOL MainDialog::OnInitDialog(CWindow focus, LPARAM init_param) {
  DoDataExchange(DDX_LOAD);
  DlgResize_Init(true);

  NetAddr_SetAllowType(address_,
                        NET_STRING_ANY_ADDRESS | NET_STRING_ANY_SERVICE);
  address_.SetWindowText(L"time.windows.com");

  result_.SetExtendedListViewStyle(LVS_EX_FULLROWSELECT);
  result_.AddColumn(L"Field", 0);
  result_.AddColumn(L"Value", 1);

  result_.AddItem(0, 0, L"Leap");
  result_.AddItem(1, 0, L"Mode");
  result_.AddItem(2, 0, L"Version");
  result_.AddItem(3, 0, L"Stratum");
  result_.AddItem(4, 0, L"Poll");
  result_.AddItem(5, 0, L"Precision");
  result_.AddItem(6, 0, L"Root Delay");
  result_.AddItem(7, 0, L"Root Dispersion");
  result_.AddItem(8, 0, L"Reference ID");
  result_.AddItem(9, 0, L"Reference Timestamp");
  result_.AddItem(10, 0, L"Origin Timestamp");
  result_.AddItem(11, 0, L"Receive Timestamp");
  result_.AddItem(12, 0, L"Transmit Timestamp");

  result_.SetColumnWidth(0, LVSCW_AUTOSIZE);

  _Module.GetMessageLoop()->AddMessageFilter(this);

  return TRUE;
}

void MainDialog::OnDestroy() {
  _Module.GetMessageLoop()->RemoveMessageFilter(this);
  ::PostQuitMessage(0);
}

void MainDialog::OnOK(UINT notify_code, int id, CWindow control) {
  REQUEST_INFO* request_info = new REQUEST_INFO;
  HRESULT result = NetAddr_GetAddress(address_,
                                      static_cast<NC_ADDRESS*>(request_info));
  if (result != S_OK ||
      request_info->Format == NET_ADDRESS_FORMAT_UNSPECIFIED) {
    NetAddr_DisplayErrorTip(address_);
    return;
  }

  request_info->window = this;
  request_info->event = event_;

  if (::TrySubmitThreadpoolCallback(SimpleCallback, request_info, NULL))
    ok_.EnableWindow(FALSE);
  else
    delete request_info;
}

void MainDialog::OnYes(UINT notify_code, int id, void* context) {
  ok_.EnableWindow();

  NTP_PACKET* response = static_cast<NTP_PACKET*>(context);

  response->root_delay = ::_byteswap_ulong(response->root_delay);
  response->root_dispersion = ::_byteswap_ulong(response->root_dispersion);
  response->reference_timestamp.seconds =
      ::_byteswap_ulong(response->reference_timestamp.seconds);
  response->reference_timestamp.fraction =
      ::_byteswap_ulong(response->reference_timestamp.fraction);
  response->origin_timestamp.seconds =
      ::_byteswap_ulong(response->origin_timestamp.seconds);
  response->origin_timestamp.fraction =
      ::_byteswap_ulong(response->origin_timestamp.fraction);
  response->receive_timestamp.seconds =
      ::_byteswap_ulong(response->receive_timestamp.seconds);
  response->receive_timestamp.fraction =
      ::_byteswap_ulong(response->receive_timestamp.fraction);
  response->transmit_timestamp.seconds =
      ::_byteswap_ulong(response->transmit_timestamp.seconds);
  response->transmit_timestamp.fraction =
      ::_byteswap_ulong(response->transmit_timestamp.fraction);

  CString item;

  item.Format(L"%d", response->leap);
  result_.SetItemText(0, 1, item);

  item.Format(L"%d", response->mode);
  result_.SetItemText(1, 1, item);

  item.Format(L"%d", response->version);
  result_.SetItemText(2, 1, item);

  item.Format(L"%d", response->stratum);
  result_.SetItemText(3, 1, item);

  item.Format(L"%d", response->poll);
  result_.SetItemText(4, 1, item);

  item.Format(L"%d", response->precision);
  result_.SetItemText(5, 1, item);

  item.Format(L"%lf", response->root_delay / 65536.0);
  result_.SetItemText(6, 1, item);

  item.Format(L"%lf", response->root_dispersion / 65536.0);
  result_.SetItemText(7, 1, item);

  ::FormatNtpReferenceId(response->reference_id, &item);
  result_.SetItemText(8, 1, item);

  ::FormatNtpTimestamp(response->reference_timestamp, &item);
  result_.SetItemText(9, 1, item);

  ::FormatNtpTimestamp(response->origin_timestamp, &item);
  result_.SetItemText(10, 1, item);

  ::FormatNtpTimestamp(response->receive_timestamp, &item);
  result_.SetItemText(11, 1, item);

  ::FormatNtpTimestamp(response->transmit_timestamp, &item);
  result_.SetItemText(12, 1, item);

  result_.SetColumnWidth(1, LVSCW_AUTOSIZE);

  delete response;
}

void MainDialog::OnNo(UINT notify_code, int id, void* context) {
  ok_.EnableWindow();

  for (int i = 0, l = result_.GetItemCount(); i < l; ++i)
    result_.SetItemText(i, 1, NULL);

  DWORD error = reinterpret_cast<DWORD>(context);
  wchar_t* message = NULL;
  ::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
      NULL, error, 0, reinterpret_cast<wchar_t*>(&message), 0, NULL);

  MessageBox(message, NULL, MB_ICONERROR);

  ::LocalFree(message);
}

void MainDialog::OnCancel(UINT notify_code, int id, CWindow control) {
  DestroyWindow();
}
