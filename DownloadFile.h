#pragma once
#include "winhttprequest.h"

class CDownloadFile : public CWinHttpRequest
{
public:
    typedef void (__stdcall *DownloadCallback)(REQUEST_STATUS status, LPVOID lpCurBuf, DWORD dwCurSize, LPVOID lpParam);
public:
    CDownloadFile(void);
    virtual ~CDownloadFile(void);

    /**
     * 设置回调函数，显示进度时使用
     * @param callBack  用户设置的回调函数，数据到来时会自动调用
     * @param lpParam 用户指定的回调函数的参数
     */
    void SetCallback(DownloadCallback callBack, LPVOID lpParam);

    /**
     * 下载文件
     * @param  lpszUrl 下载地址
     * @param  lpszSavePath 保存的路径
     * @return 成功返回TRUE，失败返回FALSE
     */
    BOOL DownloadFile(LPCTSTR lpszUrl, LPCTSTR lpszSavePath);

    /**
     * 获取文件总大小
     * @return 返回服务器上文件的总大小
     */
    DWORD GetTotalSize()const;

    /**
     * 获取已经下载的字节数
     * @return 已经下载的字节数
     */
    DWORD GetDownloadedSize()const;
protected:
    /**
     * 获取已经下载的文件字节数,用来断点续传
     * @param  dwContinueIndex
     * @return 有断点续传文件返回TRUE,失败返回FALSE
     */
    BOOL GetContinueLen(DWORD& dwContinueIndex);

    /**
     * 关闭HTTP连接,关闭文件句柄
     */
    void Close();

    /**
     * 返回数据时回调函数
     * @param status 请求状态码,下载中,下载错误,下载完成
     * @param lpCurBuf 当前下载到的数据
     * @param dwCurSize 当前下载到的数据长度
     */
    virtual void OnDataArrived(REQUEST_STATUS status, LPVOID lpCurBuf, DWORD dwCurSize);
private:
    HANDLE m_hFile;             // 文件句柄
    DWORD m_dwTotalFileSize;    // 文件总大小
    DWORD m_dwDownloaded;       // 已经下载的字节数
    CString m_sFilePath;        // 下载文件保存路径
    DownloadCallback m_callBack;    // 回调函数
    LPVOID m_callBackParam;     // 下载回调参数
};

