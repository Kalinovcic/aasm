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

#ifndef TRANSLATOR_A11_H_
#define TRANSLATOR_A11_H_

#include <string>
#include <ostream>
#include <vector>
#include <map>
#include <algorithm>

#include "../common.h"
#include "../translator.h"

class TranslatorA11: public Translator
{
public:
    TranslatorA11(Log* log, Scanner* scanner, std::ostream* out)
    : Translator(log, scanner, out),
      m_pc(0),
      m_filepos(0),
      m_main(0),
      m_functionIDCounter(0),
      m_gvarMPosCounter(0),
      m_lvarMPosCounter(0) {}
    ~TranslatorA11() {}

    void labelPass();
    void translationPass();
private:
    struct FunctionData
    {
        std::streampos inpos;
        std::map<std::string, u32> labels;
        u32 size;
    };

    u32 m_pc;
    u32 m_filepos;
    u32 m_main;

    u32 m_functionIDCounter;
    u32 m_gvarMPosCounter;
    u32 m_lvarMPosCounter;
    std::map<std::string, u32> m_functionIDs;
    std::map<std::string, u32> m_gvarMPos;
    std::map<std::string, u32> m_lvarMPos;

    std::map<std::string, FunctionData> m_functions;
    std::vector<std::string> m_nativeFunctions;

    inline void write(void* ptr, size_t size) { m_out->write(reinterpret_cast<const char*>(ptr), size); }
    inline void writeByte(u8 byte) { m_out->write(reinterpret_cast<const char*>(&byte), 1); }

    inline u32 nextFunctionID()
    {
        m_functionIDCounter++;
        if(m_functionIDCounter == 0) m_log->abort("function ID overflow");
        return m_functionIDCounter - 1;
    }

    inline u32 nextGVarMPos(u32 size)
    {
        u32 old = m_gvarMPosCounter;
        m_gvarMPosCounter += size;
        if(m_gvarMPosCounter < old) m_log->abort("globalvar ID overflow");
        return m_gvarMPosCounter - size;
    }

    inline u32 nextLVarMPos(u32 size)
    {
        u32 old = m_lvarMPosCounter;
        m_lvarMPosCounter += size;
        if(m_lvarMPosCounter < old) m_log->abort("localvar ID overflow");
        return m_lvarMPosCounter - size;
    }

    inline bool isNative(std::string name)
    {
        return std::find(m_nativeFunctions.begin(), m_nativeFunctions.end(), name) != m_nativeFunctions.end();
    }

    inline u32 functionIDFor(std::string name, bool create)
    {
        if(m_functionIDs.find(name) == m_functionIDs.end())
        {
            if(create) m_functionIDs[name] = nextFunctionID();
            else m_log->abort("function \"" + name + "\" not found");
        }

        return m_functionIDs[name];
    }

    inline u32 gvarMPosFor(std::string name, bool create, u32 size)
    {
        if(m_gvarMPos.find(name) == m_gvarMPos.end())
        {
            if(create) m_gvarMPos[name] = nextGVarMPos(size);
            else m_log->abort("globalvar \"" + name + "\" not found");
        }
        return m_gvarMPos[name];
    }

    inline u32 lvarMPosFor(std::string name, bool create, u32 size)
    {
        if(m_lvarMPos.find(name) == m_lvarMPos.end())
        {
            if(create) m_lvarMPos[name] = nextLVarMPos(size);
            else m_log->abort("var \"" + name + "\" not found");
        }
        return m_lvarMPos[name];
    }

    void passFunction(std::string name);
    u32 getFunctionSize(std::string name);
    u32 getLabelPC(std::string functionName, std::string labelName);
    void writeFunction(std::string name);

    void function();
    void native();
    void globalvar();

    void writeHeader();
    void writeNativeData();
    void writeFunctions();
    void writeGlobalvarData();
};

#endif /* TRANSLATOR_A11_H_ */
