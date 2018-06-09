/** themisv2 Project (compiled with MinGW - GNU C++11)
    ---
    Copyright (C) 2017 @quyenjd

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.ved.
    ---
    This header is a part of themisv2 Project.
    It contains all mainly used functions.
**/
#ifndef __THEMISV2__
#define __THEMISV2__

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <limits.h>
#include <stdarg.h>
#include <stdnoreturn.h>
#include <stdlib.h>

#ifndef THEMISV2
#include <conio.h>
#include <windows.h>
#include <tchar.h>
#include <psapi.h>
#include <winbase.h>
#endif

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>

/* Macro-defined exitcodes. */
#define CE    2
#define TBS   3
#define UKNL  4
#define crash 5

#define log __log__

#define mp make_pair
#define pb push_back
#define fi first
#define se second
#define ALL(a) (a).begin(),(a).end()

using namespace std;

typedef long long ll;
typedef unsigned int ui;

typedef pair<int, int> pi;
typedef vector<int>    vi;
typedef vector<pi>     vp;

const int inf = 0x3f3f3f3f;

/* Logfile:
   --------
   You can use the logfile in order to develop an interface, etc.
   It will be as clear as possible.

   UNDERSTANDING:
   --------------
   - Logs appear one per line.
   - Each line starts with date and time then exitcode and message.
*/
string logfile;
fstream err;

/* Translate exitcode to message. */
string trans (const int& exitcode) {
    if (exitcode == 0)
        return "accepted";
    if (exitcode == 1)
        return "error";
    if (exitcode == 2)
        return "compilation_error";
    if (exitcode == 3)
        return "tbsource";
    if (exitcode == 4)
        return "ukn_lang";
    return "crash";
}

/* Multiply string by times, eg. "a" x 3 = "aaa". */
template<typename T, typename _T>
string operator* (const T& a, const _T& b) {
    string r, t = string(a);
    int k = int(b);
    while (k-- > 0)
        r += t;
    return r;
}

/* Convert ints to strings. */
template<typename T>
inline string to_string (const T& t) {
    string r;
    stringstream s;
    s << t;
    s >> r;
    return r;
}

/* Minimize. */
template<typename T>
inline void mini (T& a, const T& b) {
    if (b < a)
        a = b;
}

/* Maximize. */
template<typename T>
inline void maxi (T& a, const T& b) {
    if (b > a)
        a = b;
}

/* Open file with mode. */
void open (fstream& f, const string& fn, ios_base::openmode mode = fstream::in | fstream::out | fstream :: app);

/* Reformat string with args, work like printf. */
string rfmt (const char* fmt...) {
    stringstream r;
    va_list args;
    va_start(args, fmt);
    while (*fmt != '\0') {
        char x = *fmt;
        if (x == '%') {
            ++fmt;
            if (*fmt == 'd') {
                int t = va_arg(args, int);
                r << t;
            } else
            if (*fmt == 'c') {
                int t = va_arg(args, int);
                r << char(t);
            } else
            if (*fmt == 's') {
                char* t = va_arg(args, char*);
                r << t;
            } else
            if (*fmt == 'f') {
                double t = va_arg(args, double);
                r << t;
            } else r << x << *fmt;
        } else r << x;
        ++fmt;
    }
    return r.str();
}

/* Write logs. */
void tolog (const string& r) {
    if (!err.is_open())
        if (logfile.size())
            open(err, logfile);
    if (logfile.size())
        err << r << endl;
    cerr << r << endl;
}

/* Date and Time. */
string dt() {
    // Get time from Windows.
    time_t now = time(0);
    tm *ltm = localtime(&now);

    // Variables.
    int mon  = ltm->tm_mon + 1,
        day  = ltm->tm_mday,
        hour = ltm->tm_hour,
        minn = ltm->tm_min,
        sec  = ltm->tm_sec;

    // Return the form [MM/DD/YYYY hh:mm:ss].
    return string("0") * (mon < 10) + to_string(mon) + "/" +
           string("0") * (day < 10) + to_string(day) + "/" +
           to_string(1900 + ltm->tm_year) + " " +
           string("0") * (hour < 10) + to_string(hour) + ":" +
           string("0") * (minn < 10) + to_string(minn) + ":" +
           string("0") * (sec < 10) + to_string(sec);
}

/* Halt the program with message and exitcode. */
[[noreturn]] void halt (int exitcode, const string& message = "") {
    string t = rfmt("[%s] themisv2 returned exitcode %d (%s) with message: \"%s\"",
                    dt().c_str(),
                    exitcode,
                    trans(exitcode).c_str(),
                    message.c_str());
    tolog(t);
    exit(exitcode);
}

/* Open file with mode - Implementation. */
void open (fstream& f, const string& fn, ios_base::openmode mode) {
    f.open(fn, mode);
    if (!f)
        halt(crash, rfmt("Main header: File \"%s\" can't be opened!", fn.c_str()));
}

/* --- Split and join string tools start here --- */

/* Split string by token. */
vector<string> split (const string& s, const char& token) {
	vector<string> rep;
	rep.push_back("");
	for (size_t i = 0; i < s.length(); ++i) {
		if (s[i] != token)
			rep.back() += s[i];
		else
			rep.push_back("");
	}
	return rep;
}

/* Join string (with token). */
template<typename T>
string join (const vector<string>& a, const T& token = "") {
    if (a.empty())
        return "";
    string rep;
    for (size_t i = 0; i < a.size() - 1; ++i)
        rep += a[i] + token;
    return rep + a[a.size() - 1];
}

#endif // __THEMISV2__
