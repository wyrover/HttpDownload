#include "stdafx.h"
#include "WinHttpPost.h"


CWinHttpPost::CWinHttpPost(void): CWinHttpRequest(VERB_TYPE_POST), m_hEventWait(NULL)
{
}


CWinHttpPost::~CWinHttpPost(void)
{
}

BOOL CWinHttpPost::PostRequest(LPCTSTR lpszUrl, LPCTSTR lpszParam)
{
    m_sResponse.Empty();
    m_sPostData.Empty();
    CloseEvent();

    if(CreateEvent())
    {
        CWinHttpHeader httpHeader(lpszUrl);
        httpHeader.AddHeader(_T("Content-Type: application/x-www-form-urlencoded"));

        if(Create(&httpHeader))
        {
            // This buffer must remain available until the request handle is closed
            // or the call to WinHttpReceiveResponse has completed.
            m_sPostData.Append(CW2A(lpszParam));

            if(SendRequest(m_sPostData.GetBuffer(), m_sPostData.GetLength()))
            {
                m_sPostData.ReleaseBuffer();
                return TRUE;
            }

            m_sPostData.ReleaseBuffer();
        }
    }

    return FALSE;
}

CStringA CWinHttpPost::GetResponse(DWORD dwTimeout /*= 5000*/) const
{
    ::WaitForSingleObject(m_hEventWait, dwTimeout);
    return m_sResponse;
}

void CWinHttpPost::OnDataArrived(REQUEST_STATUS status, LPVOID lpCurBuf, DWORD dwCurSize)
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

BOOL CWinHttpPost::CreateEvent()
{
    if(NULL == m_hEventWait)
    {
        m_hEventWait =::CreateEvent(NULL, TRUE, FALSE, NULL);
    }

    return (NULL != m_hEventWait);
}

void CWinHttpPost::CloseEvent()
{
    if(NULL != m_hEventWait)
    {
        ::SetEvent(m_hEventWait);
        ::CloseHandle(m_hEventWait);
        m_hEventWait = NULL;
    }
}

void CWinHttpPost::Close()
{
    CloseEvent();
    __super::Close();
}
