#pragma once

class CWinHttpHeader
{
public:
    CWinHttpHeader(void);
    virtual ~CWinHttpHeader(void);

    void AddHeader(LPCTSTR lpszHeader);
    void SetUrl(LPCTSTR lpszUrl);
    void SetUserAgent(LPCTSTR lpszUserAgent);
    CString GetHeader()const;
    CString GetUrl()const;
    CString GetUserAgent()const;
private:
    CString m_sHeader;
    CString m_sUrl;
    CString m_sUserAgent;
};

