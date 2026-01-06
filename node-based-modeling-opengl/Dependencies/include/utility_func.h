#ifndef UTILITY_FUNC_H
#define UTILITY_FUNC_H

#include <windows.h>
#include <commdlg.h>
#include <string>
#include <string.h>
#include <shlobj.h>


#include <windows.h>
#include <memory>
#include <string>
#include <locale>
#include <codecvt>

class Utility
{
public:

    Utility() {}

    std::filesystem::path OpenFileDialog() 
    {
        OPENFILENAME ofn; // 

        TCHAR lpstrFile[260] = L"";
        static TCHAR filter[] = L"All Files\0*.*\0\0";

        memset(&ofn, 0, sizeof(OPENFILENAME));
        ofn.lStructSize = sizeof(OPENFILENAME);
        ofn.hwndOwner = NULL;

        ofn.lpstrFilter = filter;
        ofn.lpstrFile = lpstrFile;
        ofn.nMaxFile = 260;
        ofn.lpstrInitialDir = L".";

        if (GetOpenFileName(&ofn) != 0)
        {
            return std::filesystem::path(ofn.lpstrFile);
        }

        return {};
    }

    std::string SelectFolder()
    {
        BROWSEINFO bi = {};
        bi.lpszTitle = L"Select Folder";
        LPITEMIDLIST pidl = SHBrowseForFolder(&bi);
        if (!pidl) return "";

        char path[MAX_PATH];
        if (SHGetPathFromIDListA(pidl, path))
        {
            CoTaskMemFree(pidl);
            return std::string(path);
        }
        CoTaskMemFree(pidl);
        return "";
    }


private:
    /*
    std::string to_string(const std::wstring& wstr)
    {
        if (wstr.empty()) return std::string();

        int size_needed = WideCharToMultiByte(
            CP_UTF8, 0, &wstr[0], (int)wstr.size(),
            NULL, 0, NULL, NULL);

        std::string str(size_needed, 0);

        WideCharToMultiByte(
            CP_UTF8, 0, &wstr[0], (int)wstr.size(),
            &str[0], size_needed, NULL, NULL);

        return str;
    }

    */




};









#endif