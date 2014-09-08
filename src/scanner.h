/* 
 * Copyright (C) 2014 Lovro Kalinovcic
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 * 
 * File: scanner.h
 * Description: 
 * Author: Lovro Kalinovcic
 * 
 */

#ifndef SCANNER_H_
#define SCANNER_H_

#include <istream>

#include "log.h"

class Scanner
{
public:
    Scanner(Log* log, std::istream* in)
    : m_log(log), m_in(in) {}
    virtual ~Scanner() {}

    virtual std::string getToken() { return ""; }
    virtual bool nextToken() { return false; }

    inline void nextTokenEOF()
    {
        if(!nextToken())
            m_log->abort("EOF not expected after " + getToken());
    }

    inline std::istream* getIn() { return m_in; }
protected:
    Log* m_log;
    std::istream* m_in;
};

#endif /* SCANNER_H_ */
