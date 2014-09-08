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
 * File: translator_function.cpp
 * Description: 
 * Author: Lovro Kalinovcic
 * 
 */

#include "translator.h"

#define NOP_CODE        0x00

#define PUSHB_CODE      0x10
#define PUSHI_CODE      0x11
#define PUSHF_CODE      0x12

#define LOAD_CODE       0x18
#define LOADWIDE_CODE   0x19
#define FETCH_CODE      0x1A
#define FETCHWIDE_CODE  0x1B

#define ADD_CODE        0x20
#define SUB_CODE        0x21
#define MUL_CODE        0x22
#define DIV_CODE        0x23
#define REM_CODE        0x24
#define NEG_CODE        0x25

#define NOT_CODE        0x30
#define AND_CODE        0x31
#define OR_CODE         0x32
#define XOR_CODE        0x33
#define SHL_CODE        0x34
#define SHR_CODE        0x35

#define LNOT_CODE       0x40
#define LAND_CODE       0x41
#define LOR_CODE        0x42
#define EQ_CODE         0x43
#define NE_CODE         0x44
#define LT_CODE         0x45
#define LE_CODE         0x46
#define GT_CODE         0x47
#define GE_CODE         0x48

#define IF_CODE         0x50
#define IFN_CODE        0x51
#define GOTO_CODE       0x52
#define CALL_CODE       0x53
#define RETURN_CODE     0x54

void TranslatorA10::passFunction(std::string name)
{
    m_pc = 0;
    m_functions[name].inpos = m_scanner->getIn()->tellg();
    m_functions[name].labels.clear();

    while(true)
    {
        m_scanner->nextTokenEOF();
        std::string token = m_scanner->getToken();

        if(token == ".") break;

        if(token[token.size() - 1] == ':')
        {
            m_functions[name].labels[token.substr(0, token.size() - 1)] = m_pc;
            continue;
        }

        m_pc++;
        if(token == "push")
        {
            m_scanner->nextTokenEOF();
            std::string value = m_scanner->getToken();
            if(isBoolean(value)) { m_pc++; }
            else if(isInteger(value)) { m_pc += 8; }
            else if(isFloat(value)) { m_pc += 8; }
            else m_log->abort("unknown push type");
        }
        else
        {
            if(token == "load" || token == "loadwide" || token == "fetch" || token == "fetchwide"
            || token == "if" || token == "ifn" || token == "goto" || token == "call")
            {
                m_pc += 2;
                if(token == "if" || token == "ifn" || token == "goto" || token == "call")
                    m_pc += 2;

                if(token == "load" || token == "loadwide" || token == "fetch" || token == "fetchwide" || token == "goto" || token == "call")
                {
                    m_scanner->nextTokenEOF();
                    if(token == "call")
                        m_scanner->nextTokenEOF();
                }
            }
        }
    }

    m_functions[name].size = m_pc;
}

u32 TranslatorA10::getFunctionSize(std::string name)
{
    return m_functions[name].size;
}

u32 TranslatorA10::getLabelPC(std::string functionName, std::string labelName)
{
    FunctionData fdat = m_functions[functionName];
    if(fdat.labels.find(labelName) == fdat.labels.end())
        m_log->abort("label \"" + labelName + "\" not found");
    return fdat.labels[labelName];
}

void TranslatorA10::writeFunction(std::string name)
{
    m_scanner->getIn()->seekg(m_functions[name].inpos);
    while(true)
    {
        m_scanner->nextTokenEOF();
        std::string token = m_scanner->getToken();
        if(token == ".") break;

        if(token[token.size() - 1] == ':') continue;
        if(token == "nop") { writeByte(NOP_CODE); continue; }
        if(token == "push")
        {
            m_scanner->nextTokenEOF();
            std::string strval = m_scanner->getToken();
            if(isBoolean(strval)) { writeByte(PUSHB_CODE); writeByte(strval == "true" ? 0x01 : 0x00); continue; }
            if(isInteger(strval)) { writeByte(PUSHI_CODE); long value = std::atol(strval.c_str()); write(&value, 8); continue; }
            if(isFloat(strval)) { writeByte(PUSHF_CODE); double value = std::atof(strval.c_str()); write(&value, 8); continue; }
            m_log->abort("unknown push type");
        }
        if(token == "load")
        {
            writeByte(LOAD_CODE);
            m_scanner->nextTokenEOF();
            u16 id = localvarIDFor(m_scanner->getToken(), true);
            write(&id, 2);
            continue;
        }
        if(token == "loadwide")
        {
            writeByte(LOADWIDE_CODE);
            m_scanner->nextTokenEOF();
            u16 id = globalvarIDFor(m_scanner->getToken(), true);
            write(&id, 2);
            continue;
        }
        if(token == "fetch")
        {
            writeByte(FETCH_CODE);
            m_scanner->nextTokenEOF();
            u16 id = localvarIDFor(m_scanner->getToken(), false);
            write(&id, 2);
            continue;
        }
        if(token == "fetchwide")
        {
            writeByte(FETCHWIDE_CODE);
            m_scanner->nextTokenEOF();
            u16 id = globalvarIDFor(m_scanner->getToken(), false);
            write(&id, 2);
            continue;
        }
        if(token == "add") { writeByte(ADD_CODE); continue; }
        if(token == "sub") { writeByte(SUB_CODE); continue; }
        if(token == "mul") { writeByte(MUL_CODE); continue; }
        if(token == "div") { writeByte(DIV_CODE); continue; }
        if(token == "rem") { writeByte(REM_CODE); continue; }
        if(token == "neg") { writeByte(NEG_CODE); continue; }
        if(token == "not") { writeByte(NOT_CODE); continue; }
        if(token == "and") { writeByte(AND_CODE); continue; }
        if(token == "or") { writeByte(OR_CODE); continue; }
        if(token == "xor") { writeByte(XOR_CODE); continue; }
        if(token == "lnot") { writeByte(LNOT_CODE); continue; }
        if(token == "land") { writeByte(LAND_CODE); continue; }
        if(token == "lor") { writeByte(LOR_CODE); continue; }
        if(token == "shl") { writeByte(SHL_CODE); continue; }
        if(token == "shr") { writeByte(SHR_CODE); continue; }
        if(token == "eq") { writeByte(EQ_CODE); continue; }
        if(token == "ne") { writeByte(NE_CODE); continue; }
        if(token == "lt") { writeByte(LT_CODE); continue; }
        if(token == "le") { writeByte(LE_CODE); continue; }
        if(token == "gt") { writeByte(GT_CODE); continue; }
        if(token == "ge") { writeByte(GE_CODE); continue; }
        if(token == "if")
        {
            writeByte(IF_CODE);
            m_scanner->nextTokenEOF();
            m_scanner->nextTokenEOF();
            u32 pc = getLabelPC(name, m_scanner->getToken()); write(&pc, 4);
            continue;
        }
        if(token == "ifn")
        {
            writeByte(IFN_CODE);
            m_scanner->nextTokenEOF();
            u32 pc = getLabelPC(name, m_scanner->getToken()); write(&pc, 4);
            continue;
        }
        if(token == "goto")
        {
            writeByte(GOTO_CODE);
            m_scanner->nextTokenEOF();
            u32 pc = getLabelPC(name, m_scanner->getToken()); write(&pc, 4);
            continue;
        }
        if(token == "call")
        {
            writeByte(CALL_CODE);
            m_scanner->nextTokenEOF();
            u16 id = functionIDFor(m_scanner->getToken(), false);
            m_scanner->nextTokenEOF();
            u8 argc = (u8) std::atoi(m_scanner->getToken().c_str());
            write(&id, 2);
            write(&argc, 1);
            continue;
        }
        if(token == "return") { writeByte(RETURN_CODE); continue; }
        m_log->abort("unrecognized mnemonic \"" + token + "\"");
    }
}
