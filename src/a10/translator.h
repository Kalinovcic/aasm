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

#ifndef TRANSLATOR_A10_H_
#define TRANSLATOR_A10_H_

#include <string>
#include <ostream>
#include <vector>
#include <map>
#include <algorithm>

#include "../common.h"
#include "../translator.h"

class TranslatorA10: public Translator
{
public:
    TranslatorA10(Log* log, Scanner* scanner, std::ostream* out)
    : Translator(log, scanner, out),
      m_pc(0),
      m_filepos(0),
      m_functionIDCounter(0),
      m_globalvarIDCounter(0),
      m_localvarIDCounter(0) {}
    ~TranslatorA10() {}

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

    u16 m_functionIDCounter;
    u16 m_globalvarIDCounter;
    u16 m_localvarIDCounter;
    std::map<std::string, u32> m_functionIDs;
    std::map<std::string, u32> m_globalvarIDs;
    std::map<std::string, u32> m_localvarIDs;

    std::map<std::string, FunctionData> m_functions;
    std::vector<std::string> m_nativeFunctions;

    inline void write(void* ptr, size_t size) { m_out->write(reinterpret_cast<const char*>(ptr), size); }
    inline void writeByte(u8 byte) { m_out->write(reinterpret_cast<const char*>(&byte), 1); }

    inline u16 nextFunctionID()
    {
        m_functionIDCounter++;
        if(m_functionIDCounter == 0) m_log->abort("function ID overflow");
        return m_functionIDCounter - 1;
    }

    inline u16 nextGlobalvarID()
    {
        m_globalvarIDCounter++;
        if(m_globalvarIDCounter == 0) m_log->abort("globalvar ID overflow");
        return m_globalvarIDCounter - 1;
    }

    inline u16 nextLocalvarID()
    {
        m_localvarIDCounter++;
        if(m_localvarIDCounter == 0) m_log->abort("var ID overflow");
        return m_localvarIDCounter - 1;
    }

    inline bool isNative(std::string name)
    {
        return std::find(m_nativeFunctions.begin(), m_nativeFunctions.end(), name) != m_nativeFunctions.end();
    }

    inline u16 functionIDFor(std::string name, bool create)
    {
        if(m_functionIDs.find(name) == m_functionIDs.end())
        {
            if(create) m_functionIDs[name] = nextFunctionID();
            else m_log->abort("function \"" + name + "\" not found");
        }

        return m_functionIDs[name];
    }

    inline u16 globalvarIDFor(std::string name, bool create)
    {
        if(m_globalvarIDs.find(name) == m_globalvarIDs.end())
        {
            if(create) m_globalvarIDs[name] = nextFunctionID();
            else m_log->abort("globalvar \"" + name + "\" not found");
        }
        return m_globalvarIDs[name];
    }

    inline u16 localvarIDFor(std::string name, bool create)
    {
        if(m_localvarIDs.find(name) == m_localvarIDs.end())
        {
            if(create) m_localvarIDs[name] = nextFunctionID();
            else m_log->abort("var \"" + name + "\" not found");
        }
        return m_localvarIDs[name];
    }

    inline bool isBoolean(std::string s)
    {
        return s == "true" || s == "false";
    }

    inline bool isInteger(std::string s)
    {
        for(unsigned int i = 0; i < s.size(); i++)
            if(s[i] < '0' || s[i] > '9')
                return false;
        return true;
    }

    inline bool isFloat(std::string s)
    {
        bool decpoint = false;
        for(unsigned int i = 0; i < s.size(); i++)
            if(s[i] < '0' || s[i] > '9')
            {
                if(s[i] == '.' && !decpoint) decpoint = true;
                else return false;
            }
        return true;
    }

    void passFunction(std::string name);
    u32 getFunctionSize(std::string name);
    u32 getLabelPC(std::string functionName, std::string labelName);
    void writeFunction(std::string name);

    void nativeFunction(std::string name, bool isVoid);
    void aspelFunction(std::string name);
    void function();
    void globalvar();

    void writeHeader();
    void writeNativeData();
    void writeGlobalvarData();
    void writeFunctionData();
    void writeFunctions();
};

#endif /* TRANSLATOR_A10_H_ */
