/*! @file tz_system_utils.h
 *  author: Zoltan Gyarmati <mr.zoltan.gyarmati@gmail.com>
 *
 *
 *                   The MIT License (MIT)
 *
 * Copyright (c) 2016 Zoltan Gyarmati <mr.zoltan.gyarmati@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of 
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so, 
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */


// sets the timezone via systemd's timedated
// dbus interface, if available
int system_set_tz_dbus(const char *tz);

// sets the system timezone by manually tinkering with the
// /etc/timezone > /usr/share/zoneinfo/ symlink
int system_set_tz_symlink(const char *tz);


// calls the command line specified with the cmd parameter
// if the cmd has the placeholder '%s', it will be substitued
// with the timezone name string tz
int system_execute_action(const char *cmd, const char* tz);
