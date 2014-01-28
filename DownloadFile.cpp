#include "stdafx.h"
#include "DownloadFile.h"
#include <assert.h>


CDownloadFile::CDownloadFile(void): CWinHttpRequest(VERB_TYPE_GET),
    m_hFile(INVALID_HANDLE_VALUE), m_dwTotalFileSize(0), m_dwDownloaded(0)
{
}


CDownloadFile::~CDownloadFile(void)
{
    Close();
}

void CDownloadFile::OnDataArrived(REQUEST_STATUS status, LPVOID lpCurBuf, DWORD dwCurSize)
{
    if(REQUEST_READCOMPLATE == status || REQUEST_READERROR == status || REQUEST_USERCANCEL == status)
    {
        Close();
    }
    else if(REQUEST_READING == status)
    {
        DWORD dwWrited = 0;

        if(!WriteFile(m_hFile, lpCurBuf, dwCurSize, &dwWrited, NULL))
        {
            if(!ERROR_IO_PENDING == GetLastError())
            {
                return ;
            }
        }

        m_dwDownloaded += dwWrited;
    }
    else if(REQUEST_HEADERS_AVAILABLE == status)
    {
        m_dwDownloaded = 0;
        m_dwTotalFileSize = dwCurSize;
        assert(INVALID_HANDLE_VALUE == m_hFile);

        DWORD dwContinueIndex = 0;

        if(GetContinueLen(dwContinueIndex))
        {
            m_hFile =::CreateFile(m_sFilePath, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
            ::SetFilePointer(m_hFile, 0, NULL, FILE_END);
            m_dwDownloaded = dwContinueIndex;
        }
        else
            m_hFile =::CreateFile(m_sFilePath, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    }

    if(m_callBack)
    {
        m_callBack(status, lpCurBuf, dwCurSize, m_callBackParam);
    }
}

BOOL CDownloadFile::DownloadFile(LPCTSTR lpszUrl, LPCTSTR lpszSavePath)
{
    m_sFilePath = lpszSavePath;

    CWinHttpHeader httpHeader(lpszUrl);

    DWORD dwContinueIndex = 0;

    if(GetContinueLen(dwContinueIndex))
    {
        CString sRange;
        sRange.Format(_T("Range: bytes=%d-"), dwContinueIndex);
        httpHeader.AddHeader(sRange);
    }

    if(Create(&httpHeader))
    {
        if(SendRequest())
        {
            return TRUE;
        }
    }

    return FALSE;
}

void CDownloadFile::SetCallback(DownloadCallback callBack, LPVOID lpParam)
{
    m_callBack = callBack;
    m_callBackParam = lpParam;
}

DWORD CDownloadFile::GetTotalSize() const
{
    return m_dwTotalFileSize;
}

DWORD CDownloadFile::GetDownloadedSize() const
{
    return m_dwDownloaded;
}

void CDownloadFile::Close()
{
    if(INVALID_HANDLE_VALUE != m_hFile)
    {
        CloseHandle(m_hFile);
        m_hFile = INVALID_HANDLE_VALUE;
    }

    __super::Close();
}

BOOL CDownloadFile::GetContinueLen(DWORD& dwContinueIndex)
{
    if(PathFileExists(m_sFilePath))
    {
        HANDLE hFile = INVALID_HANDLE_VALUE;
        DWORD dwFileSize = 0;
        hFile = CreateFile(m_sFilePath, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

        if(hFile == INVALID_HANDLE_VALUE)
            return 0;

        dwFileSize = GetFileSize(hFile, NULL);
        dwContinueIndex = dwFileSize;
        CloseHandle(hFile);
        hFile = INVALID_HANDLE_VALUE;
        return (0 != dwContinueIndex);
    }

    return FALSE;
}
