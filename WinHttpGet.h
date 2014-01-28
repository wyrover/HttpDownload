#pragma once
#include "WinHttpRequest.h"

// HTTP GET й╣ож
class CWinHttpGet : public CWinHttpRequest
{
public:
    CWinHttpGet(void);
    virtual ~CWinHttpGet(void);
    BOOL PostRequest(LPCTSTR lpszUrl);
    CStringA GetResponse(DWORD dwTimeout = 5000)const;
protected:
    void Close();
    virtual void OnDataArrived(REQUEST_STATUS status, LPVOID lpCurBuf, DWORD dwCurSize);
private:
    CStringA m_sResponse;
    HANDLE m_hEventWait;
};

