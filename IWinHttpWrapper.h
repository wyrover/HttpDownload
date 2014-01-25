#pragma once

class IWinHttpWrapper
{
public:
    virtual ~IWinHttpWrapper() = 0 {};

    virtual BOOL Download(LPCTSTR lpszUrl, LPCTSTR lpSaveFile, DWORD dwStartIndx, DWORD dwEndIndex) = 0;
//	virtual BOOL OnDataRecv(LPVOID lpBuf,DWORD dwSize)=0;
};