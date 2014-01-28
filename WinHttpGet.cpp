#include "stdafx.h"
#include "WinHttpGet.h"


CWinHttpGet::CWinHttpGet(void): CWinHttpRequest(VERB_TYPE_GET), m_hEventWait(NULL)
{
}


CWinHttpGet::~CWinHttpGet(void)
{
    Close();
}

BOOL CWinHttpGet::PostRequest(LPCTSTR lpszUrl)
{
    if(NULL != m_hEventWait)
    {
        ::SetEvent(m_hEventWait);
        ::CloseHandle(m_hEventWait);
    }

    m_hEventWait =::CreateEvent(NULL, TRUE, FALSE, NULL);

    if(NULL == m_hEventWait)
        return FALSE;

    m_sResponse.Empty();

    CWinHttpHeader httpHeader;
    httpHeader.SetUrl(lpszUrl);

    if(Create(&httpHeader))
    {
        if(SendRequest())
        {
            return TRUE;
        }
    }

    return FALSE;
}

CStringA CWinHttpGet::GetResponse(DWORD dwTimeout) const
{
    ::WaitForSingleObject(m_hEventWait, dwTimeout);
    return m_sResponse;
}

void CWinHttpGet::OnDataArrived(REQUEST_STATUS status, LPVOID lpCurBuf, DWORD dwCurSize)
{
    if(REQUEST_READCOMPLATE == status || REQUEST_READERROR == status || REQUEST_USERCANCEL == status)
    {
        ::SetEvent(m_hEventWait);
        Close();
    }
    else if(REQUEST_READING == status)
    {
        m_sResponse.AppendFormat(("%s"), lpCurBuf);
    }
    else if(REQUEST_HEADERS_AVAILABLE == status)
    {
        m_sResponse.Empty();
    }
}

void CWinHttpGet::Close()
{
    if(NULL != m_hEventWait)
    {
        ::SetEvent(m_hEventWait);
        ::CloseHandle(m_hEventWait);
        m_hEventWait = NULL;
    }

    __super::Close();
}
