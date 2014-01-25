#include "stdafx.h"
#include "HttpHeader.h"

CWinHttpHeader::CWinHttpHeader(void)
{
}


CWinHttpHeader::~CWinHttpHeader(void)
{
	m_sHeader.Empty();
	m_sUrl.Empty();
	m_sUserAgent.Empty();
}

void CWinHttpHeader::AddHeader(LPCTSTR lpszHeader)
{
    m_sHeader.AppendFormat(_T("%s"), lpszHeader);
}

void CWinHttpHeader::SetUrl(LPCTSTR lpszUrl)
{
    m_sUrl = lpszUrl;
}

CString CWinHttpHeader::GetHeader() const
{
    return m_sHeader;
}

CString CWinHttpHeader::GetUrl() const
{
    return m_sUrl;
}

void CWinHttpHeader::SetUserAgent(LPCTSTR lpszUserAgent)
{
    m_sUserAgent = lpszUserAgent;
}

CString CWinHttpHeader::GetUserAgent() const
{
    return m_sUserAgent;
}
