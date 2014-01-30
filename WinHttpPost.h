#pragma once
#include "WinHttpRequest.h"

class CWinHttpPost : public CWinHttpRequest
{
public:
    CWinHttpPost(void);
    virtual ~CWinHttpPost(void);

    BOOL PostRequest(LPCTSTR lpszUrl, LPCTSTR lpszParam);
    CStringA GetResponse(DWORD dwTimeout = INFINITE)const;
protected:
    BOOL CreateEvent();
    void CloseEvent();
    void Close();
    void OnDataArrived(REQUEST_STATUS status, LPVOID lpCurBuf, DWORD dwCurSize);
private:
    CStringA m_sResponse;
    HANDLE m_hEventWait;
    CStringA m_sPostData;
};

