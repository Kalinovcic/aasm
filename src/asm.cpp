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
 * File: asm.cpp
 * Description: 
 * Author: Lovro Kalinovcic
 * 
 */

#include <iostream>
#include <cstdlib>
#include <fstream>

#include "translator.h"
#include "a10/translator.h"

#include "scanner.h"
#include "a10/scanner.h"

int main(int argc, char** argv)
{
    std::ifstream* in = new std::ifstream();
    std::ofstream* out = new std::ofstream();

    Log* log = new Log();
    log->setStream(&std::cout, Log::INFO);
    log->setStream(&std::cout, Log::WARNING);
    log->setStream(&std::cerr, Log::ERROR);

    Scanner* scanner = new ScannerA10(log, in);
    Translator* translator = new TranslatorA10(log, scanner, out);

    in->open("../aspelc/test.aml", std::ios::in);
    out->open("../aspelc/test.aby", std::ios::out | std::ios::binary);
    translator->labelPass();
    in->close();
    out->close();

    in->open("../aspelc/test.aml", std::ios::in);
    out->open("../aspelc/test.aby", std::ios::out | std::ios::binary);
    translator->translationPass();
    in->close();
    out->close();

    return EXIT_SUCCESS;
}
