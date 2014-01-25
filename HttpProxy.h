#pragma once
class CWinHttpProxy
{
public:
	CWinHttpProxy(void);
	virtual ~CWinHttpProxy(void);

	void SetIP(LPCTSTR lpszIP);
	void SetPort(int iPort);
	void SetUserName(LPCTSTR lpszUrserName);
	void SetPassword(LPCTSTR lpszPassword);

	CString GetIP()const;
	int GetPort()const;
	CString GetUserName()const;
	CString GetPassword()const;
private:
	CString m_sIP;
	int m_iPort;
	CString m_sUserName;
	CString m_sPassword;
};

