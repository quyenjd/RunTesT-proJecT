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
    It contains everything you need to handle a process with vary checks.
**/
#ifndef __THEMISV2_RUNNER__
#define __THEMISV2_RUNNER__

#include "themisv2.h"

#define TIME_LIMIT_DEF 1000
#define MEM_LIMIT_DEF  262144

/* A class for processing command quickly and efficiently.
   It provides functions and voids for accessing process and doing many stuffs.
   Remember to call stop() whenever you do not need it anymore.
*/
class __themisv2_processhandler__ {
private:
    /* Command of the Process. */
    string cmd;

    /* Startup Info for CreateProcess. */
    STARTUPINFO si;

    /* Process Information for CreateProcess. */
    PROCESS_INFORMATION pi;

    /* Time limit for Process (in milliseconds). */
    clock_t time;

    /* Memory limit for Process (in KiloBytes). */
    ui mem;

    /* ConvertFileTime function for QuadPart 1e-7 seconds (must learn). */
    static ULONGLONG ConvertFileTime (const FILETIME* t) {
        ULARGE_INTEGER tmp;
        CopyMemory(&tmp, t, sizeof(ULARGE_INTEGER));
        return tmp.QuadPart;
    }
public:
    /* Initialization. */
    __themisv2_processhandler__() {
        cmd  = "";
        time = TIME_LIMIT_DEF;
        mem  = MEM_LIMIT_DEF;
        si   = {sizeof(STARTUPINFO)};
    }

    /* Constructor. */
    __themisv2_processhandler__ (const string&  _file,        /* Name of the Process */
                                 const string&  _argline,     /* Arguments of the Process */
                                 const clock_t& _time,        /* Time Limit */
                                 const ui&      _mem,         /* Memory Limit */
                                 const string&  _input  = "", /* Input from? */
                                 const string&  _output = "", /* Output to? */
                                 const string&  _stderr = ""  /* Stderr to? */
                                ) {
		if (_file.empty())
			halt(crash, "Process Handler: File name can't be empty!");

        // Get the destination.
        vector<string> r = split(_file, '\\');
		string k = r.back();
        r.pop_back();

        // Construct.
		cmd  = rfmt("cmd /c \"cd /d \"%s\" & \"%s\"", join(r, '\\').c_str(), k.c_str());
        time = _time;
        mem  = _mem;
        si   = {sizeof(STARTUPINFO)};

		// Arguments
		cmd += " " + _argline;

        // You need Input?
        if (!_input.empty())
            cmd += " < \"" + _input + "\"";
		else
			cmd += " <nul ";

        // You need Output?
        if (!_output.empty())
            cmd += " > \"" + _output + "\"";
		else
			cmd += " >nul ";

        // You need Stderr?
        if (!_stderr.empty()) {
			if (_stderr == _output)
				cmd += " 2>&1";
			else
				cmd += " 2> \"" + _stderr + "\"";
		}

		cmd += "\"";
    }

    /* ---  Powerful voids start here  --- */

    /* I'm using CreateProcess for starting a Process.
       All options will be set to default.
    */
    void start() {
        if (!CreateProcess(NULL, const_cast<char*> (cmd.c_str()), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
            halt(crash, "Process Handler: Cannot create process!");
    }

    /* I'm using CloseHandle for closing a Process. */
    void stop() {
		TerminateProcess(pi.hProcess, 0);
		TerminateThread(pi.hThread, 0);
        CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
    }

    /* I'm using PMC for determining PeakPagefileUsage (Maximum used memory). */
    ui memused() {
        PROCESS_MEMORY_COUNTERS pmc = {sizeof(PROCESS_MEMORY_COUNTERS)};
        if (!GetProcessMemoryInfo(pi.hProcess, &pmc, sizeof(pmc)))
            halt(crash, "Process Handler: Cannot get memory info!");
        return pmc.PeakPagefileUsage / 1024;
    }

    /* I'm using GetExitCodeProcess for getting the returned exit code by the process. */
    ui exitcode() {
        DWORD r;
        if (!GetExitCodeProcess(pi.hProcess, &r))
            halt(crash, "Process Handler: Cannot get exit code!");
        return (ui)r;
    }

    /* Check whether the process has been closed yet. */
    inline bool opening() {
        return exitcode() == STILL_ACTIVE;
    }

    /* I'm using GetProcessTimes for determining consumed time by closed process. */
    ui timeinfo() {
        if (opening())
            halt(crash, "Process Handler: Process must be closed before getting consumed time!");
		if (WaitForSingleObject(pi.hProcess, INFINITE) != WAIT_OBJECT_0)
			halt(crash, "Process Handler: Cannot wait for process to get process times!");
        FILETIME Creation, Exit, Kernal, User;
        if (!GetProcessTimes(pi.hProcess, &Creation, &Exit, &Kernal, &User))
            halt(crash, "Process Handler: Cannot get process times!");
        return (ui)(1e-4 * (ConvertFileTime(&Exit) - ConvertFileTime(&Creation)));
    }

    /* This function runs and waits for the program and terminate it if it reaches time limit.
       - It should return inf   if the program has a TLE-verdict.
       - It should return 2*inf if the program has a MLE-verdict.
       Otherwise, it returns the exit code.
       ** mem_used is used for saving maximum used memory.
	   ** time_used is used for saving consumed time.
    */
    ui run_and_wait_in_time_limit (ui& mem_used, ui& time_used) {
        start();
        clock_t now = clock();
        while (opening() && clock() - now <= time) {
            mem_used = memused();
            if (mem_used > mem)
                return 2 * inf;
        }
        if (opening())
            return inf;
        time_used = timeinfo();
        return exitcode();
    }

    /* Or you just want to run it and do not care what it happens (no MLE and TLE checks). */
    ui run_and_wait() {
        start();
        if (WaitForSingleObject(pi.hProcess, INFINITE) != WAIT_OBJECT_0)
			halt(crash, "Process Handler: Cannot wait for process!");
        return exitcode();
    }
};

typedef __themisv2_processhandler__ proc;

#endif // __THEMISV2_RUNNER__
