#include "stdafx.h"
#include "HttpProxy.h"


CWinHttpProxy::CWinHttpProxy(void):m_iPort(0)
{
}


CWinHttpProxy::~CWinHttpProxy(void)
{
}

void CWinHttpProxy::SetIP( LPCTSTR lpszIP )
{
	m_sIP=lpszIP;
}

void CWinHttpProxy::SetPort( int iPort )
{
	m_iPort=iPort;
}

void CWinHttpProxy::SetUserName( LPCTSTR lpszUrserName )
{
	m_sUserName=lpszUrserName;
}

void CWinHttpProxy::SetPassword( LPCTSTR lpszPassword )
{
	m_sPassword=lpszPassword;
}

CString CWinHttpProxy::GetIP() const
{
	return m_sIP;
}

int CWinHttpProxy::GetPort() const
{
	return m_iPort;
}

CString CWinHttpProxy::GetUserName() const
{
	return m_sUserName;
}

CString CWinHttpProxy::GetPassword() const
{
	return m_sPassword;
}
