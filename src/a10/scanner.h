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

#ifndef SCANNER_A10_H_
#define SCANNER_A10_H_

#include <string>
#include <istream>

#include "../scanner.h"

class ScannerA10: public Scanner
{
public:
    ScannerA10(Log* log, std::istream* in): Scanner(log, in) {}
    ~ScannerA10() {}

    std::string getToken()
    {
        return m_token;
    }

    bool nextToken()
    {
        *m_in >> m_token;
        return !m_in->eof();
    }
private:
    std::string m_token;
};

#endif /* SCANNER_A10_H_ */
