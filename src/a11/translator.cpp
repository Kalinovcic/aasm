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

    m_localvarIDCounter = 0;
    m_localvarIDs.clear();

    passFunction(name);
}

void TranslatorA11::native()
{
    if(m_scanner->getToken().size() > 2)
        m_log->abort("invalid native def \"" + m_scanner->getToken() + "\"");

    m_scanner->nextTokenEOF();
    std::string name = m_scanner->getToken();

    if(m_functionIDs.find(name) == m_functionIDs.end())
        m_functionIDs[name] = nextFunctionID();
    else m_log->abort("native \"" + name + "\" redeclared");

    m_nativeFunctions.push_back(name);
}

void TranslatorA11::globalvar()
{
    m_scanner->nextTokenEOF();
    std::string name = m_scanner->getToken();
    globalvarIDFor(name, true);
}

void TranslatorA11::writeHeader()
{
    writeByte('A');
    writeByte('B');
    writeByte('Y');
    writeByte(27);

    u16 version = 0;
    write(&version, 2);

    m_filepos += 4;
}

void TranslatorA11::writeNativeData()
{
    u16 nativecount = m_nativeFunctions.size();
    write(&nativecount, 2);

    m_filepos += 2;

    for(std::vector<std::string>::iterator i = m_nativeFunctions.begin(); i != m_nativeFunctions.end(); i++)
    {
        std::string name = *i;

        for(unsigned int i = 0; i < name.size(); i++)
            writeByte(name[i]);
        writeByte(0x00);

        u16 id = functionIDFor(name, false);
        write(&id, 2);

        m_filepos += name.size() + 4;
    }
}

void TranslatorA11::writeGlobalvarData()
{
    u16 globalvarc = m_globalvarIDs.size();
    write(&globalvarc, 2);

    m_filepos += 2;
}

void TranslatorA11::writeFunctionData()
{
    u16 functionc = m_functionIDs.size();
    write(&functionc, 2);

    m_filepos += 2 + (functionc - m_nativeFunctions.size())* 6;

    for(std::map<std::string, u32>::iterator i = m_functionIDs.begin(); i != m_functionIDs.end(); i++)
        if(!isNative(i->first))
        {
            u16 id = i->second;

            write(&id, 2);
            write(&m_filepos, 4);
            m_filepos += getFunctionSize(i->first);
        }
}

void TranslatorA11::writeFunctions()
{
    for(std::map<std::string, u32>::iterator i = m_functionIDs.begin(); i != m_functionIDs.end(); i++)
        if(!isNative(i->first))
            writeFunction(i->first);
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
    writeGlobalvarData();
    writeFunctionData();
    writeFunctions();
}
