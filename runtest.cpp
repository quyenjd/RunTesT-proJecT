/*
   Copyright 2018 @quyenjd

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#define RunTesT_proJecT_version "1.0a"

#include <bits/stdc++.h>
#include <windows.h>
#include <dirent.h>
#include <tchar.h>
#include <conio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "themisv2/runner.h"

#define BLACK          0
#define BLUE           1
#define GREEN          2
#define CYAN           3
#define RED            4
#define MAGENTA        5
#define BROWN          6
#define LIGHTGRAY      7
#define DARKGRAY       8
#define LIGHTBLUE      9
#define LIGHTGREEN    10
#define LIGHTCYAN     11
#define LIGHTRED      12
#define LIGHTMAGENTA  13
#define YELLOW        14
#define WHITE         15

#define _ts to_string

#define UP_ARROW     72
#define DOWN_ARROW   80
#define LEFT_ARROW   75
#define RIGHT_ARROW  77
#define fnk_F2       60

#define NEW_GEN  177
#define OLD_GEN  178
#define EXIT     179

#define MAX_IFS 100

using namespace std;

char VolumeName[MAX_PATH];

string path_to_randomer, path_to_solution, input_form, output_form, console_title, TEMP;
bool solution_needed, from_load;
int idx, num_test, time_limit /* in ms */, mem_limit /* in KB */;

const vector<int> except = { '\\', '/', ':', '*', '\"', '<', '>', '|' };

template<typename __T>
string mul (const __T a, int b)
{
    string s;
    for (; b; --b)
        s += a;
    return s;
}

struct file
{
    string fname;
    size_t fsize; // in KB
    bool is_dir;
    file (string __a = "", size_t __b = 0, bool __c = 0): fname(__a), fsize(__b), is_dir(__c) {}

    bool operator< (file fz)
    {
        return is_dir != fz.is_dir ? is_dir > fz.is_dir : fname < fz.fname;
    }

    bool operator== (file fz)
    {
        return fname == fz.fname && fsize == fz.fsize && is_dir == fz.is_dir;
    }
};

struct dire
{
    vector<file> files;
    size_t maximum_dir_name_length = 0;
    size_t maximum_fil_name_length = 0;
    size_t maximum_fil_size_length = 0;
    bool invalid = 0;

    string get (int i)
    {
        if (i >= (int)files.size())
            return "";
        if (files[i].fname == "..")
            return "Back\n";
        if (files[i].is_dir)
            return files[i].fname + mul(' ', maximum_dir_name_length - files[i].fname.length() + 2) + "<DIR>\n";
        return files[i].fname + mul(' ', maximum_fil_name_length - files[i].fname.length() + 2
                                       + maximum_fil_size_length - _ts(files[i].fsize).length()) + _ts(files[i].fsize) + " KB\n";
    }

    void update ()
    {
        maximum_dir_name_length = maximum_fil_name_length = maximum_fil_size_length = 0;
        for (int i = 0; i < (int)files.size(); ++i)
            if (files[i].is_dir)
                maximum_dir_name_length = max(maximum_dir_name_length, files[i].fname.length());
            else
            {
                maximum_fil_name_length = max(maximum_fil_name_length, files[i].fname.length());
                maximum_fil_size_length = max(maximum_fil_size_length, _ts(files[i].fsize).length());
            }
        sort(files.begin(), files.end());
        auto it = find(files.begin(), files.end(), file(".", -1, 1));
        if (it != files.end())
            files.erase(it);
    }

    void clear ()
    {
        maximum_dir_name_length = maximum_fil_name_length = maximum_fil_size_length = invalid = 0;
        files.clear();
    }
};

struct drl
{
    string let, drname;
    drl (string __a = "", string __b = ""): let(__a), drname(__b) {}
};

struct func
{
    string func_;
    int func_id;
    func (string __s = "", int __t = 0): func_(__s), func_id(__t) {}
};

const vector<func> menus = { func("Start new generator", NEW_GEN),
                             func("Load used generator", OLD_GEN),
                             func("Exit", EXIT)
                           };

class console_output_buffer
{
private:
    map<int, string> lines;
    int __x, __y;

public:
    void clear ()
    {
        system("cls");
        lines.clear();
        __x = __y = 0;
    }

    console_output_buffer ()
    {
        clear();
    }

    void gotoXY (int x, int y)
    {
        COORD coord = { max(0, x), max(0, y) };
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
    }

    void clear_line (int line)
    {
        gotoXY(0, line);
        for (int i = 0; i < (int)lines[line].size(); ++i)
            cerr << ' ';
        lines.erase(line);
        if (__y == line)
            __x = 0;
    }

    void clear_from_line (int line)
    {
        for (auto it = lines.lower_bound(line); it != lines.end(); it = lines.lower_bound(line))
            clear_line(it->first);
        set_pos(0, line);
    }

    void out_on_line (int line, string inf)
    {
        clear_line(line);
        gotoXY(0, line);
        cerr << inf;
        lines[line] = inf;
    }

    void out (string inf)
    {
        gotoXY(__x, __y);
        for (int i = 0; i < (int)inf.length(); ++i)
            if (inf[i] != '\n')
            {
                lines[__y] += inf[i];
                ++__x;
                cerr << inf[i];
            }
            else
            {
                __x = 0;
                ++__y;
                clear_line(__y);
            }
    }

    void out_from_coor (int x, int y, string inf)
    {
        int tmpx = __x, tmpy = __y;
        __x = x, __y = y;
        out(inf);
        __x = tmpx, __y = tmpy;
    }

    void set_pos (int x, int y)
    {
        gotoXY(x, y);
        __x = x;
        __y = y;
    }

    void del_last_char_from_line (int line)
    {
        if (lines[line].empty())
            return;
        gotoXY((int)lines[line].size() - 1, line);
        cerr << ' ';
        lines[line].pop_back();
        gotoXY((int)lines[line].size() - 1, line);
    }

    void out_last_on_line (int line, string inf)
    {
        out_from_coor(lines[line].size(), line, inf);
    }

    void color (int Forg, int Back = 0)
    {
        WORD wColor = ((Back & 0x0F) << 4) + (Forg & 0x0F);
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), wColor);
    }

    void rcol ()
    {
        color(LIGHTGRAY, BLACK);
    }

    void del_last ()
    {
        if (!__x && !__y)
            return;
        if (__x)
        {
            --__x;
            gotoXY(__x, __y);
            cerr << ' ';
            gotoXY(__x, __y);
        }
        else
        {
            clear_line(__y);
            __y = max(0, __y - 1);
        }
    }

    void get_position (int &x, int &y)
    {
        x = __x;
        y = __y;
    }
} ob;

// read key with arrow keys and fn keys handling
int readkey (bool &fn, bool &arrow)
{
    fn = arrow = 0;
    int tmp = _getch();
    fn = !tmp;
    arrow = tmp == 224;
    if (fn || arrow)
    {
        tmp = _getch();
        return tmp;
    }
    return tmp;
}

void go_back (string& path)
{
    bool bck = 0;
    while (path.size() && !(bck && path.back() == '\\'))
    {
        bck = 1;
        path.pop_back();
    }
}

string to_lower (string s)
{
    string t;
    for (int i = 0; i < (int)s.length(); ++i)
        t += tolower(s[i]);
    return t;
}

// get working directory
string workingd ()
{
    char buffer[MAX_PATH];
    GetModuleFileName(NULL, buffer, MAX_PATH);
    string path = buffer;
    go_back(path);
    return path;
}

dire getd (string path)
{
    while (path.back() == '\\')
        path.pop_back();
    dire rr;
    WIN32_FIND_DATA ffd;
    basic_string<TCHAR> ts(_T(path + "\\*"));
    vector<TCHAR> buf(ts.begin(), ts.end());
    buf.push_back(_T('\0'));
    TCHAR *szDir = &buf[0];
    HANDLE hFind = INVALID_HANDLE_VALUE;
    hFind = FindFirstFile(szDir, &ffd);
    if (hFind == INVALID_HANDLE_VALUE)
    {
        rr.invalid = 1;
        return rr;
    }
    do
    {
        bool is_dir = ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
        size_t size = -1;
        LARGE_INTEGER filesize;
        if (!is_dir)
        {
            filesize.LowPart = ffd.nFileSizeLow;
            filesize.HighPart = ffd.nFileSizeHigh;
            size = (int)filesize.QuadPart / 1024;
        }
        rr.files.push_back(file((string)ffd.cFileName, size, is_dir));
    }
    while (FindNextFile(hFind, &ffd) != 0);
    if (GetLastError() != ERROR_NO_MORE_FILES)
        rr.invalid = 1;
    FindClose(hFind);
    rr.update();
    return rr;
}

bool check (string fname)
{
    if (fname.length() < 4)
        return 0;
    return fname.substr(fname.length() - 4, 4) == ".exe";
}

void path (string& WORKING, string doing, string suff = "")
{
    console_title += " - Choose " + doing;
    SetConsoleTitle(console_title.c_str());
    while (WORKING.back() == '\\')
        WORKING.pop_back();
    WORKING += '\\';
    bool nff = 0;
    while (1)
    {
        int id = 0;
        char pre = '\\';
        string fi;
        ob.clear();
        ob.out("Choose your file! (Press ");
        if (suff.empty())
        {
            ob.color(YELLOW);
            ob.out("F2");
            ob.rcol();
            ob.out(" to change drive letter or ");
        }
        ob.color(YELLOW);
        ob.out("ESC");
        ob.rcol();
        ob.out(" to go back to main menu)\n");
        ob.out("Current: " + WORKING + '\n');
        ob.out("---\n");
        dire tdirs = getd(WORKING);
        vector<file> sup;
        if (suff.size())
        {
            for (int i = 0; i < (int)tdirs.files.size(); ++i)
                if (tdirs.files[i].fname.size() >= suff.size() &&
                    tdirs.files[i].fname.substr(tdirs.files[i].fname.size() - suff.size(), suff.size()) == suff)
                    sup.push_back(tdirs.files[i]);
            tdirs.files = sup;
            tdirs.update();
        }
        dire dirs = tdirs;
        if (dirs.invalid)
        {
            ob.color(RED);
            ob.out("Invalid directory!\n");
            ob.rcol();
        }
        else
        {
            ob.out("\n");
            OUTPUT_DIR:if (!dirs.files.empty())
            {
                for (int i = 0; i < (int)dirs.files.size(); ++i)
                {
                    if (i == id)
                        ob.out("--> ");
                    else
                        ob.out("... ");
                    ob.out(dirs.get(i));
                }
                nff = 0;
            }
            else
            {
                ob.color(RED);
                ob.out("File not found!\n");
                ob.rcol();
                nff = 1;
            }
        }
        ob.out_last_on_line(1, "");

        // user interact
        while (1)
        {
            bool fn = 0, arrow = 0;
            int tmp = readkey(fn, arrow);
            if (tmp > 126)
                continue;
            if (fn && tmp == fnk_F2 && suff.empty())
            {
                console_title += " - Choose drive letter";
                SetConsoleTitle(console_title.c_str());
                int id = 0;
                vector<drl> dls;
                for (int i = 0; i < 26; ++i)
                {
                    bool valid = GetVolumeInformation((string(1, char(65 + i)) + ":\\").c_str(), VolumeName, MAX_PATH,
                                                      NULL, NULL, NULL, NULL, 0);
                    string s(VolumeName);
                    string t(s.begin(), s.end());
                    if (valid)
                        dls.push_back(drl(string(1, char(65 + i)) + ":\\", t));
                }
                ob.clear();
                ob.out("Your drive letter is: (Use only ");
                ob.color(RED);
                ob.out("UP/DOWN");
                ob.rcol();
                ob.out(" arrow keys to move and ");
                ob.color(RED);
                ob.out("ENTER");
                ob.rcol();
                ob.out(" to select)\n---\n\n");
                for (int i = 0; i < (int)dls.size(); ++i)
                {
                    if (i == id)
                        ob.out("--> ");
                    else
                        ob.out("... ");
                    ob.out(dls[i].let + " - " + dls[i].drname + '\n');
                }
                ob.out_last_on_line(id, "");
                while (1)
                {
                    bool _fn = 0, _arrow = 0;
                    int _tmp = readkey(_fn, _arrow);
                    if (_tmp >= 10 && _tmp <= 13)
                    {
                        WORKING = dls[id].let;
                        break;
                    }
                    if (_arrow && _tmp == UP_ARROW)
                    {
                        int pre = id, now = (id - 1 + (int)dls.size()) % (int)dls.size();
                        ob.out_on_line(pre + 3, "... " + dls[pre].let + " - " + dls[pre].drname + '\n');
                        ob.out_on_line(now + 3, "--> " + dls[now].let + " - " + dls[now].drname + '\n');
                        id = now;
                        ob.out_last_on_line(id + 3, "");
                        continue;
                    }
                    if (_arrow && _tmp == DOWN_ARROW)
                    {
                        int pre = id, now = (id + 1) % (int)dls.size();
                        ob.out_on_line(pre + 3, "... " + dls[pre].let + " - " + dls[pre].drname + '\n');
                        ob.out_on_line(now + 3, "--> " + dls[now].let + " - " + dls[now].drname + '\n');
                        id = now;
                        ob.out_last_on_line(id + 3, "");
                        continue;
                    }
                }
                for (int i = 0; i < 22; ++i)
                    console_title.pop_back();
                SetConsoleTitle(console_title.c_str());
                break;
            }
            if (tmp == 27)
            {
                WORKING = "-1";
                goto END_PHASE2;
            }
            if (arrow && tmp == UP_ARROW && !nff)
            {
                int pre = id, now = (id - 1 + (int)dirs.files.size()) % (int)dirs.files.size();
                ob.out_on_line(pre + 4, "... " + dirs.get(pre));
                ob.out_on_line(now + 4, "--> " + dirs.get(now));
                id = now;
                ob.out_last_on_line(id + 4, "");
                continue;
            }
            if (arrow && tmp == DOWN_ARROW && !nff)
            {
                int pre = id, now = (id + 1) % (int)dirs.files.size();
                ob.out_on_line(pre + 4, "... " + dirs.get(pre));
                ob.out_on_line(now + 4, "--> " + dirs.get(now));
                id = now;
                ob.out_last_on_line(id + 4, "");
                continue;
            }
            if (tmp < 32)
            {
                if (tmp >= 10 && tmp <= 13)
                {
                    if (dirs.invalid || dirs.files.empty())
                        continue;
                    if (dirs.files[id].is_dir)
                    {
                        if (dirs.files[id].fname == "..")
                            go_back(WORKING);
                        else
                            WORKING += dirs.files[id].fname + '\\';
                    }
                    else
                    {
                        if (check(dirs.files[id].fname) || suff.size())
                        {
                            WORKING += dirs.files[id].fname;
                            goto END_PHASE2;
                        }
                        continue;
                    }
                }
                if (tmp == 8)
                {
                    if (fi.empty())
                        continue;
                    fi.pop_back();
                    if (fi.size())
                        pre = fi.back();
                    else
                        pre = '\\';
                    ob.del_last_char_from_line(1);
                    SEARCH:do
                    {
                        ob.clear_from_line(4);
                        dirs.clear();
                        for (int i = id = 0; i < (int)tdirs.files.size(); ++i)
                        {
                            string t1 = to_lower(tdirs.files[i].fname),
                                   t2 = to_lower(fi);
                            if (t1.length() < t2.length())
                                continue;
                            bool ok = 1;
                            for (int j = 0; ok && j < (int)t2.length(); ++j)
                                if (t1[j] != t2[j])
                                    ok = 0;
                            if (ok)
                                dirs.files.push_back(tdirs.files[i]);
                        }
                        dirs.update();
                        goto OUTPUT_DIR;
                        ob.out_last_on_line(1, "");
                    }
                    while (0);
                }
                break;
            }
            if (!fn && !arrow && tmp == '\\' && tmp == pre)
                continue;
            if (!fn && !arrow && tmp == '\\')
            {
                WORKING += tmp;
                break;
            }
            if (!fn && !arrow && find(except.begin(), except.end(), tmp) == except.end())
            {
                fi += tmp;
                ob.out_last_on_line(1, string(1, tmp));
                goto SEARCH;
            }
        }
    }

    END_PHASE2:do
    {
        ob.clear();
        for (int i = 0; i < (10 + (int)doing.length()); ++i)
            console_title.pop_back();
        SetConsoleTitle(console_title.c_str());
    }
    while (0);
}

void test_info ()
{
    console_title += " - Test info";
    SetConsoleTitle(console_title.c_str());
    ob.clear();
    ob.out("Generator: " + path_to_randomer + "\nSolution:  " + path_to_solution + "\n---\n");
    ob.out("Now answer some questions:\n\n");
    ob.out("- Tests should be numbered from? (0/1)               ");
    while (1)
    {
        bool fn = 0, arrow = 0;
        int tmp = readkey(fn, arrow);
        if (fn || arrow)
            continue;
        if (tmp == '0')
        {
            idx = 0;
            break;
        }
        if (tmp == '1')
        {
            idx = 1;
            break;
        }
    }
    ob.out(_ts(idx) + "\n- How many tests do you want? (limit: 5000)          ");
    while (1)
    {
        bool fn = 0, arrow = 0;
        int tmp = readkey(fn, arrow);
        if (fn || arrow)
            continue;
        if (isdigit(tmp) && (num_test * 10 + tmp - '0') <= 5000)
        {
            (num_test *= 10) += tmp - '0';
            ob.out(string(1, tmp));
        }
        if (tmp == 8 && num_test)
        {
            num_test /= 10;
            ob.del_last();
        }
        if (tmp >= 10 && tmp <= 13 && num_test > 0)
            break;
    }
    ob.out("\n- Your time limit? (in milliseconds, limit: 10000)   ");
    while (1)
    {
        bool fn = 0, arrow = 0;
        int tmp = readkey(fn, arrow);
        if (fn || arrow)
            continue;
        if (isdigit(tmp) && (time_limit * 10 + tmp - '0') <= 10000)
        {
            (time_limit *= 10) += tmp - '0';
            ob.out(string(1, tmp));
        }
        if (tmp == 8 && time_limit)
        {
            time_limit /= 10;
            ob.del_last();
        }
        if (tmp >= 10 && tmp <= 13 && time_limit > 0)
            break;
    }
    ob.out("\n- Your memory limit? (in kilobytes, limit: 524288)   ");
    while (1)
    {
        bool fn = 0, arrow = 0;
        int tmp = readkey(fn, arrow);
        if (fn || arrow)
            continue;
        if (isdigit(tmp) && (1LL * mem_limit * 10 + tmp - '0') <= 524288)
        {
            (mem_limit *= 10) += tmp - '0';
            ob.out(string(1, tmp));
        }
        if (tmp == 8 && mem_limit)
        {
            mem_limit /= 10;
            ob.del_last();
        }
        if (tmp >= 10 && tmp <= 13 && mem_limit > 0)
            break;
    }
    ob.out("\n---\nNow, we will ask you about your form of input/output files.\n");
    ob.out("Your form MUST follow the following form:\n");
    ob.color(GREEN);
    ob.out("    [%path\\]folder1\\folder2\\...\\folderN\\myfiles%x.txt\n\n");
    ob.color(YELLOW);
    ob.out("%x");
    ob.rcol();
    ob.out(" can be %0, %1, %2, ..., %9.\n");
    ob.color(YELLOW);
    ob.out("%x");
    ob.rcol();
    ob.out(" is used to define the minimum number of digits of the test number to be added into the text's filename.\n");
    ob.out("So, by default, if the test number is less than 10^(x-1), leading zero(es) will be added.\n");
    ob.out("If you don't need the test number, please don't put ");
    ob.color(YELLOW);
    ob.out("%x");
    ob.rcol();
    ob.out(" in your form.\n\n");
    ob.out("eg. If the test number is ");
    ob.color(YELLOW);
    ob.out("0");
    ob.rcol();
    ob.out("  and the form is 'user");
    ob.color(YELLOW);
    ob.out("%0");
    ob.rcol();
    ob.out(".txt' then the filename is 'user.txt'\n");
    ob.out("    If the test number is ");
    ob.color(YELLOW);
    ob.out("12");
    ob.rcol();
    ob.out(" and the form is 'user");
    ob.color(YELLOW);
    ob.out("%3");
    ob.rcol();
    ob.out(".txt' then the filename is 'user");
    ob.color(YELLOW);
    ob.out("012");
    ob.rcol();
    ob.out(".txt'\n");
    ob.out("    If the test number is ");
    ob.color(YELLOW);
    ob.out("93");
    ob.rcol();
    ob.out(" and the form is 'higuys.txt' then the filename is 'higuys.txt'\n");
    ob.out("    If the test number is ");
    ob.color(YELLOW);
    ob.out("55");
    ob.rcol();
    ob.out(" and the form is 'ts");
    ob.color(YELLOW);
    ob.out("%3");
    ob.rcol();
    ob.out("\\a.inp' then the file is in folder 'ts");
    ob.color(YELLOW);
    ob.out("055");
    ob.rcol();
    ob.out("' and named 'a.inp'\n\n");
    ob.color(WHITE);
    ob.out("%path");
    ob.rcol();
    ob.out(" is the directory that contains the generator.\n");
    ob.out("The part inside ");
    ob.color(WHITE);
    ob.out("[");
    ob.rcol();
    ob.out(" and ");
    ob.color(WHITE);
    ob.out("]");
    ob.rcol();
    ob.out(" MUST NOT be present in your form.\n\n");
    ob.out("--> Your input form:  ");
    char pre = '\\';
    while (1)
    {
        bool fn = 0, arrow = 0;
        int tmp = readkey(fn, arrow);
        if (fn || arrow)
            continue;
        if (tmp == pre && tmp == '\\')
            continue;
        if (find(except.begin(), except.end(), char(tmp)) != except.end() && tmp != '\\')
            continue;
        if (tmp == 8)
        {
            if (input_form.length())
            {
                ob.del_last();
                input_form.pop_back();
                if (input_form.size())
                    pre = input_form.back();
                else
                    pre = '\\';
            }
            continue;
        }
        if (tmp >= 10 && tmp <= 13)
        {
            if (pre == '\\')
                continue;
            break;
        }
        ob.out(string(1, tmp));
        input_form += tmp;
        pre = tmp;
    }
    int obx, oby;
    ob.get_position(obx, oby);
    ob.out_from_coor(0, oby, "...");
    ob.out("\n--> Your output form: ");
    pre = '\\';
    while (1)
    {
        bool fn = 0, arrow = 0;
        int tmp = readkey(fn, arrow);
        if (fn || arrow)
            continue;
        if (tmp == pre && tmp == '\\')
            continue;
        if (find(except.begin(), except.end(), char(tmp)) != except.end() && tmp != '\\')
            continue;
        if (tmp == 8)
        {
            if (output_form.size())
            {
                ob.del_last();
                output_form.pop_back();
                if (output_form.size())
                    pre = output_form.back();
                else
                    pre = '\\';
            }
            continue;
        }
        if (tmp >= 10 && tmp <= 13)
        {
            if (tmp == '\\')
                continue;
            break;
        }
        ob.out(string(1, tmp));
        output_form += tmp;
        pre = tmp;
    }
    ob.clear();
    for (int i = 0; i < 12; ++i)
        console_title.pop_back();
    SetConsoleTitle(console_title.c_str());
}

string fform (string form, int id)
{
    string t;
    for (int i = 0; i < (int)form.length(); ++i)
        if (i && form[i - 1] == '%' && isdigit(form[i]))
        {
            t.pop_back();
            string r = _ts(id);
            while ((int)r.length() < (form[i] - '0'))
                r = '0' + r;
            t += r;
        }
        else
            t += form[i];
    return t;
}

void ensure (string form)
{
    vector<string> dirs = split(form, '\\');
    string tp = dirs[0] + '\\';
    for (int i = 0; i < ((int)dirs.size() - 1); ++i)
    {
        struct stat info;
        if (stat(tp.c_str(), &info) != 0)
            system((string("mkdir \"") + tp + "\" >nul 2>&1").c_str());
        if (i)
            tp += '\\';
        tp += dirs[i + 1];
    }
}

void run_generator ()
{
    console_title += " - Run generator";
    SetConsoleTitle(console_title.c_str());
    ob.clear();
    string zz = path_to_randomer;
    int i, obx, oby, exc;
    bool rerun = 0;

    system((string("del /f /s /q \"") + TEMP + "\" >nul 2>&1").c_str());
    system((string("mkdir \"") + TEMP + "\" >nul 2>&1").c_str());

    // check info
    ob.out("Please double-check your information:\n---\n\n");
    ob.out("- Generator:         ");
    ob.color(YELLOW);
    ob.out(path_to_randomer + '\n');
    ob.rcol();
    ob.out("- Solution:          ");
    ob.color(YELLOW);
    if (solution_needed)
        ob.out(path_to_solution + '\n');
    else
        ob.out("No\n");
    ob.rcol();
    ob.out("- Starting index:    ");
    ob.color(YELLOW);
    ob.out(_ts(idx) + '\n');
    ob.rcol();
    ob.out("- Number of tests:   ");
    ob.color(YELLOW);
    ob.out(_ts(num_test) + '\n');
    ob.rcol();
    ob.out("- Time limit:        ");
    ob.color(YELLOW);
    ob.out(_ts(time_limit) + " ms\n");
    ob.rcol();
    ob.out("- Memory limit:      ");
    ob.color(YELLOW);
    ob.out(_ts(mem_limit) + " KB\n");
    ob.rcol();
    ob.out("- Input form:        ");
    ob.color(YELLOW);
    ob.out(input_form + '\n');
    ob.rcol();
    ob.out("- Output form:       ");
    ob.color(YELLOW);
    ob.out(output_form + "\n\n");
    ob.rcol();
    ob.out("Do you want to continue? (Y/N) ");
    while (1)
    {
        bool fn = 0, arrow = 0;
        int tmp = readkey(fn, arrow);
        if (fn || arrow)
            continue;
        if (tmp == 'Y')
            break;
        if (tmp == 'N')
            goto END_PHASE;
    }

    // run
    ob.clear();
    for (i = idx; i < (num_test + idx); ++i)
    {
        __RUN:do
        {
            ob.out("___ Test " + _ts(i) + "\n\n");
            bool run_yet = 0;

            // generator
            ob.out("--> Running generator ");
            string fcr = path_to_randomer, _fcr;
            go_back(fcr);
            string frf = fform(input_form, i);
            fcr += frf;
            ensure(fcr);
            string _temp = TEMP + frf;
            ensure(_temp);
            go_back(_temp);
            proc gen(path_to_randomer, _ts(i) + ' ' + _ts(num_test), time_limit, mem_limit, "", fcr);
            exc = system((string("copy /y \"") + fcr + "\" \"" + _temp + "\" >nul 2>&1").c_str());
            if (exc)
            {
                ob.clear();
                SetConsoleTitle("Error!");
                ob.out("Cannot copy temporary files.");
                exit(1);
            }
            ui mu = 0, tu = 0, ex = 0;
            int obx, oby;
            ex = gen.run_and_wait_in_time_limit(mu, tu);
            SOME_INFO:do
            {
                ob.out("- Done (time used: ");
                if ((int)tu > time_limit)
                    ob.out("TLE");
                else
                    ob.out(_ts(tu) + " ms");
                ob.out(", mem used: ");
                if ((int)mu > mem_limit)
                    ob.out("MLE");
                else
                    ob.out(_ts(mu) + " KB");
                ob.out(", exitcode: " + _ts(ex) + ")\n");
                ob.get_position(obx, oby);
                ob.out_from_coor(0, oby - 1, "...");
            }
            while (0);

            // run solution (if needed)
            if (solution_needed && !run_yet)
            {
                ob.out("--> Running solution ");
                _fcr = path_to_randomer;
                go_back(_fcr);
                frf = fform(output_form, i);
                _fcr += frf;
                ensure(_fcr);
                _temp = TEMP + frf;
                ensure(_temp);
                go_back(_temp);
                exc = system((string("copy /y \"") + fcr + "\" \"" + _temp + "\" >nul 2>&1").c_str());
                if (exc)
                {
                    ob.clear();
                    SetConsoleTitle("Error!");
                    ob.out("Cannot copy temporary files.");
                    exit(1);
                }
                proc sol(path_to_solution, "", time_limit, mem_limit, fcr, _fcr);
                ex = sol.run_and_wait_in_time_limit(mu, tu);
                run_yet = 1;
                ob.out(" ");
                goto SOME_INFO;
            }

            // print out input
            ob.out("\nInput:\n");
            ifstream fi(fcr);
            char x;
            int ss = 0;
            for (int i = 0; i < (MAX_IFS + 1) && fi.get(x); ++i, ++ss)
                if (i < MAX_IFS)
                    ob.out(string(1, x));
            if (ss > MAX_IFS)
                ob.out("\n...\n");
            else
                ob.out("\n");
            fi.close();

            // print out output (if needed)
            if (solution_needed)
            {
                ob.out("\nOutput:\n");
                ifstream _fi(_fcr);
                ss = 0;
                for (int i = 0; i < (MAX_IFS + 1) && _fi.get(x); ++i, ++ss)
                    if (i < MAX_IFS)
                        ob.out(string(1, x));
                if (ss > MAX_IFS)
                    ob.out("\n...\n");
                else
                    ob.out("\n");
                _fi.close();
            }

            ob.out("\n");
        }
        while (0);
        if (rerun)
            break;
    }

    // rerun
    while (1)
    {
        ob.out("---\n-> Which test do you want to re-run? (Press ");
        ob.color(YELLOW);
        ob.out("a");
        ob.rcol();
        ob.out(" to re-run all tests or ");
        ob.color(YELLOW);
        ob.out("ESC");
        ob.rcol();
        ob.out(" to skip this part)   #");
        int rer = rerun = 0;
        bool esc = 0;
        while (1)
        {
            bool fn = 0, arrow = 0;
            int tmp = readkey(fn, arrow);
            if (fn || arrow)
                continue;
            if (tmp == 8)
            {
                if (rer)
                {
                    ob.del_last();
                    rer /= 10;
                }
                continue;
            }
            if (tmp >= 10 && tmp <= 13)
            {
                if (rer < idx)
                    continue;
                ob.out("\n\n");
                rerun = 1;
                i = rer;
                goto __RUN;
            }
            if (tmp == 27)
            {
                esc = 1;
                ob.out("\n\n");
                break;
            }
            if (tmp == 'a')
            {
                i = idx;
                ob.clear();
                goto __RUN;
            }
            if (!isdigit(tmp))
                continue;
            if ((rer * 10 + tmp - '0') == rer)
                continue;
            if ((rer * 10 + tmp - '0') >= (num_test + idx))
                continue;
            (rer *= 10) += tmp - '0';
            ob.out(string(1, tmp));
        }
        if (esc)
            break;
    }

    // 7z
    if (!system("7za >nul 2>&1"))
    {
        ob.out("---\n");
        ob.color(YELLOW, RED);
        ob.out("[7z]");
        ob.rcol();
        ob.out(" Do you want to pack all files to a zip?\n");
        ob.out("     The zip will be saved in the same directory with the generator and have its name too.\n\n");
        ob.out("- What do you say? (Y/N) ");
        while (1)
        {
            bool fn = 0, arrow = 0;
            int tmp = readkey(fn, arrow);
            if (!fn && !arrow && tmp == 'Y')
                break;
            if (!fn && !arrow && tmp == 'N')
                goto ASK_SAVE;
        }

        for (int i = 0; i < 3; ++i)
            zz.pop_back();
        zz += "zip";
        ob.out("\n\n[7z]   Packing ... ");
        int exc = system((string("7za a -tzip -y \"") + zz + "\" \"" + TEMP + "*\" >nul 2>&1").c_str());
        if (exc)
        {
            ob.clear();
            SetConsoleTitle("Error!");
            ob.out("7z failed to zip files!");
            exit(1);
        }
        ob.get_position(obx, oby);
        ob.clear_from_line(oby);
        ob.out("[7z]   Packing complete! ");
    }

    // ask to save
    ASK_SAVE:if (!from_load)
    {
        ob.out("\n\n---\nDo you want to save this project? (Y/N) ");
        while (1)
        {
            bool fn = 0, arrow = 0;
            int tmp = readkey(fn, arrow);
            if (!fn && !arrow && tmp == 'Y')
                goto SAVE_PHASE;
            if (!fn && !arrow && tmp == 'N')
                goto END_PHASE;
        }
    }

    SAVE_PHASE:if (!from_load)
    do
    {
        string fname;
        ob.out("\nYour file name: ");
        while (1)
        {
            bool fn = 0, arrow = 0;
            int tmp = readkey(fn, arrow);
            if (fn || arrow)
                continue;
            if (tmp == 8)
            {
                if (fname.size())
                {
                    fname.pop_back();
                    ob.del_last();
                }
                continue;
            }
            if (tmp >= 10 && tmp <= 13)
            {
                if (fname.size())
                    break;
                continue;
            }
            if (find(except.begin(), except.end(), char(tmp)) != except.end())
                continue;
            ob.out(string(1, tmp));
            fname += tmp;
        }
        ofstream fo(".\\project\\" + fname + ".runtest");
        fo << path_to_randomer << endl;
        fo << path_to_solution << endl;
        fo << input_form << endl;
        fo << output_form << endl;
        fo << solution_needed << endl;
        fo << idx << endl;
        fo << num_test << endl;
        fo << time_limit << endl;
        fo << mem_limit << endl;
        fo.close();
    }
    while (0);

    END_PHASE:do
    {
        system((string("del /f /s /q \"") + TEMP + "\" >nul 2>&1").c_str());
        ob.clear();
        for (int i = 0; i < 16; ++i)
            console_title.pop_back();
        SetConsoleTitle(console_title.c_str());
    }
    while (0);
}

void new_generator ()
{
    // phase 0: set title
    console_title += " - Start new generator";
    SetConsoleTitle(console_title.c_str());

    // phase 1: pick randomer
    PHASE_1:do
    {
        ob.clear();
        ob.color(YELLOW, RED);
        ob.out("Please answer the questions carefully because you are NOT allowed to change anything past.\n\n");
        ob.rcol();
        ob.out("First, let's choose your generator.\n");
        ob.out("Press ");
        ob.color(YELLOW);
        ob.out("Y");
        ob.rcol();
        ob.out(" to continue to the explorer to pick up your generator file.\n");
        ob.out("Note that the generator should be of the type ");
        ob.color(YELLOW);
        ob.out("*.exe");
        ob.rcol();
        ob.out(".\nThe first 2 arguments of the generator MUST be the index of test and the number of tests, respectively.");
        ob.out("\n\nAre you ready? (Y/N) ");
        while (1)
        {
            bool fn = 0, arrow = 0;
            int tmp = readkey(fn, arrow);
            if (!fn && !arrow && tmp == 'Y')
                break;
            if (!fn && !arrow && tmp == 'N')
                goto PHASE_E;
        }
        path_to_randomer = workingd();
        path(path_to_randomer, "generator");
        if (path_to_randomer == "-1")
            goto PHASE_1;
    }
    while (0);

    // phase 2: pick solution (if needed)
    PHASE_2:do
    {
        ob.clear();
        ob.out("Generator: " + path_to_randomer + "\n---\n");
        ob.out("Second, let's choose your solution.\n");
        ob.out("Press ");
        ob.color(YELLOW);
        ob.out("Y");
        ob.rcol();
        ob.out(" to continue to the explorer to pick up your solution file.\n");
        ob.out("Press ");
        ob.color(RED);
        ob.out("N");
        ob.rcol();
        ob.out(" to generate tests only with no expected outputs needed.\n");
        ob.out("Note that the solution should be of the type ");
        ob.color(YELLOW);
        ob.out("*.exe");
        ob.rcol();
        ob.out(" and the input/output stream MUST be stdin/stdout.\n\n");
        ob.out("Is a solution needed? (Y/N) ");
        while (1)
        {
            bool fn = 0, arrow = 0;
            int tmp = readkey(fn, arrow);
            if (!fn && !arrow && tmp == 'Y')
            {
                solution_needed = 1;
                path_to_solution = workingd();
                path(path_to_solution, "solution");
                if (path_to_solution == "-1")
                    goto PHASE_2;
                break;
            }
            if (!fn && !arrow && tmp == 'N')
                break;
        }
    }
    while (0);

    // phase 3: few questions about tests
    test_info();

    // phase 4: run the generator and solution (if needed)
    run_generator();

    // phase 5: reset title
    PHASE_E:do
    {
        ob.clear();
        for (int i = 0; i < 22; ++i)
            console_title.pop_back();
        SetConsoleTitle(console_title.c_str());
    }
    while (0);
}

void load_generator ()
{
    ob.clear();
    console_title += " - Load generator";
    SetConsoleTitle(console_title.c_str());
    bool reload = 0;

    ob.color(YELLOW, RED);
    ob.out("If the saved file has been modified or changed outside RunTesT, YOU must be responsible for the program's misbehavior!\n");
    ob.rcol();
    ob.out("---\n\nDo you want to go to the explorer to choose your saved project? (Y/N) ");
    while (1)
    {
        bool fn = 0, arrow = 0;
        int tmp = readkey(fn, arrow);
        if (fn || arrow)
            continue;
        if (tmp == 'Y')
            goto LOAD;
        if (tmp == 'N')
            goto END_PHASE3;
    }

    LOAD:do
    {
        from_load = 1;
        string pname = workingd() + "project";
        path(pname, "project", ".runtest");
        if (pname == "-1")
        {
            reload = 1;
            goto END_PHASE3;
        }
        ifstream fi(pname);
        fi >> path_to_randomer >> path_to_solution
           >> input_form       >> output_form
           >> solution_needed  >> idx >> num_test
           >> time_limit       >> mem_limit;
        fi.close();
        run_generator();
    }
    while (0);

    END_PHASE3:do
    {
        ob.clear();
        for (int i = 0; i < 17; ++i)
            console_title.pop_back();
        SetConsoleTitle(console_title.c_str());
        if (reload)
        {
            load_generator();
            return;
        }
    }
    while (0);
}

void Init ()
{
    path_to_randomer.clear();
    path_to_solution.clear();
    input_form      .clear();
    output_form     .clear();
    num_test = time_limit = mem_limit = solution_needed = from_load = 0;
    ob.rcol();
}

void Welcome ()
{
    Init();
    ob.clear();
    ob.out("Welcome to ");
    ob.color(YELLOW, LIGHTBLUE);
    ob.out("RunTesT proJect v1.0a");
    ob.rcol();
    ob.out(" (with 7zip support)!\nThis is an efficient and simple tool that helps you generate tests quickly.\n---\n\n");
    ob.out("What do you want to do? (Use only ");
    ob.color(RED);
    ob.out("UP/DOWN");
    ob.rcol();
    ob.out(" arrow keys to move and ");
    ob.color(RED);
    ob.out("ENTER");
    ob.rcol();
    ob.out(" to select)\n");
    int id = 0, chosen;
    for (int i = 0; i < (int)menus.size(); ++i)
    {
        if (i == id)
            ob.out("--> ");
        else
            ob.out("... ");
        ob.out(menus[i].func_ + '\n');
    }
    ob.out_last_on_line(5, " ");
    while (1)
    {
        bool fn = 0, arrow = 0;
        int tmp = readkey(fn, arrow);
        if (tmp >= 10 && tmp <= 13)
        {
            chosen = menus[id].func_id;
            break;
        }
        if (arrow && tmp == UP_ARROW)
        {
            int pre = id, now = (id - 1 + (int)menus.size()) % (int)menus.size();
            ob.out_on_line(pre + 5, "... " + menus[pre].func_ + '\n');
            ob.out_on_line(now + 5, "--> " + menus[now].func_ + '\n');
            id = now;
            ob.out_last_on_line(id + 5, "");
            continue;
        }
        if (arrow && tmp == DOWN_ARROW)
        {
            int pre = id, now = (id + 1) % (int)menus.size();
            ob.out_on_line(pre + 5, "... " + menus[pre].func_ + '\n');
            ob.out_on_line(now + 5, "--> " + menus[now].func_ + '\n');
            id = now;
            ob.out_last_on_line(id + 5, "");
            continue;
        }
    }
    switch (chosen)
    {
    case NEW_GEN:
        new_generator();
        break;

    case OLD_GEN:
        load_generator();
        break;

    case EXIT:
        ob.clear();
        ob.out("Goodbye!\n");
        return;

    default:
        break;
    }
    ob.clear();
    Welcome();
}

int main ()
{
    console_title = string("RunTesT proJecT v") + RunTesT_proJecT_version;
    SetConsoleTitle(console_title.c_str());
    system("mkdir project >nul 2>&1");
    GetTempPath(MAX_PATH, VolumeName);
    ostringstream _conv;
    _conv << VolumeName;
    TEMP = _conv.str() + "runtest\\";
    Welcome();
    return 0;
}
