/**********************************************************************
 *
 *  Toby Opferman
 *
 *  Example Dynamic Loading a Driver
 *
 *  This example is for educational purposes only.  I license this source
 *  out for use in learning how to write a device driver.
 *
 *
 **********************************************************************/


#include <windows.h>
#include <stdio.h>
#include <iostream>
#include <string>

using namespace std;

int RunMiniFilter();
int RunDriver();

BOOL IsProcessElevated()
{
    BOOL fIsElevated = FALSE;
    HANDLE hToken = NULL;
    TOKEN_ELEVATION elevation;
    DWORD dwSize;

    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken))
    {
        printf("\n Failed to get Process Token :%d.", GetLastError());
        goto Cleanup;  // if Failed, we treat as False
    }


    if (!GetTokenInformation(hToken, TokenElevation, &elevation, sizeof(elevation), &dwSize))
    {
        printf("\nFailed to get Token Information :%d.", GetLastError());
        goto Cleanup;// if Failed, we treat as False
    }

    fIsElevated = elevation.TokenIsElevated;

Cleanup:
    if (hToken)
    {
        CloseHandle(hToken);
        hToken = NULL;
    }
    return fIsElevated;
}


int _cdecl main(void)
{
    if (IsProcessElevated() != TRUE)
    {
        cout << "Not elevated" << endl;
    }
    //RunDriver();
    RunMiniFilter();
}

int RunMiniFilter()
{
    const string name = "FsFilter";
    const string path = "C:\\Users\\hieu\\source\\repos\\Windows-Minifilter-Template\\driver\\x64\\Debug\\";
    const int runtime = 5;

    string s = "sc delete " + name;
    system(&s[0]);
    // return 0;

    cout << "Delete OKE \n";

    string s1 = "rundll32 syssetup,SetupInfObjectInstallAction DefaultInstall 128 " + path + name + ".inf";
    system(&s1[0]);

    cout << "Press enter to continue " << endl;
    getchar();

    string s2 = "net start " + name;
    system(&s2[0]);

    cout << "Service stop in:" << endl;
    for (int i = runtime; i > 0; i--)
    {
        cout << i << endl;
        Sleep(1000);
    }

    string s3 = "fltmc unload " + name;
    system(&s3[0]);

    string s4 = "net stop " + name;
    system(&s4[0]);

    string s5 = "sc stop " + name;
    system(&s5[0]);

    cout << "Service stopped" << endl;
    getchar();
    return 0;

}

int RunDriver()
{

    const string name = "MiniFs";
    const string path = "C:\\Users\\hieu\\source\\repos\\" + string("Windows-Minifilter-Template") + "\\x64\\Release\\";

    string s = "sc delete " + name;
    system(&s[0]);

    string s1 = "sc create " + name + " binPath= " + path + name + ".sys type= kernel";
    system(&s1[0]);

    cout << "Press enter to continue " << endl;
    getchar();

    string s2 = "sc start " + name;
    system(&s2[0]);

    cout << "Service stop in:" << endl;
    for (int i = 5; i > 0; i--)
    {
        cout << i << endl;
        Sleep(1000);
    }

    string s3 = "sc stop " + name;
    system(&s3[0]);

    system(&s[0]);
    getchar();
    return 0;
}
