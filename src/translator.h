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
 * File: translator.h
 * Description: 
 * Author: Lovro Kalinovcic
 * 
 */

#ifndef TRANSLATOR_H_
#define TRANSLATOR_H_

#include <ostream>

#include "scanner.h"

class Translator
{
public:
    Translator(Log* log, Scanner* scanner, std::ostream* out)
    : m_log(log), m_scanner(scanner), m_out(out) {}
    virtual ~Translator() {}

    virtual void labelPass() {}
    virtual void translationPass() {}
protected:
    Log* m_log;
    Scanner* m_scanner;
    std::ostream* m_out;
};

#endif /* TRANSLATOR_H_ */
