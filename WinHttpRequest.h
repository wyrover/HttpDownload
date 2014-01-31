#pragma once
#include "HttpHeader.h"
#include "HttpProxy.h"
#include <winhttp.h>

enum VERB_TYPE
{
    VERB_TYPE_GET = 0,
    VERB_TYPE_POST = 1,
    VERB_TYPE_POST_MULTIPART = 2,
    VERB_TYPE_DELETE = 3,
};

enum REQUEST_STATUS
{
    REQUEST_HEADERS_AVAILABLE,
    REQUEST_READING,
    REQUEST_READCOMPLATE,
    REQUEST_USERCANCEL,
    REQUEST_READERROR
};

class CWinHttpRequest
{

public:
    CWinHttpRequest(VERB_TYPE verbType = VERB_TYPE_GET);
    virtual ~CWinHttpRequest(void);

    void SetProxy(ProxyResolver* pHttpProxy);
    BOOL Create(CWinHttpHeader* pHttpHeader);
    BOOL SendRequest(LPVOID lpOptional = NULL, DWORD dwOptionalLength = NULL);
    void Close();
    void CancelRequest();
    DWORD GetStatusCode()const;
protected:
    virtual void OnDataArrived(REQUEST_STATUS status, LPVOID lpCurBuf, DWORD dwCurSize) {};
protected:
    BOOL GetUrlComponent(CString& sHost, INTERNET_PORT& nPort, CString& RelativePath, INTERNET_SCHEME& nScheme);
    CString GetVerbText() const;
    static void __stdcall WinHttpStatusCallback(HINTERNET hInternet, DWORD_PTR dwContext, DWORD dwInternetStatus,
            LPVOID lpvStatusInformation, DWORD dwStatusInformationLength);
    void _WinHttpStatusCallback(DWORD dwInternetStatus, LPVOID lpvStatusInformation, DWORD dwStatusInformationLength);
    BOOL ReadData(LPVOID lpOutBuffer, DWORD dwBufSize);
private:
    HINTERNET m_hSession;
    HINTERNET m_hConnect;
    HINTERNET m_hRequest;
    CWinHttpHeader* m_pHttpHeader;
    ProxyResolver* m_pHttpProxy;
    VERB_TYPE m_verbType;
    BOOL m_bUserCancel;     // 在接收数据过程中,用户取消HTTP请求
    DWORD m_dwStatusCode;   // HTTP请求状态码
};

