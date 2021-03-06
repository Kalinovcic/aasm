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
 * File: translator.cpp
 * Description: 
 * Author: Lovro Kalinovcic
 * 
 */

#include "translator.h"

#include <iostream>

void TranslatorA11::function()
{
    if(m_scanner->getToken().size() > 2)
        m_log->abort("invalid function def \"" + m_scanner->getToken() + "\"");

    m_scanner->nextTokenEOF();
    std::string name = m_scanner->getToken();

    if(m_functionIDs.find(name) == m_functionIDs.end())
        m_functionIDs[name] = nextFunctionID();
    else m_log->abort("function \"" + name + "\" redeclared");

    if(name == "main") m_main = m_functionIDs[name];

    m_lvarMPosCounter = 0;
    m_lvarMPos.clear();

    passFunction(m_functionIDs[name]);
}

void TranslatorA11::native()
{
    if(m_scanner->getToken().size() > 2)
        m_log->abort("invalid native def \"" + m_scanner->getToken() + "\"");

    m_scanner->nextTokenEOF();
    std::string name = m_scanner->getToken();

    if(m_nativeIDs.find(name) == m_nativeIDs.end())
        m_nativeIDs[name] = nextNativeID();
    else m_log->abort("native \"" + name + "\" redeclared");

    m_nativeFunctions.push_back(name);
}

void TranslatorA11::globalvar()
{
    m_scanner->nextTokenEOF();
    std::string amltype = m_scanner->getToken();
    u32 size = 0;
    if(amltype == "i4") size = 4;
    else if(amltype == "i8") size = 8;
    else if(amltype == "f4") size = 4;
    else if(amltype == "f8") size = 8;
    else m_log->abort("invalid AML type " + amltype);
    m_scanner->nextTokenEOF();
    std::string name = m_scanner->getToken();
    gvarMPosFor(name, true, size);
}

void TranslatorA11::writeHeader()
{
    writeByte('A');
    writeByte('B');
    writeByte('Y');
    writeByte(27);

    u16 version = 0;
    write(&version, 2);

    m_filepos += 6;
}

void TranslatorA11::writeNativeData()
{
    u32 nativecount = m_nativeIDCounter;
    write(&nativecount, 4);
    m_filepos += 4;

    for(u32 i = 0; i < nativecount; i++)
    {
        std::string name = m_nativeFunctions[i];
        for(unsigned int i = 0; i < name.size(); i++)
            writeByte(name[i]);
        writeByte(0x00);
        m_filepos += name.size();
    }
}

void TranslatorA11::writeFunctions()
{
    u32 functionc = m_functionIDCounter;
    write(&functionc, 4);
    m_filepos += 4;

    for(u32 i = 0; i < functionc; i++)
    {
        u32 size = getFunctionSize(i);
        write(&size, 4);
        m_filepos += 4;
        writeFunction(i);
    }

    write(&m_main, 4);
}

void TranslatorA11::writeGlobalvarData()
{
    write(&m_gvarMPosCounter, 4);
    m_filepos += 4;
}

void TranslatorA11::labelPass()
{
    while(m_scanner->nextToken())
    {
        std::string token = m_scanner->getToken();
        if(token.length() < 2 || token[1] != ':')
            m_log->abort("invalid block def \"" + token + "\"");

        switch(token[0])
        {
        case 'f': function(); break;
        case 'n': native(); break;
        case 'w': globalvar(); break;
        }
    }
}

void TranslatorA11::translationPass()
{
    writeHeader();
    writeNativeData();
    writeFunctions();
    writeGlobalvarData();
}
