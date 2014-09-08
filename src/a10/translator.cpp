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

void TranslatorA10::nativeFunction(std::string name, bool isVoid)
{
    if(isNative(name))
        m_log->abort("duplicate native \"" + name + "\"");
    m_nativeFunctions.push_back(name);

    m_log->info("found native \"" + name + "\"");
}

void TranslatorA10::aspelFunction(std::string name)
{
    m_localvarIDCounter = 0;
    m_localvarIDs.clear();

    passFunction(name);

    m_log->info("found aspel function \"" + name + "\"");
}

void TranslatorA10::function()
{
    std::string token = m_scanner->getToken();

    if(token.size() > 4) m_log->abort("invalid function def \"" + token + "\"");

    bool isNative = false, isVoid = false;
    for(unsigned int i = 2; i < token.size(); i++)
        switch(token[i])
        {
        case 'n': if(isNative) m_log->abort("invalid function def \"" + token + "\""); isNative = true; break;
        case 'v': if(isVoid) m_log->abort("invalid function def \"" + token + "\""); isVoid = true; break;
        }

    m_scanner->nextTokenEOF();
    std::string name = m_scanner->getToken();

    if(m_functionIDs.find(name) == m_functionIDs.end())
        m_functionIDs[name] = nextFunctionID();
    else m_log->abort("function \"" + name + "\" redeclared");

    if(isNative) nativeFunction(name, isVoid);
    else aspelFunction(name);
}

void TranslatorA10::globalvar()
{
    m_scanner->nextTokenEOF();
    std::string name = m_scanner->getToken();
    globalvarIDFor(name, true);

    m_log->info("found globalvar \"" + name + "\"");
}

void TranslatorA10::writeHeader()
{
    m_log->info("writing header");

    writeByte('A');
    writeByte('B');
    writeByte('Y');
    writeByte(27);

    m_filepos += 4;
}

void TranslatorA10::writeNativeData()
{
    m_log->info("writing native data");

    u16 nativecount = m_nativeFunctions.size();
    write(&nativecount, 2);

    m_filepos += 2;

    for(std::vector<std::string>::iterator i = m_nativeFunctions.begin(); i != m_nativeFunctions.end(); i++)
    {
        std::string name = *i;
        m_log->info("writing native function \"" + name + "\"");

        for(unsigned int i = 0; i < name.size(); i++)
            writeByte(name[i]);
        writeByte(0x00);

        u16 id = functionIDFor(name, false);
        write(&id, 2);

        m_filepos += name.size() + 3;
    }
}

void TranslatorA10::writeGlobalvarData()
{
    m_log->info("writing globalvar data");

    u16 globalvarc = m_globalvarIDs.size();
    write(&globalvarc, 2);

    m_filepos += 2;
}

void TranslatorA10::writeFunctionData()
{
    m_log->info("writing function data");

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

void TranslatorA10::writeFunctions()
{
    for(std::map<std::string, u32>::iterator i = m_functionIDs.begin(); i != m_functionIDs.end(); i++)
        if(!isNative(i->first))
        {
            m_log->info("writing function \"" + i->first + "\"");
            writeFunction(i->first);
        }
}

void TranslatorA10::labelPass()
{
    m_log->info("label pass:");
    while(m_scanner->nextToken())
    {
        std::string token = m_scanner->getToken();
        if(token.length() < 2 || token[1] != ':')
            m_log->abort("invalid block def \"" + token + "\"");

        switch(token[0])
        {
        case 'f': function(); break;
        case 'w': globalvar(); break;
        }
    }
}

void TranslatorA10::translationPass()
{
    m_log->info("translation pass:");

    writeHeader();
    writeNativeData();
    writeGlobalvarData();
    writeFunctionData();
    writeFunctions();
}
