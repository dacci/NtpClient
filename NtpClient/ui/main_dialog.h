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

#ifndef NTPCLIENT_UI_MAIN_DIALOG_H_
#define NTPCLIENT_UI_MAIN_DIALOG_H_

#include <atlbase.h>
#include <atlstr.h>

#pragma warning(push)
#pragma warning(disable: 4302 4838)

#include <atlapp.h>
#include <atlcrack.h>
#include <atlctrls.h>
#include <atlddx.h>
#include <atldlgs.h>
#include <atlframe.h>

#pragma warning(pop)

#include "misc/ntp.h"
#include "res/resource.h"

class MainDialog
    : public CDialogImpl<MainDialog>,
      public CWinDataExchange<MainDialog>,
      public CDialogResize<MainDialog>,
      public CMessageFilter {
 public:
  static const UINT IDD = IDD_MAIN;

  MainDialog();
  ~MainDialog();

  BEGIN_MSG_MAP(MainDialog)
    MSG_WM_INITDIALOG(OnInitDialog)
    MSG_WM_DESTROY(OnDestroy)

    NOTIFY_HANDLER_EX(IDOK, BCN_DROPDOWN, OnOkDropDown)
    NOTIFY_HANDLER_EX(IDC_RESULT, NM_DBLCLK, OnResultDoubleClicked)

    COMMAND_RANGE_HANDLER_EX(ID_FILE_MRU_FILE2, ID_FILE_MRU_FILE6, OnChangeMode)
    COMMAND_ID_HANDLER_EX(IDOK, OnOK)
    COMMAND_ID_HANDLER_EX(IDYES, OnYes)
    COMMAND_ID_HANDLER_EX(IDNO, OnNo)
    COMMAND_ID_HANDLER_EX(IDCANCEL, OnCancel)

    CHAIN_MSG_MAP(CDialogResize)
  END_MSG_MAP()

  BEGIN_DDX_MAP(MainDialog)
    DDX_CONTROL_HANDLE(IDC_ADDRESS, address_)
    DDX_CONTROL_HANDLE(IDOK, ok_)
    DDX_CONTROL_HANDLE(IDC_RESULT, result_)
  END_DDX_MAP()

  BEGIN_DLGRESIZE_MAP(MainDialog)
    DLGRESIZE_CONTROL(IDC_ADDRESS, DLSZ_SIZE_X)
    DLGRESIZE_CONTROL(IDOK, DLSZ_MOVE_X)
    DLGRESIZE_CONTROL(IDC_RESULT, DLSZ_SIZE_X | DLSZ_SIZE_Y)
  END_DLGRESIZE_MAP()

  BOOL PreTranslateMessage(MSG* message) override;

 private:
  BOOL OnInitDialog(CWindow focus, LPARAM init_param);
  void OnDestroy();

  LRESULT OnOkDropDown(NMHDR* header);
  LRESULT OnResultDoubleClicked(NMHDR* header);

  void OnChangeMode(UINT notify_code, int id, CWindow control);
  void OnOK(UINT notify_code, int id, CWindow control);
  void OnYes(UINT notify_code, int id, void* context);
  void OnNo(UINT notify_code, int id, void* context);
  void OnCancel(UINT notify_code, int id, CWindow control);

  HANDLE event_;
  CWindow address_;
  CButton ok_;
  CListViewCtrl result_;
  NTP_MODE mode_;

  NTP_PACKET* response_;
};

#endif  // NTPCLIENT_UI_MAIN_DIALOG_H_
