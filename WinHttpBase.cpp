#include "stdafx.h"
#include "WinHttpBase.h"
#include <assert.h>


CWinHttpBase::CWinHttpBase(void): m_hSession(NULL), m_hConnect(NULL), m_hRequest(NULL)
{
}


CWinHttpBase::~CWinHttpBase(void)
{
    Cleanup();
}

BOOL CWinHttpBase::Download(LPCTSTR lpszUrl, LPCTSTR lpSaveFile, DWORD dwStartIndx, DWORD dwEndIndex)
{
    assert(NULL != lpszUrl);
    assert(NULL != lpSaveFile);

    do
    {
        if(!OpenSession())
        {
            break;
        }

        URL_COMPONENTS urlComp;

        if(!CrackUrl(lpszUrl, urlComp))
        {
            break;
        }

        BOOL bRet = OpenConnect(urlComp.lpszHostName, urlComp.nPort);
        delete[] urlComp.lpszHostName;

        if(!bRet)
        {
            break;
        }

        DWORD dwOpenRequestFlag = (INTERNET_SCHEME_HTTPS == urlComp.nScheme) ? WINHTTP_FLAG_SECURE : 0;

        if(!OpenRequest(_T("GET"), urlComp.lpszUrlPath, dwOpenRequestFlag))
        {
            break;
        }

        if(!SetWinHttpCallback(WinHttpStatusCallback))
        {
            break;
        }

        if(!SendRequest(WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, (DWORD_PTR)this))
        {
            break;
        }

        return TRUE;
    }
    while(FALSE);

    Cleanup();
    return FALSE;
}

void CWinHttpBase::Cleanup()
{
    if(NULL != m_hRequest)
    {
        WinHttpSetStatusCallback(m_hRequest, NULL, NULL, NULL);
        WinHttpCloseHandle(m_hRequest);
        m_hRequest = NULL;
    }

    if(NULL != m_hConnect)
    {
        WinHttpCloseHandle(m_hConnect);
        m_hConnect = NULL;
    }

    if(NULL != m_hSession)
    {
        WinHttpCloseHandle(m_hSession);
        m_hSession = NULL;
    }
}

BOOL CWinHttpBase::CrackUrl(LPCTSTR lpszUrl, URL_COMPONENTS& urlComp)
{
    ZeroMemory(&urlComp, sizeof(urlComp));
    urlComp.dwStructSize = sizeof(urlComp);
    urlComp.lpszHostName        = new TCHAR[MAX_PATH];
    urlComp.dwHostNameLength    = MAX_PATH;
    urlComp.dwUrlPathLength = -1;
    urlComp.dwSchemeLength = -1;

    if(!WinHttpCrackUrl(lpszUrl, 0, 0, &urlComp))
    {
        return FALSE;
    }

    return TRUE;
}

BOOL CWinHttpBase::OpenSession()
{
    if(NULL == m_hSession)
    {
        m_hSession = WinHttpOpen(_T("Asynchronous WinHTTP Demo/1.0"),
                                 WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
                                 WINHTTP_NO_PROXY_NAME,
                                 WINHTTP_NO_PROXY_BYPASS,
                                 WINHTTP_FLAG_ASYNC);
    }

    return (NULL != m_hSession);
}

BOOL CWinHttpBase::OpenConnect(LPCWSTR pswzServerName, INTERNET_PORT nServerPort)
{
    assert(NULL != m_hSession);

    if(NULL == m_hConnect)
    {
        m_hConnect = WinHttpConnect(m_hSession, pswzServerName, nServerPort, 0);
    }

    return (NULL != m_hConnect);
}

BOOL CWinHttpBase::OpenRequest(LPCTSTR lpVerb, LPCTSTR lpRelativeUrl, DWORD dwFlags)
{
    assert(NULL != m_hConnect);

    if(NULL == m_hRequest)
    {
        m_hRequest = WinHttpOpenRequest(m_hConnect, lpVerb, lpRelativeUrl, NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, dwFlags);
    }

    return (NULL != m_hRequest);
}

BOOL CWinHttpBase::SetWinHttpCallback(WINHTTP_STATUS_CALLBACK statusCallback)
{
    WINHTTP_STATUS_CALLBACK pCallback = WinHttpSetStatusCallback(m_hRequest, statusCallback, WINHTTP_CALLBACK_FLAG_ALL_COMPLETIONS | WINHTTP_CALLBACK_FLAG_REDIRECT,    NULL);
    return (WINHTTP_INVALID_STATUS_CALLBACK != pCallback);
}

BOOL CWinHttpBase::SendRequest(LPCWSTR pwszHeaders, DWORD dwHeadersLength, LPVOID lpOptional, DWORD dwOptionalLength, DWORD dwTotalLength, DWORD_PTR dwContext)
{
    assert(NULL != m_hRequest);
    BOOL bRet = WinHttpSendRequest(m_hRequest, pwszHeaders, dwHeadersLength, lpOptional, dwOptionalLength, dwTotalLength, dwContext);
    return bRet;
}

void __stdcall CWinHttpBase::WinHttpStatusCallback(HINTERNET hInternet, DWORD_PTR dwContext, DWORD dwInternetStatus, LPVOID lpvStatusInformation, DWORD dwStatusInformationLength)
{
    CWinHttpBase* pThis = (CWinHttpBase*)dwContext;

    if(NULL != pThis)
    {
        pThis->_WinHttpStatusCallback(dwInternetStatus, lpvStatusInformation, dwStatusInformationLength);
    }
}

void CWinHttpBase::_WinHttpStatusCallback(DWORD dwInternetStatus, LPVOID lpvStatusInformation, DWORD dwStatusInformationLength)
{
    switch(dwInternetStatus)
    {
        case WINHTTP_CALLBACK_STATUS_SENDREQUEST_COMPLETE:
        {
            OutputDebugString(_T("请求发送成功\r\n"));

            if(WinHttpReceiveResponse(m_hRequest, NULL) == FALSE)
            {
                Cleanup();
            }

            break;
        }

        case WINHTTP_CALLBACK_STATUS_HEADERS_AVAILABLE:
        {
            OutputDebugString(_T("返回头有效\r\n"));

            if(!QueryData())
            {
                Cleanup();
            }

            break;
        }

        case WINHTTP_CALLBACK_STATUS_DATA_AVAILABLE:
        {
            OutputDebugString(_T("返回数据有效\r\n"));
            DWORD dwDataSize = *((LPDWORD)lpvStatusInformation);

            if(0 == dwDataSize)
            {
                Cleanup();
            }
            else
            {
                BYTE* pBuf = new BYTE[dwDataSize];

                if(!ReadData(pBuf, dwDataSize))
                {
                    Cleanup();
                }

                delete[] pBuf;
            }

            break;
        }

        case WINHTTP_CALLBACK_STATUS_READ_COMPLETE:
        {
            OutputDebugString(_T("数据读取完成\r\n"));

            if(!QueryData())
            {
                Cleanup();
            }

            break;
        }

        case WINHTTP_CALLBACK_STATUS_REDIRECT:
        {
            OutputDebugString(_T("返回数据有效\r\n"));
            break;
        }

        case WINHTTP_CALLBACK_STATUS_REQUEST_ERROR:
        {
			OutputDebugString(_T("请求发生错误\r\n"));
            Cleanup();
            break;
        }

        default:
            break;
    }
}

BOOL CWinHttpBase::QueryData()
{
    assert(NULL != m_hRequest);
    return WinHttpQueryDataAvailable(m_hRequest, NULL);
}

BOOL CWinHttpBase::ReadData(LPVOID lpOutBuffer, DWORD dwBufSize)
{
    DWORD dwReaded = 0;
    BOOL bRet = WinHttpReadData(m_hRequest, lpOutBuffer, dwBufSize, &dwReaded);

    if(!bRet || (dwBufSize != dwReaded))
    {
        return FALSE;
    }

    return TRUE;
}
