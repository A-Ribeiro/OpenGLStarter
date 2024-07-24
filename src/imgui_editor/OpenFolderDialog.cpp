#include "OpenFolderDialog.h"

#include <InteractiveToolkit/Platform/Process.h>

#include <appkit-gl-engine/Engine.h>

#if defined(_WIN32)

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <Commdlg.h>
#include <ShlObj.h>

#else

#include <regex> // std::regex

#endif

namespace Native
{

#if defined(_WIN32)

    class Win32OpenFile
    {
    public:
        char szFile[MAX_PATH];

        bool open(const std::string &starting_path, std::string *output)
        {
            sprintf(szFile, "%s", starting_path.c_str());

            OPENFILENAME ofn;

            // open a file name
            ZeroMemory(&ofn, sizeof(ofn));
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = nullptr;
            ofn.lpstrFile = szFile;
            // ofn.lpstrFile[0] = '\0';
            ofn.nMaxFile = sizeof(szFile);
            // ofn.lpstrFilter = "All\0*.*\0Text\0*.TXT\0";
            // ofn.nFilterIndex = 1;
            ofn.lpstrFileTitle = nullptr;
            ofn.nMaxFileTitle = 0;
            ofn.lpstrInitialDir = nullptr;
            // ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
            ofn.Flags = OFN_PATHMUSTEXIST; // | OFN_FILEMUSTEXIST;

            if (GetOpenFileName(&ofn))
            {
                *output = ofn.lpstrFile;
                return true;
            }

            // Now simpley display the file name
            // MessageBox(nullptr, ofn.lpstrFile, "File Name", MB_OK);
            return false;
        }
    };

    class Win32OpenFolder
    {
        // WCHAR szFileW[MAX_PATH];
        bool winVistaMethod(const std::string &starting_path, std::string *output)
        {
            bool result = false;
            IFileDialog *fileDialog;
            if (SUCCEEDED(CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&fileDialog))))
            {
                DWORD dwOptions;
                if (SUCCEEDED(fileDialog->GetOptions(&dwOptions)))
                {
                    fileDialog->SetOptions(dwOptions | FOS_PICKFOLDERS);

                    IShellItem *shellItemSelectFolder;

                    std::wstring wstr;

                    std::u32string _u32_str = ITKCommon::StringUtil::utf8_to_utf32(starting_path);
                    if (sizeof(wchar_t) == 2) {
                        std::u16string _u16_str = ITKCommon::StringUtil::utf32_to_utf16(_u32_str);
                        wstr = (wchar_t*)_u16_str.c_str();
                    }
                    else {
                        wstr = (wchar_t*)_u32_str.c_str();
                    }

                    HRESULT hr = SHCreateItemFromParsingName(wstr.c_str(), nullptr, IID_PPV_ARGS(&shellItemSelectFolder));
                    // HRESULT hr = SHGetKnownFolderItem(FOLDERID_Windows, KF_FLAG_DEFAULT, nullptr, IID_IShellItem, (void**)&psiwindir);
                    if (hr == S_OK)
                    {
                        hr = fileDialog->SetFolder(shellItemSelectFolder); // MSDN says it is OK to change the folder in OnFolderChanging with SetFolder
                        shellItemSelectFolder->Release();
                    }
                }
                if (SUCCEEDED(fileDialog->Show((HWND)AppKit::GLEngine::Engine::Instance()->window->getNativeWindowHandle())))
                {
                    IShellItem *shellItem;
                    if (SUCCEEDED(fileDialog->GetResult(&shellItem)))
                    {
                        WCHAR *tmp = nullptr;
                        if (!SUCCEEDED(shellItem->GetDisplayName(SIGDN_DESKTOPABSOLUTEPARSING, &tmp)))
                        {
                            MessageBox(nullptr, "GetIDListName() failed", nullptr, NULL);
                        }
                        if (tmp != nullptr)
                        {
                            if (sizeof(wchar_t) == 2) {
                                std::u32string _u32_str = ITKCommon::StringUtil::utf16_to_utf32((char16_t*)tmp);
                                *output = ITKCommon::StringUtil::utf32_to_utf8(_u32_str);
                            }
                            else {
                                *output = ITKCommon::StringUtil::utf32_to_utf8((char32_t*)tmp);
                            }
                            //*output = ITKCommon::StringUtil::toString(tmp);
                            result = true;
                        }

                        shellItem->Release();
                    }
                }
                fileDialog->Release();
            }

            return result;
        }

        bool win2000Method(const std::string &starting_path, std::string *output)
        {
            sprintf(szFile, "%s", starting_path.c_str());

            BROWSEINFO bi;
            ZeroMemory(&bi, sizeof(bi));

            bi.hwndOwner = (HWND)AppKit::GLEngine::Engine::Instance()->window->getNativeWindowHandle(); // nullptr;// Process.GetCurrentProcess().MainWindowHandle; ;
            bi.pidlRoot = nullptr;
            bi.pszDisplayName = szFile;
            bi.lpszTitle = "Choose a Folder";
            bi.ulFlags = 0; // BIF_NEWDIALOGSTYLE | BIF_SHAREABLE;
            bi.lpfn = nullptr; // new BrowseCallBackProc(OnBrowseEvent);
            bi.lParam = NULL;
            bi.iImage = 0;

            PIDLIST_ABSOLUTE pidl = SHBrowseForFolder(&bi);
            if (pidl != nullptr)
            {
                if (SHGetPathFromIDList(pidl, szFile))
                {
                    CoTaskMemFree(pidl);
                    *output = szFile;
                    return true;
                }
                else
                    CoTaskMemFree(pidl);
            }

            return false;
        }

    public:
        char szFile[MAX_PATH];
        bool open(const std::string &starting_path, std::string *output)
        {
            return winVistaMethod(starting_path, output);
        }
    };

#elif defined(__APPLE__)

    static std::string osascript_quote(std::string const &str)
    {
        return "\"" + std::regex_replace(str, std::regex("[\\\\\"]"), "\\$&") + "\"";
    }

    class OSAppleScript
    {

    public:
        OSAppleScript()
        {
        }

        bool open(const std::string &starting_path, std::string *output)
        {

            AppKit::GLEngine::Engine::Instance()->window->setMouseCursorVisible(true);
            // command line

            std::string parameter = "-e ";
            parameter += "set ret to choose folder";
            if (starting_path.size() > 0)
                parameter += " default location " + osascript_quote(starting_path);
            parameter += " with prompt " + osascript_quote("Choose a Folder");
            parameter += "\nPOSIX path of ret";

            parameter = osascript_quote(parameter);

            printf("[OpenFolderDialog]%s %s\n", "osascript", parameter.c_str());

            aRibeiro::UnixPipe child_stdout;
            child_stdout.setReadBlocking(false);

            aRibeiro::PlatformProcess process("osascript",
                                              ITKCommon::StringUtil::parseArgv(parameter),
                                              INT32_MAX,
                                              nullptr, &child_stdout, nullptr);

            std::vector<uint8_t> raw_output;
            aRibeiro::ObjectBuffer buffer;
            while (!child_stdout.isReadSignaled())
            {
                if (child_stdout.read(&buffer))
                    raw_output.insert(raw_output.end(), &buffer.data[0], &buffer.data[buffer.size]);

                //::printf(".");::fflush(stdout);

                // process window events - avoid window not responding error...
                AppKit::GLEngine::Engine::Instance()->window->forwardWindowEventsToInputManager(true);

                aRibeiro::PlatformSleep::sleepMillis(100);
            }
            //printf("\n");
            raw_output.push_back(0);
            std::string path = ITKCommon::StringUtil::trim((char *)raw_output.data());
            printf("result: '%s'\n", path.c_str());

            int exit_code = -1;
            process.waitExit(&exit_code, UINT_MAX);
            printf("EXIT CODE: %i\n", exit_code);

            AppKit::GLEngine::Engine::Instance()->window->setMouseCursorVisible(false);

            if (exit_code == 0 && path.size() > 0)
            {
                *output = path;
                return true;
            }

            return false;
        }
    };
#elif defined(__linux__)

    enum LinuxDialogType
    {

        LinuxDialogZenity = 0,
        LinuxDialogMatedialog,
        LinuxDialogQarma,
        LinuxDialogKDialog,

        LinuxDialogCount
    };

    class LinuxExternalDialogs
    {
        bool available[LinuxDialogCount];

    public:
        LinuxExternalDialogs()
        {
            available[LinuxDialogZenity] = Platform::Process::ApplicationExists("zenity");
            available[LinuxDialogMatedialog] = Platform::Process::ApplicationExists("matedialog");
            available[LinuxDialogQarma] = Platform::Process::ApplicationExists("qarma");
            available[LinuxDialogKDialog] = Platform::Process::ApplicationExists("kdialog");

            printf("[LinuxExternalDialogs] LinuxDialogZenity %i\n", (int)available[LinuxDialogZenity]);
            printf("[LinuxExternalDialogs] LinuxDialogMatedialog %i\n", (int)available[LinuxDialogMatedialog]);
            printf("[LinuxExternalDialogs] LinuxDialogQarma %i\n", (int)available[LinuxDialogQarma]);
            printf("[LinuxExternalDialogs] LinuxDialogKDialog %i\n", (int)available[LinuxDialogKDialog]);

            if (available[LinuxDialogZenity] && available[LinuxDialogKDialog])
            {
                char *buf = std::getenv("XDG_SESSION_DESKTOP");
                std::string XDG_SESSION_DESKTOP = (buf != nullptr) ? buf : "";
                if (XDG_SESSION_DESKTOP == std::string("gnome"))
                    available[LinuxDialogKDialog] = false;
                else if (XDG_SESSION_DESKTOP == std::string("KDE"))
                    available[LinuxDialogZenity] = false;
            }
        }

        std::string getAvailableCommand()
        {
            if (available[LinuxDialogZenity])
                return "zenity";
            if (available[LinuxDialogMatedialog])
                return "matedialog";
            if (available[LinuxDialogQarma])
                return "qarma";
            if (available[LinuxDialogKDialog])
                return "kdialog";
            return "echo";
        }

        bool is_gnome_family_dialog() const
        {
            return available[LinuxDialogZenity] || available[LinuxDialogMatedialog] || available[LinuxDialogQarma];
        }

        bool is_kdialog() const
        {
            return available[LinuxDialogKDialog];
        }

        bool open(const std::string &starting_path, std::string *output)
        {
            printf("[LinuxExternalDialogs] starting_path: %s\n", starting_path.c_str());
            std::string path;
            int exit_code = -1;

            std::string command;
            std::string parameters;

            if (is_kdialog())
            {
                command = getAvailableCommand();
                parameters = "--getexistingdirectory " + ITKCommon::StringUtil::quote_cmd(starting_path);
                parameters += std::string(" --title ") + ITKCommon::StringUtil::quote_cmd("Choose a Folder");
            }
            else if (is_gnome_family_dialog())
            {
                command = getAvailableCommand();
                parameters = "--file-selection";
                parameters += std::string(" --filename=") + ITKCommon::StringUtil::quote_cmd(starting_path + ITKCommon::PATH_SEPARATOR);
                parameters += std::string(" --title ") + ITKCommon::StringUtil::quote_cmd("Choose a Folder");
                parameters += std::string(" --directory");
            }

            if (command.size() > 0)
            {
                AppKit::GLEngine::Engine::Instance()->window->setMouseCursorVisible(true);

                printf("[OpenFolderDialog]%s %s\n", command.c_str(), parameters.c_str());

                Platform::UnixPipe child_stdout;
                child_stdout.setReadBlocking(false);

                Platform::Process process(command,
                                                  ITKCommon::StringUtil::parseArgv(parameters),
                                                  INT32_MAX,
                                                  nullptr, &child_stdout, nullptr);

                std::vector<uint8_t> raw_output;
                Platform::ObjectBuffer buffer;
                while (!child_stdout.isReadSignaled())
                {
                    if (child_stdout.read(&buffer))
                        raw_output.insert(raw_output.end(), &buffer.data[0], &buffer.data[buffer.size]);
                    
                    //printf(".");
                    //fflush(stdout);

                    // process window events - avoid window not responding error...
                    AppKit::GLEngine::Engine::Instance()->window->forwardWindowEventsToInputManager(true);

                    Platform::Sleep::millis(100);
                }
                //printf("\n");
                raw_output.push_back(0);
                path = ITKCommon::StringUtil::trim((char *)raw_output.data());
                process.waitExit(&exit_code, UINT_MAX);

                AppKit::GLEngine::Engine::Instance()->window->setMouseCursorVisible(false);
            }

            printf("result: '%s'\n", path.c_str());
            printf("EXIT CODE: %i\n", exit_code);

            if (exit_code == 0 && path.size() > 0)
            {
                *output = path;
                return true;
            }

            return false;
        }
    };

#endif

    bool OpenFolderDialog(const std::string &starting_path, std::string *output)
    {
#if defined(_WIN32)
        // convert path separator
        // std::string begin = starting_path;
        // std::replace(begin.begin(), begin.end(), '/', '\\');
        // char fullFilename[MAX_PATH];
        // GetFullPathName(starting_path.c_str(), MAX_PATH, fullFilename, nullptr);

        std::string fullFilename = ITKCommon::Path::getAbsolutePath(starting_path);

        Win32OpenFolder win32OpenFolder;
        if (win32OpenFolder.open(fullFilename, output))
        {
            return true;
        }
        // Now simpley display the file name
        // MessageBox(nullptr, ofn.lpstrFile, "File Name", MB_OK);
#elif defined(__APPLE__)

        std::string fullFilename = ITKCommon::Path::getAbsolutePath(starting_path);
        printf("fullFilename: %s\n", fullFilename.c_str());
        OSAppleScript osAppleScript;
        return osAppleScript.open(fullFilename, output);

#elif defined(__linux__)

        std::string fullFilename = ITKCommon::Path::getAbsolutePath(starting_path);
        printf("fullFilename: %s\n", fullFilename.c_str());
        LinuxExternalDialogs linuxExternalDialogs;
        return linuxExternalDialogs.open(fullFilename, output);

#endif

        return false;
    }

}