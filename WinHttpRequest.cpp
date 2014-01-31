#include "stdafx.h"
#include "WinHttpRequest.h"
#include <assert.h>
#include <vector>
CWinHttpRequest::CWinHttpRequest(VERB_TYPE verbType): m_hSession(NULL), m_hConnect(NULL),
    m_hRequest(NULL), m_pHttpHeader(NULL), m_verbType(verbType), m_bUserCancel(FALSE), m_dwStatusCode(0)
{
    m_pHttpProxy = new ProxyResolver();
}


CWinHttpRequest::~CWinHttpRequest(void)
{
    delete m_pHttpProxy;
    m_pHttpProxy = NULL;
    Close();
}

BOOL CWinHttpRequest::Create(CWinHttpHeader* pHttpHeader)
{
    assert(NULL == m_hSession);
    assert(NULL == m_hConnect);
    assert(NULL == m_hRequest);
    assert(NULL != pHttpHeader);
    m_pHttpHeader = pHttpHeader;
    m_bUserCancel = FALSE;

    // 创建SESSION
    m_hSession = WinHttpOpen(_T("CWinHttpRequest/1.0"), WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME,
                             WINHTTP_NO_PROXY_BYPASS, WINHTTP_FLAG_ASYNC);

    if(NULL == m_hSession)
        return FALSE;

    // 解析URL
    CString sHost;
    INTERNET_PORT nPort = 80;
    CString sRelativePath;
    INTERNET_SCHEME sScheme = 0;

    if(!GetUrlComponent(sHost, nPort, sRelativePath, sScheme))
        return FALSE;

    // 创建CONNECT
    m_hConnect = WinHttpConnect(m_hSession, sHost, nPort, 0);

    if(NULL == m_hConnect)
        return FALSE;

    // 创建REQUEST
    DWORD dwOpenFlag = (INTERNET_SCHEME_HTTPS == sScheme) ? WINHTTP_FLAG_SECURE : 0;
    m_hRequest = WinHttpOpenRequest(m_hConnect, GetVerbText(), sRelativePath, NULL,
                                    WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, dwOpenFlag | WINHTTP_FLAG_REFRESH) ;

    if(NULL == m_hRequest)
        return FALSE;

    // 添加HTTP HEADER
    CString sHttpHeader = m_pHttpHeader->GetHeader();

    if(!sHttpHeader.IsEmpty())
    {
        BOOL bAdd = WinHttpAddRequestHeaders(m_hRequest, sHttpHeader, sHttpHeader.GetLength(), WINHTTP_ADDREQ_FLAG_ADD /*| WINHTTP_ADDREQ_FLAG_REPLACE*/);

        if(!bAdd)
            return FALSE;
    }

    // 设置代理
    if(NULL != m_pHttpProxy)
    {
        if(ERROR_SUCCESS != m_pHttpProxy->ResolveProxy(m_hRequest, m_pHttpHeader->GetUrl()))
            return FALSE;
    }

    // 设置回调函数
    WINHTTP_STATUS_CALLBACK pCallback = WinHttpSetStatusCallback(m_hRequest, WinHttpStatusCallback,
                                        WINHTTP_CALLBACK_FLAG_ALL_COMPLETIONS | WINHTTP_CALLBACK_FLAG_REDIRECT,    NULL);

    if(WINHTTP_INVALID_STATUS_CALLBACK == pCallback)
        return FALSE;

    return TRUE;
}

BOOL CWinHttpRequest::SendRequest(LPVOID lpOptional, DWORD dwOptionalLength)
{
    return WinHttpSendRequest(m_hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, lpOptional, dwOptionalLength, dwOptionalLength, (DWORD_PTR)this);
}

void CWinHttpRequest::Close()
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

void CWinHttpRequest::SetProxy(ProxyResolver* pHttpProxy)
{
    m_pHttpProxy = pHttpProxy;
}

CString CWinHttpRequest::GetVerbText() const
{
    switch(m_verbType)
    {
        case VERB_TYPE_GET :
            return _T("GET") ;

        case VERB_TYPE_POST :
        case VERB_TYPE_POST_MULTIPART :
            return _T("POST") ;

        case VERB_TYPE_DELETE :
            return _T("DELETE") ;
    }

    return _T("GET") ;
}

BOOL CWinHttpRequest::ReadData(LPVOID lpOutBuffer, DWORD dwBufSize)
{
    DWORD dwReaded = 0;
    BOOL bRet = WinHttpReadData(m_hRequest, lpOutBuffer, dwBufSize, &dwReaded);

    if(!bRet || (dwBufSize != dwReaded))
    {
        return FALSE;
    }

    return TRUE;
}

void __stdcall CWinHttpRequest::WinHttpStatusCallback(HINTERNET hInternet, DWORD_PTR dwContext, DWORD dwInternetStatus, LPVOID lpvStatusInformation, DWORD dwStatusInformationLength)
{
    CWinHttpRequest* pThis = (CWinHttpRequest*)dwContext;

    if(NULL != pThis)
    {
        pThis->_WinHttpStatusCallback(dwInternetStatus, lpvStatusInformation, dwStatusInformationLength);
    }
}

void CWinHttpRequest::_WinHttpStatusCallback(DWORD dwInternetStatus, LPVOID lpvStatusInformation, DWORD dwStatusInformationLength)
{
    switch(dwInternetStatus)
    {
        case WINHTTP_CALLBACK_STATUS_SENDREQUEST_COMPLETE:
        {
            OutputDebugString(_T("请求发送成功\r\n"));

            if(WinHttpReceiveResponse(m_hRequest, NULL) == FALSE)
            {
                Close();
            }

            break;
        }

        case WINHTTP_CALLBACK_STATUS_HEADERS_AVAILABLE:
        {
            OutputDebugString(_T("返回头有效\r\n"));
            // 查询文件长度
            DWORD dwFileLen = 0;
            DWORD dwbufLen = sizeof(DWORD);

            if(WinHttpQueryHeaders(m_hRequest, WINHTTP_QUERY_CONTENT_LENGTH | WINHTTP_QUERY_FLAG_NUMBER,
                                   WINHTTP_HEADER_NAME_BY_INDEX, &dwFileLen, &dwbufLen, WINHTTP_NO_HEADER_INDEX))
            {
                OnDataArrived(REQUEST_HEADERS_AVAILABLE, NULL, dwFileLen);
            }

            // 查询返回状态码
            DWORD statusCodeSize = sizeof(DWORD);

            if(!::WinHttpQueryHeaders(m_hRequest, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
                                      WINHTTP_HEADER_NAME_BY_INDEX, &m_dwStatusCode, &statusCodeSize, WINHTTP_NO_HEADER_INDEX))
            {
                Close();
            }

            if(!WinHttpQueryDataAvailable(m_hRequest, NULL))
            {
                Close();
            }

            break;
        }

        case WINHTTP_CALLBACK_STATUS_DATA_AVAILABLE:
        {
            OutputDebugString(_T("返回数据有效\r\n"));
            DWORD dwDataSize = *((LPDWORD)lpvStatusInformation);

            if(0 == dwDataSize)
            {
                OnDataArrived(REQUEST_READCOMPLATE, NULL, 0);
                Close();
            }
            else
            {
                BYTE* pBuf = new BYTE[dwDataSize + 1];
                ZeroMemory(pBuf, dwDataSize + 1);

                if(!ReadData(pBuf, dwDataSize))
                {
                    Close();
                }

                delete[] pBuf;
                pBuf = NULL;
            }

            break;
        }

        case WINHTTP_CALLBACK_STATUS_READ_COMPLETE:
        {
            OutputDebugString(_T("数据读取完成\r\n"));
            OnDataArrived(REQUEST_READING, lpvStatusInformation, dwStatusInformationLength);

            if(m_bUserCancel)
            {
                OnDataArrived(REQUEST_USERCANCEL, NULL, 0);
                Close();
                break;
            }

            if(!WinHttpQueryDataAvailable(m_hRequest, NULL))
            {
                Close();
            }

            break;
        }

        case WINHTTP_CALLBACK_STATUS_REDIRECT:
        {
            OutputDebugString(_T("重定向\r\n"));
            break;
        }

        case WINHTTP_CALLBACK_STATUS_WRITE_COMPLETE:
        {
            break;
        }

        case WINHTTP_CALLBACK_STATUS_REQUEST_ERROR:
        {
            OutputDebugString(_T("请求发生错误\r\n"));
            OnDataArrived(REQUEST_READERROR, NULL, 0);
            Close();
            break;
        }

        default:
            break;
    }
}

BOOL CWinHttpRequest::GetUrlComponent(CString& sHost, INTERNET_PORT& nPort, CString& RelativePath, INTERNET_SCHEME& nScheme)
{
    CString sUrl = m_pHttpHeader->GetUrl();

    URL_COMPONENTS   uc ;
    ZeroMemory(&uc, sizeof(uc)) ;
    DWORD   nBufferSize = sUrl.GetLength() + 8 ;
    std::vector<TCHAR>   t1(nBufferSize, (TCHAR)0) ;
    std::vector<TCHAR>   t2(nBufferSize, (TCHAR)0) ;
    uc.dwStructSize = sizeof(uc) ;
    uc.lpszHostName = &t1[0] ;
    uc.dwHostNameLength = nBufferSize ;
    uc.lpszUrlPath = &t2[0] ;
    uc.dwUrlPathLength = nBufferSize ;

    if(!WinHttpCrackUrl(sUrl, sUrl.GetLength(), 0, &uc))
        return FALSE;

    sHost = &t1[0] ;
    RelativePath = &t2[0] ;
    nScheme = uc.nScheme ;
    nPort = uc.nPort;

    return TRUE;
}

void CWinHttpRequest::CancelRequest()
{
    m_bUserCancel = TRUE;
}
