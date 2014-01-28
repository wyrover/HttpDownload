#pragma once

class CWinHttpHeader
{
public:
    CWinHttpHeader(LPCTSTR lpszUrl);
    virtual ~CWinHttpHeader(void);

    void AddHeader(LPCTSTR lpszHeader);
    void SetUserAgent(LPCTSTR lpszUserAgent);
    CString GetHeader()const;
    CString GetUrl()const;
    CString GetUserAgent()const; 
protected:
	void SetUrl(LPCTSTR lpszUrl);
private:
    CString m_sHeader;
    CString m_sUrl;
    CString m_sUserAgent;
};

