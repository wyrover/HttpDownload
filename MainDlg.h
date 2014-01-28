// MainDlg.h : interface of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once
#include "DownloadFile.h"
#include <atlctrls.h>
#include "WinHttpGet.h"

class CMainDlg : public CDialogImpl<CMainDlg>
{
private:
//    CWinHttpRequest httpRequest;
    CDownloadFile m_downloadFile;
//  CDownloadFile* m_pDownloadFile;
    CProgressBarCtrl m_progressCtrl;
    CWinHttpGet m_httpGet;
public:
    enum { IDD = IDD_MAINDLG };

    BEGIN_MSG_MAP(CMainDlg)
    MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
    COMMAND_ID_HANDLER(ID_APP_ABOUT, OnAppAbout)
    COMMAND_ID_HANDLER(IDOK, OnOK)
    COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
    COMMAND_ID_HANDLER(IDC_START_DOWNLOAD, OnStartDwonload)
    COMMAND_ID_HANDLER(IDC_STOP_DOWNLOAD, OnStopDwonload)
    COMMAND_ID_HANDLER(IDC_QUIT, OnCancel)
    END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//  LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//  LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//  LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

    static void __stdcall OnDataProcess(REQUEST_STATUS status, LPVOID lpCurBuf, DWORD dwCurSize, LPVOID lpParam)
    {
        CMainDlg* pThis = (CMainDlg*)lpParam;
        CProgressBarCtrl progressCtrl = pThis->m_progressCtrl;
        CDownloadFile* pDownloadFile = &pThis->m_downloadFile;

        if(REQUEST_HEADERS_AVAILABLE == status)
        {
            progressCtrl.SetPos((int)(1.0 * pDownloadFile->GetDownloadedSize() / pDownloadFile->GetTotalSize() * 100));
        }
        else if(REQUEST_READCOMPLATE == status || REQUEST_READERROR == status)
        {
            if(REQUEST_READCOMPLATE == status)
            {
                progressCtrl.SetPos(100);
                ::MessageBox(NULL, _T("下载完成"), _T("成功"), MB_OK);
            }
            else
                ::MessageBox(NULL, _T("下载错误"), _T("失败"), MB_OK);

        }
        else if(REQUEST_READING == status)
        {
            pThis->m_progressCtrl.SetPos((int)(1.0 * pDownloadFile->GetDownloadedSize() / pDownloadFile->GetTotalSize() * 100));
        }
    }

    LRESULT OnDwonload(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
//         IWinHttpWrapper* pWinHttp = new CWinHttpBase();
//         if(NULL != pWinHttp)
//         {
//             const TCHAR* sUrl = _T("http://ww4.sinaimg.cn/bmiddle/89ad7439jw1ea5low3d1bj20c86esqv5.jpg");
//             const TCHAR* sSavePath = _T("Demo.jpg");
//             pWinHttp->Download(sUrl, sSavePath, 0, 0);
//         }

        return 0;
    }

    CStringW Utf8ToUnicode(CStringA lpszUtf8)
    {
        LPWSTR lpwBuf = NULL;

        try
        {
            //得到转换后的字符串长度
            DWORD dwBufLen = MultiByteToWideChar(CP_UTF8, 0, lpszUtf8, -1, NULL, NULL);
            //new buffer
            lpwBuf = new wchar_t[dwBufLen];

            if(NULL == lpwBuf)
            {
                return _T("");
            }

            memset(lpwBuf, 0, dwBufLen * sizeof(wchar_t));
            MultiByteToWideChar(CP_UTF8, 0, lpszUtf8, -1, lpwBuf, dwBufLen);
            CStringW s = lpwBuf;
            delete[] lpwBuf;
            return s;
        }
        catch(...)
        {
            return _T("");
        }

        return lpwBuf;
    }

    LRESULT OnStartDwonload(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        m_httpGet.PostRequest(_T("http://www.163.com"));
        CStringA sRes = m_httpGet.GetResponse(INFINITE);
        CString s = Utf8ToUnicode(sRes);

        CEdit editUrl = GetDlgItem(IDC_EDIT1);
        CEdit editSavePath = GetDlgItem(IDC_EDIT2);
        CString sUrl;
        editUrl.GetWindowText(sUrl);
        CString sSavePath;
        editSavePath.GetWindowText(sSavePath);

        m_downloadFile.SetCallback(OnDataProcess, this);
        m_downloadFile.DownloadFile(sUrl, sSavePath);
        return 0;
    }



    LRESULT OnStopDwonload(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        m_downloadFile.CancelRequest();
        return 0;
    }

    LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        // center the dialog on the screen
        CenterWindow();

        // set icons
        HICON hIcon = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON));
        SetIcon(hIcon, TRUE);
        HICON hIconSmall = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON));
        SetIcon(hIconSmall, FALSE);

        m_progressCtrl = GetDlgItem(IDC_PROGRESS1);
        m_progressCtrl.SetRange(0, 100);
        m_progressCtrl.SetPos(0);

        const TCHAR* sUrl = _T("http://dl_dir.qq.com/invc/qqplayer/QQPlayerMini_Setup_3.2.845.500.exe");
        CEdit editUrl = GetDlgItem(IDC_EDIT1);
        editUrl.SetWindowText(sUrl);

        const TCHAR* sSavePath = _T("test.exe");
        CEdit editSavePath = GetDlgItem(IDC_EDIT2);
        editSavePath.SetWindowText(sSavePath);
        return TRUE;
    }

    LRESULT OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        CSimpleDialog<IDD_ABOUTBOX, FALSE> dlg;
        dlg.DoModal();
        return 0;
    }

    LRESULT OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        // TODO: Add validation code
        EndDialog(wID);
        return 0;
    }

    LRESULT OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        EndDialog(wID);
        return 0;
    }
};
