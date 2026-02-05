#include <windows.h>
#include <windowsx.h>
#include "../Common/FileChooser.h"
#include "../Common/Silkroad.h"
#include "../Common/ConfigFile.h"
#include "../Common/GetCommonDirectory.h"
#include "../Common/common.h"
#include <string>
#include <sstream>
#include <map>
#include <algorithm>
#include "resource.h"

//-------------------------------------------------------------------------

INT_PTR CALLBACK DlgProc_Loader(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

//-------------------------------------------------------------------------

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	HWND hWnd = 0;
	MSG msg = {0};

	hWnd = FindWindow("#32770", "edxSilkroadLoader5");
	if(hWnd)
	{
		ShowWindow(hWnd, SW_RESTORE);
		SetForegroundWindow(hWnd);
		return 0;
	}

	hWnd = CreateDialogParam(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DlgProc_Loader, (LPARAM)hInstance);
	while(GetMessage(&msg, NULL, 0, 0))
	{
		if(IsDialogMessage(hWnd, &msg) == 0)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	DestroyWindow(hWnd);

	return 0;
}

//-------------------------------------------------------------------------

// Tokenizes a string into a vector
static std::vector<std::string> TokenizeString(const std::string& str, const std::string& delim)
{
	// http://www.gamedev.net/community/forums/topic.asp?topic_id=381544#TokenizeString
	using namespace std;
	vector<string> tokens;
	size_t p0 = 0, p1 = string::npos;
	while(p0 != string::npos)
	{
		p1 = str.find_first_of(delim, p0);
		if(p1 != p0)
		{
			string token = str.substr(p0, p1 - p0);
			tokens.push_back(token);
		}
		p0 = str.find_first_not_of(delim, p1);
	}
	return tokens;
}

//-------------------------------------------------------------------------

INT_PTR CALLBACK DlgProc_Loader(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static HWND HWND_IDC_DIRS = 0;
	static HWND HWND_IDC_ADD = 0;
	static HWND HWND_IDC_REMOVE = 0;
	static HWND HWND_IDC_OPEN = 0;
	static HWND HWND_IDC_DIVS = 0;
	static HWND HWND_IDC_HOSTS = 0;
	static HWND HWND_IDC_COUNTRY = 0;
	static HWND HWND_IDC_LANGUAGE = 0;
	static HWND HWND_IDC_LOCALE = 0;
	static HWND HWND_IDC_VERSION = 0;
	static HWND HWND_IDC_PORTS = 0;
	static HWND HWND_IDC_PATCH_ENGLISH = 0;
	static HWND HWND_IDC_PATCH_MULTI = 0;
	static HWND HWND_IDC_PATCH_SWEAR = 0;
	static HWND HWND_IDC_PATCH_NUDE = 0;
	static HWND HWND_IDC_PATCH_KOREA = 0;
	static HWND HWND_IDC_PATCH_REDIRECT_GWS = 0;
	static HWND HWND_IDC_PATCH_REDIRECT_AGS = 0;
	static HWND HWND_IDC_PATCH_CONSOLE = 0;
	static HWND HWND_IDC_PATCH_ZOOM = 0;
	static HWND HWND_IDC_PATCH_GWS_IP = 0;
	static HWND HWND_IDC_PATCH_AGS_IP = 0;
	static HWND HWND_IDC_PATCH_GWS_PORT = 0;
	static HWND HWND_IDC_PATCH_AGS_PORT = 0;
	static HWND HWND_IDC_PATCH_HS = 0;
	static HWND HWND_IDC_PATCH_INPUT = 0;
	static HWND HWND_IDC_LAUNCH = 0;
	static HWND HWND_IDC_REFRESH = 0;
	static HWND HWND_IDC_CONFIG = 0;
	static HWND HWND_IDC_SEED = 0;
	static HWND HWND_IDC_AUTO_PARSE = 0;

	static ConfigFile cf;
	static std::map<std::string, SilkroadData> paths;

	static std::string English_Patch = "no";
	static std::string Multiclient = "no";
	static std::string Debug_Console = "no";
	static std::string Swear_Filter = "no";
	static std::string Nude_Patch = "no";
	static std::string Zoom_Hack = "no";
	static std::string Korean_Captcha = "no";
	static std::string No_Hackshield = "no";
	static std::string Redirect_Gateway = "no";
	static std::string Redirect_Agent = "no";
	static std::string Gateway_Ip = "127.0.0.1";
	static std::string Gateway_Port = "19000";
	static std::string Agent_Ip = "127.0.0.1";
	static std::string Agent_Port = "19001";
	static std::string Hook_Input = "no";
	static std::string Patch_Seed = "no";
	static std::string Auto_Parse = "no";

	static std::string pathToDll;

	switch(uMsg)
	{
		case WM_USER + 5:
		{
			cf.DeleteSection("Directories");
			cf.Write("Directories", "count", "%i", paths.size());
			std::map<std::string, SilkroadData>::iterator itr2 = paths.begin();
			int ind = 0;
			while(itr2 != paths.end())
			{
				std::stringstream ss;
				ss << "directory_" << ind;
				cf.Write("Directories", ss.str(), "%s", itr2->first.c_str());
				++ind;
				++itr2;
			}
		} break;

		case WM_USER + 6:
		{
			cf.Write("Patches", "English_Patch", English_Patch.c_str());
			cf.Write("Patches", "Multiclient", Multiclient.c_str());
			cf.Write("Patches", "Debug_Console", Debug_Console.c_str());
			cf.Write("Patches", "Swear_Filter", Swear_Filter.c_str());
			cf.Write("Patches", "Nude_Patch", Nude_Patch.c_str());
			cf.Write("Patches", "Zoom_Hack", Zoom_Hack.c_str());
			cf.Write("Patches", "Korean_Captcha", Korean_Captcha.c_str());
			cf.Write("Patches", "No_Hackshield", No_Hackshield.c_str());
			cf.Write("Patches", "Redirect_Gateway", Redirect_Gateway.c_str());
			cf.Write("Patches", "Redirect_Agent", Redirect_Agent.c_str());
			cf.Write("Patches", "Gateway_Ip", Gateway_Ip.c_str());
			cf.Write("Patches", "Gateway_Port", Gateway_Port.c_str());
			cf.Write("Patches", "Agent_Ip", Agent_Ip.c_str());
			cf.Write("Patches", "Agent_Port", Agent_Port.c_str());
			cf.Write("Patches", "Hook_Input", Hook_Input.c_str());
			cf.Write("Patches", "Patch_Seed", Patch_Seed.c_str());
			cf.Write("Patches", "Auto_Parse", Auto_Parse.c_str());
		} break;

		case WM_INITDIALOG:
		{
			HWND_IDC_DIRS = GetDlgItem(hWnd, IDC_DIRS);
			HWND_IDC_ADD = GetDlgItem(hWnd, IDC_ADD);
			HWND_IDC_REMOVE = GetDlgItem(hWnd, IDC_REMOVE);
			HWND_IDC_OPEN = GetDlgItem(hWnd, IDC_OPEN);
			HWND_IDC_DIVS = GetDlgItem(hWnd, IDC_DIVS);
			HWND_IDC_HOSTS = GetDlgItem(hWnd, IDC_HOSTS);
			HWND_IDC_COUNTRY = GetDlgItem(hWnd, IDC_COUNTRY);
			HWND_IDC_LANGUAGE = GetDlgItem(hWnd, IDC_LANGUAGE);
			HWND_IDC_LOCALE = GetDlgItem(hWnd, IDC_LOCALE);
			HWND_IDC_VERSION = GetDlgItem(hWnd, IDC_VERSION);
			HWND_IDC_PORTS = GetDlgItem(hWnd, IDC_PORTS);
			HWND_IDC_PATCH_ENGLISH = GetDlgItem(hWnd, IDC_PATCH_ENGLISH);
			HWND_IDC_PATCH_MULTI = GetDlgItem(hWnd, IDC_PATCH_MULTI);
			HWND_IDC_PATCH_SWEAR = GetDlgItem(hWnd, IDC_PATCH_SWEAR);
			HWND_IDC_PATCH_NUDE = GetDlgItem(hWnd, IDC_PATCH_NUDE);
			HWND_IDC_PATCH_KOREA = GetDlgItem(hWnd, IDC_PATCH_KOREA);
			HWND_IDC_PATCH_REDIRECT_GWS = GetDlgItem(hWnd, IDC_PATCH_REDIRECT_GWS);
			HWND_IDC_PATCH_REDIRECT_AGS = GetDlgItem(hWnd, IDC_PATCH_REDIRECT_AGS);
			HWND_IDC_PATCH_CONSOLE = GetDlgItem(hWnd, IDC_PATCH_CONSOLE);
			HWND_IDC_PATCH_ZOOM = GetDlgItem(hWnd, IDC_PATCH_ZOOM);
			HWND_IDC_PATCH_GWS_IP = GetDlgItem(hWnd, IDC_PATCH_GWS_IP);
			HWND_IDC_PATCH_AGS_IP = GetDlgItem(hWnd, IDC_PATCH_AGS_IP);
			HWND_IDC_PATCH_GWS_PORT = GetDlgItem(hWnd, IDC_PATCH_GWS_PORT);
			HWND_IDC_PATCH_AGS_PORT = GetDlgItem(hWnd, IDC_PATCH_AGS_PORT);
			HWND_IDC_PATCH_HS = GetDlgItem(hWnd, IDC_PATCH_HS);
			HWND_IDC_LAUNCH = GetDlgItem(hWnd, IDC_LAUNCH);
			HWND_IDC_REFRESH = GetDlgItem(hWnd, IDC_REFRESH);
			HWND_IDC_CONFIG = GetDlgItem(hWnd, IDC_CONFIG);
			HWND_IDC_PATCH_INPUT = GetDlgItem(hWnd, IDC_PATCH_INPUT);
			HWND_IDC_SEED = GetDlgItem(hWnd, IDC_SEED);
			HWND_IDC_AUTO_PARSE = GetDlgItem(hWnd, IDC_AUTO_PARSE);

			char cd[MAX_PATH + 1] = {0};
			GetCurrentDirectoryA(MAX_PATH, cd);
			pathToDll = cd;
			pathToDll += "\\edxSilkroadDll5.dll";

			ShowWindow(hWnd, SW_NORMAL);
			UpdateWindow(hWnd);

			std::string path = GetCommonDirectory("edxSilkroadLoader5");
			path += "edxSilkroadLoader5.ini";

			bool exists = cf.Open(path.c_str());
			if(!exists)
			{
				SendMessage(hWnd, WM_USER + 5, 0, 0);
				SendMessage(hWnd, WM_USER + 6, 0, 0);
			}

			int count = cf.ReadType<int>("Directories", "count");
			if(count > 0)
			{
				for(int x = 0; x < count; ++x)
				{
					std::stringstream kss;
					kss << "directory_" << x;

					std::string sel_path = cf.Read("Directories", kss.str());

					std::map<std::string, SilkroadData>::iterator itr = paths.find(sel_path);
					if(itr != paths.end())
					{
						continue;
					}

					SilkroadData data;
					if(LoadPath(sel_path, data) == false)
					{
						continue;
					}

					paths.insert(std::make_pair(sel_path, data));

					ListBox_AddString(HWND_IDC_DIRS, sel_path.c_str());
				}
				ListBox_SetCurSel(HWND_IDC_DIRS, 0);
				PostMessage(hWnd, WM_COMMAND, MAKELONG(IDC_DIRS, LBN_SELCHANGE), (LPARAM)HWND_IDC_DIRS);
			}

			English_Patch = cf.Read("Patches", "English_Patch");
			Multiclient = cf.Read("Patches", "Multiclient");
			Debug_Console = cf.Read("Patches", "Debug_Console");
			Swear_Filter = cf.Read("Patches", "Swear_Filter");
			Nude_Patch = cf.Read("Patches", "Nude_Patch");
			Zoom_Hack = cf.Read("Patches", "Zoom_Hack");
			Korean_Captcha = cf.Read("Patches", "Korean_Captcha");
			No_Hackshield = cf.Read("Patches", "No_Hackshield");
			Redirect_Gateway = cf.Read("Patches", "Redirect_Gateway");
			Redirect_Agent = cf.Read("Patches", "Redirect_Agent");
			Hook_Input = cf.Read("Patches", "Hook_Input");
			Patch_Seed = cf.Read("Patches", "Patch_Seed");
			Auto_Parse = cf.Read("Patches", "Auto_Parse");
			Gateway_Ip = cf.Read("Patches", "Gateway_Ip");
			Gateway_Port = cf.Read("Patches", "Gateway_Port");
			Agent_Ip = cf.Read("Patches", "Agent_Ip");
			Agent_Port = cf.Read("Patches", "Agent_Port");

			Button_SetCheck(HWND_IDC_PATCH_ENGLISH, English_Patch == "yes" ? BST_CHECKED : BST_UNCHECKED);
			Button_SetCheck(HWND_IDC_PATCH_MULTI, Multiclient == "yes" ? BST_CHECKED : BST_UNCHECKED);
			Button_SetCheck(HWND_IDC_PATCH_CONSOLE, Debug_Console == "yes" ? BST_CHECKED : BST_UNCHECKED);
			Button_SetCheck(HWND_IDC_PATCH_SWEAR, Swear_Filter == "yes" ? BST_CHECKED : BST_UNCHECKED);
			Button_SetCheck(HWND_IDC_PATCH_NUDE, Nude_Patch == "yes" ? BST_CHECKED : BST_UNCHECKED);
			Button_SetCheck(HWND_IDC_PATCH_ZOOM, Zoom_Hack == "yes" ? BST_CHECKED : BST_UNCHECKED);
			Button_SetCheck(HWND_IDC_PATCH_KOREA, Korean_Captcha == "yes" ? BST_CHECKED : BST_UNCHECKED);
			Button_SetCheck(HWND_IDC_PATCH_HS, No_Hackshield == "yes" ? BST_CHECKED : BST_UNCHECKED);
			Button_SetCheck(HWND_IDC_PATCH_REDIRECT_GWS, Redirect_Gateway == "yes" ? BST_CHECKED : BST_UNCHECKED);
			Button_SetCheck(HWND_IDC_PATCH_REDIRECT_AGS, Redirect_Agent == "yes" ? BST_CHECKED : BST_UNCHECKED);
			Button_SetCheck(HWND_IDC_PATCH_INPUT, Hook_Input == "yes" ? BST_CHECKED : BST_UNCHECKED);
			Button_SetCheck(HWND_IDC_SEED, Patch_Seed == "yes" ? BST_CHECKED : BST_UNCHECKED);
			Button_SetCheck(HWND_IDC_AUTO_PARSE, Auto_Parse == "yes" ? BST_CHECKED : BST_UNCHECKED);
			SetWindowTextA(HWND_IDC_PATCH_GWS_IP, Gateway_Ip.c_str());
			SetWindowTextA(HWND_IDC_PATCH_GWS_PORT, Gateway_Port.c_str());
			SetWindowTextA(HWND_IDC_PATCH_AGS_IP, Agent_Ip.c_str());
			SetWindowTextA(HWND_IDC_PATCH_AGS_PORT, Agent_Port.c_str());
		} break;

		case WM_COMMAND:
		{
			int button = LOWORD(wParam);
			switch(button)
			{
				case IDCANCEL:
				{
					PostQuitMessage(0);
				}
				break;

				case IDC_REMOVE:
				{
					LRESULT sel = ListBox_GetCurSel(HWND_IDC_DIRS);
					if(sel == LB_ERR)
					{
						break;
					}

					std::string path;
					path.resize(ListBox_GetTextLen(HWND_IDC_DIRS, sel));
					ListBox_GetText(HWND_IDC_DIRS, sel, &path[0]);

					std::map<std::string, SilkroadData>::iterator itr = paths.find(path);
					if(itr == paths.end())
					{
						MessageBox(0, "The selected path was not found.", "Error", MB_ICONERROR);
						break;
					}

					paths.erase(itr);

					ListBox_DeleteString(HWND_IDC_DIRS, sel);

					SendMessage(hWnd, WM_USER + 5, 0, 0); // Save paths

					if(ListBox_GetCount(HWND_IDC_DIRS))
					{
						ListBox_SetCurSel(HWND_IDC_DIRS, 0);
						PostMessage(hWnd, WM_COMMAND, MAKELONG(IDC_DIRS, LBN_SELCHANGE), (LPARAM)HWND_IDC_DIRS);
					}
					else
					{
						ListBox_ResetContent(HWND_IDC_DIVS);
						ListBox_ResetContent(HWND_IDC_HOSTS);
						ListBox_ResetContent(HWND_IDC_PORTS);
						SetWindowTextA(HWND_IDC_COUNTRY, "");
						SetWindowTextA(HWND_IDC_LANGUAGE, "");
						SetWindowTextA(HWND_IDC_LOCALE, "");
						SetWindowTextA(HWND_IDC_VERSION, "");
					}
				} break;

				case IDC_OPEN:
				{
					LRESULT sel = ListBox_GetCurSel(HWND_IDC_DIRS);
					if(sel == LB_ERR)
					{
						break;
					}

					std::string path;
					path.resize(ListBox_GetTextLen(HWND_IDC_DIRS, sel));
					ListBox_GetText(HWND_IDC_DIRS, sel, &path[0]);

					ShellExecuteA(hWnd, "open", path.c_str(), NULL, NULL, SW_NORMAL);
				} break;

				case IDC_REFRESH:
				{
					std::map<std::string, SilkroadData>::iterator itr = paths.begin();
					while(itr != paths.end())
					{
						SilkroadData new_data;
						if(LoadPath(itr->first, new_data) == true)
						{
							itr->second = new_data;
						}
						++itr;
					}

					if(ListBox_GetCount(HWND_IDC_DIRS))
					{
						ListBox_SetCurSel(HWND_IDC_DIRS, 0);
						PostMessage(hWnd, WM_COMMAND, MAKELONG(IDC_DIRS, LBN_SELCHANGE), (LPARAM)HWND_IDC_DIRS);
					}
				} break;

				case IDC_CONFIG:
				{
					std::string path = GetCommonDirectory("edxSilkroadLoader5");
					ShellExecuteA(hWnd, "open", path.c_str(), NULL, NULL, SW_NORMAL);
				} break;

				case IDC_ADD:
				{
					FileChooser fc;
					fc.SetDefaultFileName("sro_client.exe");
					fc.AddFilter("Executable Files", "*.exe");
					fc.SetDialogTitle("Please choose your \"sro_client.exe\" executable...");
					if(fc.ShowChooseFile(true) == false)
					{
						break;
					}

					std::string sel_path = fc.GetSelectedFileDirectory();
					std::transform(sel_path.begin(), sel_path.end(), sel_path.begin(), tolower);

					std::map<std::string, SilkroadData>::iterator itr = paths.find(sel_path);
					if(itr != paths.end())
					{
						MessageBox(0, "The selected path already exists.", "Error", MB_ICONERROR);
						break;
					}

					SilkroadData data;
					if(LoadPath(sel_path, data) == false)
					{
						break;
					}

					paths.insert(std::make_pair(sel_path, data));

					SendMessage(hWnd, WM_USER + 5, 0, 0); // Save paths

					LRESULT index = ListBox_AddString(HWND_IDC_DIRS, sel_path.c_str());
					ListBox_SetCurSel(HWND_IDC_DIRS, index);

					PostMessage(hWnd, WM_COMMAND, MAKELONG(IDC_DIRS, LBN_SELCHANGE), (LPARAM)HWND_IDC_DIRS);
				} break;

				case IDC_DIRS:
				{
					if(HIWORD(wParam) != LBN_SELCHANGE)
					{
						return FALSE;
					}

					LRESULT sel = ListBox_GetCurSel(HWND_IDC_DIRS);
					if(sel == LB_ERR)
					{
						break;
					}

					std::string path;
					path.resize(ListBox_GetTextLen(HWND_IDC_DIRS, sel));
					ListBox_GetText(HWND_IDC_DIRS, sel, &path[0]);

					std::map<std::string, SilkroadData>::iterator itr = paths.find(path);
					if(itr == paths.end())
					{
						MessageBox(0, "The selected path was not found.", "Error", MB_ICONERROR);
						break;
					}

					SilkroadData & data = itr->second;

					Edit_SetText(HWND_IDC_LANGUAGE, data.typeInfo["language"].c_str());

					Edit_SetText(HWND_IDC_COUNTRY, data.typeInfo["country"].c_str());

					std::stringstream localess;
					localess << (int)data.divInfo.locale;
					Edit_SetText(HWND_IDC_LOCALE, localess.str().c_str());

					std::stringstream verss;
					verss << data.version;
					Edit_SetText(HWND_IDC_VERSION, verss.str().c_str());

					ListBox_ResetContent(HWND_IDC_DIVS);
					for(size_t index = 0; index < data.divInfo.divisions.size(); ++index)
					{
						ListBox_AddString(HWND_IDC_DIVS, data.divInfo.divisions[index].name.c_str());
					}

					ListBox_ResetContent(HWND_IDC_HOSTS);

					ListBox_SetCurSel(HWND_IDC_DIVS, 0);
					PostMessage(hWnd, WM_COMMAND, MAKELONG(IDC_DIVS, LBN_SELCHANGE), (LPARAM)HWND_IDC_DIVS);
				} break;

				case IDC_DIVS:
				{
					if(HIWORD(wParam) != LBN_SELCHANGE)
					{
						return FALSE;
					}

					LRESULT sel = ListBox_GetCurSel(HWND_IDC_DIRS);
					if(sel == LB_ERR)
					{
						break;
					}

					std::string path;
					path.resize(ListBox_GetTextLen(HWND_IDC_DIRS, sel));
					ListBox_GetText(HWND_IDC_DIRS, sel, &path[0]);

					std::map<std::string, SilkroadData>::iterator itr = paths.find(path);
					if(itr == paths.end())
					{
						MessageBox(0, "The selected path was not found.", "Error", MB_ICONERROR);
						break;
					}

					SilkroadData & data = itr->second;

					LRESULT divindex = ListBox_GetCurSel(HWND_IDC_DIVS);
					if(divindex == LB_ERR)
					{
						break;
					}

					ListBox_ResetContent(HWND_IDC_HOSTS);

					for(size_t x = 0; x < data.divInfo.divisions[divindex].addresses.size(); ++x)
					{
						std::string hostaddress = data.divInfo.divisions[divindex].addresses[x];
						std::stringstream ss;
						ss << hostaddress;
						ListBox_AddString(HWND_IDC_HOSTS, ss.str().c_str());
					}

					ListBox_SetCurSel(HWND_IDC_HOSTS, 0);

					ListBox_ResetContent(HWND_IDC_PORTS);

					std::stringstream gpss;
					gpss << data.gatePort;
					ListBox_AddString(HWND_IDC_PORTS, gpss.str().c_str());

					ListBox_SetCurSel(HWND_IDC_PORTS, 0);
				} break;

				case IDC_PATCH_ENGLISH:
				{
					std::string & str = English_Patch;
					if(Button_GetCheck((HWND)lParam) == BST_CHECKED)
					{
						str = "yes";
					}
					else
					{
						str = "no";
					}
					SendMessage(hWnd, WM_USER + 6, 0, 0); // Save patches
				} break;

				case IDC_PATCH_MULTI:
				{
					std::string & str = Multiclient;
					if(Button_GetCheck((HWND)lParam) == BST_CHECKED)
					{
						str = "yes";
					}
					else
					{
						str = "no";
					}
					SendMessage(hWnd, WM_USER + 6, 0, 0); // Save patches
				} break;

				case IDC_PATCH_SWEAR:
				{
					std::string & str = Swear_Filter;
					if(Button_GetCheck((HWND)lParam) == BST_CHECKED)
					{
						str = "yes";
					}
					else
					{
						str = "no";
					}
					SendMessage(hWnd, WM_USER + 6, 0, 0); // Save patches
				} break;

				case IDC_PATCH_NUDE:
				{
					std::string & str = Nude_Patch;
					if(Button_GetCheck((HWND)lParam) == BST_CHECKED)
					{
						str = "yes";
					}
					else
					{
						str = "no";
					}
					SendMessage(hWnd, WM_USER + 6, 0, 0); // Save patches
				} break;

				case IDC_PATCH_KOREA:
				{
					std::string & str = Korean_Captcha;
					if(Button_GetCheck((HWND)lParam) == BST_CHECKED)
					{
						str = "yes";
					}
					else
					{
						str = "no";
					}
					SendMessage(hWnd, WM_USER + 6, 0, 0); // Save patches
				} break;

				case IDC_PATCH_REDIRECT_GWS:
				{
					std::string & str = Redirect_Gateway;
					if(Button_GetCheck((HWND)lParam) == BST_CHECKED)
					{
						str = "yes";
					}
					else
					{
						str = "no";
					}
					SendMessage(hWnd, WM_USER + 6, 0, 0); // Save patches
				} break;

				case IDC_PATCH_REDIRECT_AGS:
				{
					std::string & str = Redirect_Agent;
					if(Button_GetCheck((HWND)lParam) == BST_CHECKED)
					{
						str = "yes";
					}
					else
					{
						str = "no";
					}
					SendMessage(hWnd, WM_USER + 6, 0, 0); // Save patches
				} break;

				case IDC_PATCH_CONSOLE:
				{
					std::string & str = Debug_Console;
					if(Button_GetCheck((HWND)lParam) == BST_CHECKED)
					{
						str = "yes";
					}
					else
					{
						str = "no";
					}
					SendMessage(hWnd, WM_USER + 6, 0, 0); // Save patches
				} break;

				case IDC_PATCH_ZOOM:
				{
					std::string & str = Zoom_Hack;
					if(Button_GetCheck((HWND)lParam) == BST_CHECKED)
					{
						str = "yes";
					}
					else
					{
						str = "no";
					}
					SendMessage(hWnd, WM_USER + 6, 0, 0); // Save patches
				} break;

				case IDC_PATCH_HS:
				{
					std::string & str = No_Hackshield;
					if(Button_GetCheck((HWND)lParam) == BST_CHECKED)
					{
						str = "yes";
					}
					else
					{
						str = "no";
					}
					SendMessage(hWnd, WM_USER + 6, 0, 0); // Save patches
				} break;

				case IDC_PATCH_INPUT:
				{
					std::string & str = Hook_Input;
					if(Button_GetCheck((HWND)lParam) == BST_CHECKED)
					{
						str = "yes";
					}
					else
					{
						str = "no";
					}
					SendMessage(hWnd, WM_USER + 6, 0, 0); // Save patches
				} break;

				case IDC_SEED:
				{
					std::string & str = Patch_Seed;
					if(Button_GetCheck((HWND)lParam) == BST_CHECKED)
					{
						str = "yes";
					}
					else
					{
						str = "no";
					}
					SendMessage(hWnd, WM_USER + 6, 0, 0); // Save patches
				} break;

				case IDC_AUTO_PARSE:
				{
					std::string & str = Auto_Parse;
					if(Button_GetCheck((HWND)lParam) == BST_CHECKED)
					{
						str = "yes";
					}
					else
					{
						str = "no";
					}
					SendMessage(hWnd, WM_USER + 6, 0, 0); // Save patches
				} break;

				case IDC_PATCH_GWS_IP:
				{
					std::string & str = Gateway_Ip;
					if(HIWORD(wParam) == EN_CHANGE)
					{
						str.resize(GetWindowTextLength((HWND)lParam));
						GetWindowTextA((HWND)lParam, &str[0], str.size() + 1);
					}
					SendMessage(hWnd, WM_USER + 6, 0, 0); // Save patches
				} break;

				case IDC_PATCH_AGS_IP:
				{
					std::string & str = Agent_Ip;
					if(HIWORD(wParam) == EN_CHANGE)
					{
						str.resize(GetWindowTextLength((HWND)lParam));
						GetWindowTextA((HWND)lParam, &str[0], str.size() + 1);
					}
					SendMessage(hWnd, WM_USER + 6, 0, 0); // Save patches
				} break;

				case IDC_PATCH_GWS_PORT:
				{
					std::string & str = Gateway_Port;
					if(HIWORD(wParam) == EN_CHANGE)
					{
						str.resize(GetWindowTextLength((HWND)lParam));
						GetWindowTextA((HWND)lParam, &str[0], str.size() + 1);
					}
					SendMessage(hWnd, WM_USER + 6, 0, 0); // Save patches
				} break;

				case IDC_PATCH_AGS_PORT:
				{
					std::string & str = Agent_Port;
					if(HIWORD(wParam) == EN_CHANGE)
					{
						str.resize(GetWindowTextLength((HWND)lParam));
						GetWindowTextA((HWND)lParam, &str[0], str.size() + 1);
					}
					SendMessage(hWnd, WM_USER + 6, 0, 0); // Save patches
				} break;

				case IDC_LAUNCH:
				{
					LRESULT sel = ListBox_GetCurSel(HWND_IDC_DIRS);
					if(sel == LB_ERR)
					{
						MessageBoxA(0, "Please select a Silkroad directory first.", "Error", MB_ICONERROR);
						break;
					}

					if(Redirect_Agent == "yes" && Redirect_Gateway == "no")
					{
						MessageBoxA(0, "You must also redirect the GatewayServer if you wish to redirect the AgentServer.", "Error", MB_ICONERROR);
						break;
					}

					if(Redirect_Gateway == "yes")
					{
						std::vector<std::string> tokens = TokenizeString(Gateway_Ip, "\r\n\t .");
						if(tokens.size() != 4)
						{
							MessageBoxA(0, "The GatewayServer IP is in an invalid format. Please fix it to a format of X.X.X.X", "Error", MB_ICONERROR);
							break;
						}

						if(Gateway_Port.empty())
						{
							MessageBoxA(0, "Please enter a GatewayServer port to redirect to.", "Error", MB_ICONERROR);
							break;
						}
					}

					if(Redirect_Agent == "yes")
					{
						std::vector<std::string> tokens = TokenizeString(Agent_Ip, "\r\n\t .");
						if(tokens.size() != 4)
						{
							MessageBoxA(0, "The GatewayServer IP is in an invalid format. Please fix it to a format of X.X.X.X", "Error", MB_ICONERROR);
							break;
						}

						if(Agent_Port.empty())
						{
							MessageBoxA(0, "Please enter a AgentServer port to redirect to.", "Error", MB_ICONERROR);
							break;
						}
					}

					std::string path;
					path.resize(ListBox_GetTextLen(HWND_IDC_DIRS, sel));
					ListBox_GetText(HWND_IDC_DIRS, sel, &path[0]);

					std::map<std::string, SilkroadData>::iterator itr = paths.find(path);
					if(itr == paths.end())
					{
						MessageBox(0, "The selected path was not found.", "Error", MB_ICONERROR);
						break;
					}

					SilkroadData & data = itr->second;

					if(English_Patch == "yes" && data.divInfo.locale == 18)
					{
						MessageBoxA(0, "English Patch is not compatible with ISRO.", "Error", MB_ICONERROR);
						break;
					}

					if(Korean_Captcha == "yes" && data.divInfo.locale != 2)
					{
						MessageBoxA(0, "Korean Captcha Hook only works with KSRO.", "Error", MB_ICONERROR);
						break;
					}

					if(Hook_Input == "yes" && data.divInfo.locale == 2)
					{
						MessageBoxA(hWnd, "The Hook Input patch does not work with KSRO.", "Error", MB_ICONERROR);
						break;
					}

					LRESULT divindex = ListBox_GetCurSel(HWND_IDC_DIVS);
					if(divindex == LB_ERR)
					{
						MessageBoxA(0, "Please select a Division Server", "Error", MB_ICONERROR);
						break;
					}

					LRESULT ipindex = ListBox_GetCurSel(HWND_IDC_HOSTS);
					if(ipindex == LB_ERR)
					{
						MessageBoxA(0, "Please select a Host", "Error", MB_ICONERROR);
						break;
					}

					std::stringstream args;
					args << "0 /" << (int)data.divInfo.locale << " " << divindex << " " << ipindex;

					STARTUPINFOA si = {0};
					PROCESS_INFORMATION pi = {0};
					si.cb = sizeof(STARTUPINFOA);

					std::string pathToClient = data.path;
					pathToClient += "sro_client.exe";

					std::stringstream cmdLine;
					cmdLine << "\"" << pathToClient << "\" " << args.str();

					bool result = (0 != CreateProcessA(0, (LPSTR)cmdLine.str().c_str(), 0, NULL, FALSE, CREATE_SUSPENDED, NULL, NULL, &si, &pi));
					if(result == false)
					{
						MessageBoxA(0, "Could not start \"sro_client.exe\".", "Fatal Error", MB_ICONERROR);
						return 0;
					}

					InjectDLL(pi.hProcess, GetFileEntryPoint(pathToClient.c_str()), pathToDll.c_str(), "Initialize");

					ResumeThread(pi.hThread);
					ResumeThread(pi.hProcess);

					// Got too annoying
					//ShowWindow(hWnd, SW_MINIMIZE);
				} break;

				default:
				{
					return FALSE;
				}
			}
		} break;

		default:
		{
			return FALSE;
		}
	}
	return TRUE;
}

//-------------------------------------------------------------------------
