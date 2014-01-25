#pragma once
#include "iwinhttpwrapper.h"
#include <winhttp.h>
#pragma comment(lib,"Winhttp.lib")

class CWinHttpBase : public IWinHttpWrapper
{
public:
    CWinHttpBase(void);
    virtual ~CWinHttpBase(void);

    BOOL Download(LPCTSTR lpszUrl, LPCTSTR lpSaveFile, DWORD dwStartIndx, DWORD dwEndIndex);

protected:
    BOOL CrackUrl(LPCTSTR lpszUrl, URL_COMPONENTS& urlComp);
    BOOL OpenSession();
    BOOL OpenConnect(LPCWSTR pswzServerName, INTERNET_PORT nServerPort);
    BOOL OpenRequest(LPCTSTR lpVerb, LPCTSTR lpRelativeUrl, DWORD dwFlags);
    BOOL SetWinHttpCallback(WINHTTP_STATUS_CALLBACK statusCallback);
    BOOL SendRequest(LPCWSTR pwszHeaders, DWORD dwHeadersLength, LPVOID lpOptional, DWORD dwOptionalLength, DWORD dwTotalLength, DWORD_PTR dwContext);
    static void __stdcall WinHttpStatusCallback(HINTERNET hInternet, DWORD_PTR dwContext, DWORD dwInternetStatus, LPVOID lpvStatusInformation, DWORD dwStatusInformationLength);
    void _WinHttpStatusCallback(DWORD dwInternetStatus, LPVOID lpvStatusInformation, DWORD dwStatusInformationLength);
    BOOL QueryData();
    BOOL ReadData(LPVOID lpBuf, DWORD dwBufSize);
    void Cleanup();
private:
    HINTERNET m_hSession;
    HINTERNET m_hConnect;
    HINTERNET m_hRequest;

// 
// public:
// 	BOOL Create();
};

