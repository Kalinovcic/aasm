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

#define OP_NOP          0x00
#define OP_PUSH4        0x08
#define OP_PUSH8        0x09
#define OP_POP4         0x0C
#define OP_POP8         0x0D

#define OP_LOAD4        0x10
#define OP_LOAD8        0x11
#define OP_LOADWIDE4    0x12
#define OP_LOADWIDE8    0x13
#define OP_FETCH4       0x18
#define OP_FETCH8       0x19
#define OP_FETCHWIDE4   0x1A
#define OP_FETCHWIDE8   0x1B

#define OP_ALLOC        0x20
#define OP_FREE         0x21

#define OP_REFL1        0x30
#define OP_REFL2        0x31
#define OP_REFL4        0x32
#define OP_REFL8        0x33
#define OP_EXTR1        0x38
#define OP_EXTR2        0x39
#define OP_EXTR4        0x3A
#define OP_EXTR8        0x3B

#define OP_SWAP4        0x40
#define OP_SWAP8        0x41
#define OP_SWAP48       0x42
#define OP_SWAP84       0x43
#define OP_DUP4         0x48
#define OP_DUP8         0x49

#define OP_ADDI4        0x50
#define OP_ADDI8        0x51
#define OP_ADDF4        0x52
#define OP_ADDF8        0x53
#define OP_SUBI4        0x54
#define OP_SUBI8        0x55
#define OP_SUBF4        0x56
#define OP_SUBF8        0x57
#define OP_MULI4        0x58
#define OP_MULI8        0x59
#define OP_MULF4        0x5A
#define OP_MULF8        0x5B
#define OP_DIVI4        0x5C
#define OP_DIVI8        0x5D
#define OP_DIVU4        0x5E
#define OP_DIVU8        0x5F
#define OP_DIVF4        0x60
#define OP_DIVF8        0x61
#define OP_REMI4        0x62
#define OP_REMI8        0x63
#define OP_REMU4        0x64
#define OP_REMU8        0x65
#define OP_NEGI4        0x66
#define OP_NEGI8        0x67
#define OP_NEGF4        0x68
#define OP_NEGF8        0x69

#define OP_SHL4         0x80
#define OP_SHL8         0x81
#define OP_SHR4         0x82
#define OP_SHR8         0x83
#define OP_SHRU4        0x84
#define OP_SHRU8        0x85
#define OP_BNOT4        0x86
#define OP_BNOT8        0x87
#define OP_BAND4        0x88
#define OP_BAND8        0x89
#define OP_BXOR4        0x8A
#define OP_BXOR8        0x8B
#define OP_BOR4         0x8C
#define OP_BOR8         0x8D

#define OP_LNOT4        0xA0
#define OP_LNOT8        0xA1
#define OP_LAND4        0xA2
#define OP_LAND8        0xA3
#define OP_LOR4         0xA4
#define OP_LOR8         0xA5

#define OP_CI14         0xB0
#define OP_CI24         0xB1
#define OP_CI41         0xB2
#define OP_CI42         0xB3
#define OP_CI48         0xB4
#define OP_CI84         0xB5
#define OP_CF48         0xB6
#define OP_CF84         0xB7
#define OP_CFI4         0xB8
#define OP_CFI8         0xB9
#define OP_CIF4         0xBA
#define OP_CIF8         0xBB

#define OP_GOTO         0xD0
#define OP_CALL         0xD1
#define OP_RETURN       0xD2
#define OP_IF           0xD4
#define OP_IFN          0xD5
#define OP_LTNL         0xDA
#define OP_LENL         0xDB
#define OP_GTNL         0xDC
#define OP_GENL         0xDD
#define OP_EQNL         0xDE
#define OP_NENL         0xDF

#define OP_CMP4         0xE0
#define OP_CMP8         0xE1
#define OP_ICMP4        0xE2
#define OP_ICMP8        0xE3
#define OP_IUCMP4       0xE4
#define OP_IUCMP8       0xE5
#define OP_IUCMPR4      0xE6
#define OP_IUCMPR8      0xE7
#define OP_FCMP4        0xE8
#define OP_FCMP8        0xE9
#define OP_FICMP4       0xEA
#define OP_FICMP8       0xEB
#define OP_FICMPR4      0xEC
#define OP_FICMPR8      0xED
#define OP_FUCMP4       0xEE
#define OP_FUCMP8       0xEF
#define OP_FUCMPR4      0xF1
#define OP_FUCMPR8      0xF2

void TranslatorA11::passFunction(std::string name)
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
        if(token == "load4" || token == "load8" || token == "fetch4" || token == "fetch8"
        || token == "loadwide4" || token == "loadwide8" || token == "fetchwide4" || token == "fetchwide8"
        || token == "if" || token == "ifn" || token == "goto" || token == "call"
        || token == "pushi4" || token == "pushi8"|| token == "pushf4" || token == "pushf8")
        {
            m_pc += 4;
            if(token == "pushi8" || token == "pushf8") m_pc += 4;

            if(!(token == "if" || token == "ifn"))
                m_scanner->nextTokenEOF();
        }
    }

    m_functions[name].size = m_pc;
}

u32 TranslatorA11::getFunctionSize(std::string name)
{
    return m_functions[name].size;
}

u32 TranslatorA11::getLabelPC(std::string functionName, std::string labelName)
{
    FunctionData fdat = m_functions[functionName];
    if(fdat.labels.find(labelName) == fdat.labels.end())
        m_log->abort("label \"" + labelName + "\" not found");
    return fdat.labels[labelName];
}
#include <iostream>

void TranslatorA11::writeFunction(std::string name)
{
    m_scanner->getIn()->seekg(m_functions[name].inpos);
    while(true)
    {
        m_scanner->nextTokenEOF();
        std::string token = m_scanner->getToken();
        if(token == ".") break;
        if(token[token.size() - 1] == ':') continue;

        if(token == "nop") { writeByte(OP_NOP); continue; }
        if(token == "pushi4")
        {
            writeByte(OP_PUSH4);
            m_scanner->nextTokenEOF();
            i32 value = std::atoi(m_scanner->getToken().c_str());
            write(&value, 4);
            continue;
        }
        if(token == "pushi8")
        {
            writeByte(OP_PUSH8);
            m_scanner->nextTokenEOF();
            i64 value;
            if(m_scanner->getToken()[0] == '@') value = lvarMPosFor(m_scanner->getToken().substr(1), false, 0);
            else value = std::atoll(m_scanner->getToken().c_str());
            write(&value, 8);
            continue;
        }
        if(token == "pushf4")
        {
            writeByte(OP_PUSH4);
            m_scanner->nextTokenEOF();
            f32 value = std::atof(m_scanner->getToken().c_str());
            write(&value, 4);
            continue;
        }
        if(token == "pushf8")
        {
            writeByte(OP_PUSH8);
            m_scanner->nextTokenEOF();
            f64 value = std::atof(m_scanner->getToken().c_str());
            write(&value, 8);
            continue;
        }
        if(token == "pop4") { writeByte(OP_POP4); continue; }
        if(token == "pop8") { writeByte(OP_POP8); continue; }
        if(token == "load4")
        {
            writeByte(OP_LOAD4);
            m_scanner->nextTokenEOF();
            u32 mpos = lvarMPosFor(m_scanner->getToken(), true, 4);
            write(&mpos, 4);
            continue;
        }
        if(token == "load8")
        {
            writeByte(OP_LOAD8);
            m_scanner->nextTokenEOF();
            u32 mpos = lvarMPosFor(m_scanner->getToken(), true, 8);
            write(&mpos, 4);
            continue;
        }
        if(token == "fetch4")
        {
            writeByte(OP_FETCH4);
            m_scanner->nextTokenEOF();
            u32 mpos = lvarMPosFor(m_scanner->getToken(), false, 4);
            write(&mpos, 4);
            continue;
        }
        if(token == "fetch8")
        {
            writeByte(OP_FETCH8);
            m_scanner->nextTokenEOF();
            u32 mpos = lvarMPosFor(m_scanner->getToken(), false, 8);
            write(&mpos, 4);
            continue;
        }
        if(token == "loadwide4")
        {
            writeByte(OP_LOADWIDE4);
            m_scanner->nextTokenEOF();
            u32 mpos = gvarMPosFor(m_scanner->getToken(), true, 4);
            write(&mpos, 4);
            continue;
        }
        if(token == "loadwide8")
        {
            writeByte(OP_LOADWIDE8);
            m_scanner->nextTokenEOF();
            u32 mpos = gvarMPosFor(m_scanner->getToken(), true, 8);
            write(&mpos, 4);
            continue;
        }
        if(token == "fetchwide4")
        {
            writeByte(OP_FETCHWIDE4);
            m_scanner->nextTokenEOF();
            u32 mpos = gvarMPosFor(m_scanner->getToken(), false, 4);
            write(&mpos, 4);
            continue;
        }
        if(token == "fetchwide8")
        {
            writeByte(OP_FETCHWIDE8);
            m_scanner->nextTokenEOF();
            u32 mpos = gvarMPosFor(m_scanner->getToken(), false, 8);
            write(&mpos, 4);
            continue;
        }
        if(token == "alloc") { writeByte(OP_ALLOC); continue; }
        if(token == "free") { writeByte(OP_FREE); continue; }
        if(token == "refl1") { writeByte(OP_REFL1); continue; }
        if(token == "refl2") { writeByte(OP_REFL2); continue; }
        if(token == "refl4") { writeByte(OP_REFL4); continue; }
        if(token == "refl8") { writeByte(OP_REFL8); continue; }
        if(token == "extr1") { writeByte(OP_EXTR1); continue; }
        if(token == "extr2") { writeByte(OP_EXTR2); continue; }
        if(token == "extr4") { writeByte(OP_EXTR4); continue; }
        if(token == "extr8") { writeByte(OP_EXTR8); continue; }
        if(token == "swap4") { writeByte(OP_SWAP4); continue; }
        if(token == "swap8") { writeByte(OP_SWAP8); continue; }
        if(token == "swap48") { writeByte(OP_SWAP48); continue; }
        if(token == "swap84") { writeByte(OP_SWAP84); continue; }
        if(token == "dup4") { writeByte(OP_DUP4); continue; }
        if(token == "dup8") { writeByte(OP_DUP8); continue; }
        if(token == "addi4") { writeByte(OP_ADDI4); continue; }
        if(token == "addi8") { writeByte(OP_ADDI8); continue; }
        if(token == "addf4") { writeByte(OP_ADDF4); continue; }
        if(token == "addf8") { writeByte(OP_ADDF8); continue; }
        if(token == "subi4") { writeByte(OP_SUBI4); continue; }
        if(token == "subi8") { writeByte(OP_SUBI8); continue; }
        if(token == "subf4") { writeByte(OP_SUBF4); continue; }
        if(token == "subf8") { writeByte(OP_SUBF8); continue; }
        if(token == "muli4") { writeByte(OP_MULI4); continue; }
        if(token == "muli8") { writeByte(OP_MULI8); continue; }
        if(token == "mulf4") { writeByte(OP_MULF4); continue; }
        if(token == "mulf8") { writeByte(OP_MULF8); continue; }
        if(token == "divi4") { writeByte(OP_DIVI4); continue; }
        if(token == "divi8") { writeByte(OP_DIVI8); continue; }
        if(token == "divu4") { writeByte(OP_DIVU4); continue; }
        if(token == "divu8") { writeByte(OP_DIVU8); continue; }
        if(token == "divf4") { writeByte(OP_DIVF4); continue; }
        if(token == "divf8") { writeByte(OP_DIVF8); continue; }
        if(token == "remi4") { writeByte(OP_REMI4); continue; }
        if(token == "remi8") { writeByte(OP_REMI8); continue; }
        if(token == "remu4") { writeByte(OP_REMU4); continue; }
        if(token == "remu8") { writeByte(OP_REMU8); continue; }
        if(token == "negi4") { writeByte(OP_NEGI4); continue; }
        if(token == "negi8") { writeByte(OP_NEGI8); continue; }
        if(token == "negf4") { writeByte(OP_NEGF4); continue; }
        if(token == "negf8") { writeByte(OP_NEGF8); continue; }
        if(token == "shl4") { writeByte(OP_SHL4); continue; }
        if(token == "shl8") { writeByte(OP_SHL8); continue; }
        if(token == "shr4") { writeByte(OP_SHR4); continue; }
        if(token == "shr8") { writeByte(OP_SHR8); continue; }
        if(token == "shru4") { writeByte(OP_SHRU4); continue; }
        if(token == "shru8") { writeByte(OP_SHRU8); continue; }
        if(token == "bnot4") { writeByte(OP_BNOT4); continue; }
        if(token == "bnot8") { writeByte(OP_BNOT8); continue; }
        if(token == "band4") { writeByte(OP_BAND4); continue; }
        if(token == "band8") { writeByte(OP_BAND8); continue; }
        if(token == "bxor4") { writeByte(OP_BXOR4); continue; }
        if(token == "bxor8") { writeByte(OP_BXOR8); continue; }
        if(token == "bor4") { writeByte(OP_BOR4); continue; }
        if(token == "bor8") { writeByte(OP_BOR8); continue; }
        if(token == "lnot4") { writeByte(OP_LNOT4); continue; }
        if(token == "lnot8") { writeByte(OP_LNOT8); continue; }
        if(token == "land4") { writeByte(OP_LAND4); continue; }
        if(token == "land8") { writeByte(OP_LAND8); continue; }
        if(token == "lor4") { writeByte(OP_LOR4); continue; }
        if(token == "lor8") { writeByte(OP_LOR8); continue; }
        if(token == "ci14") { writeByte(OP_CI14); continue; }
        if(token == "ci24") { writeByte(OP_CI24); continue; }
        if(token == "ci41") { writeByte(OP_CI41); continue; }
        if(token == "ci42") { writeByte(OP_CI42); continue; }
        if(token == "ci48") { writeByte(OP_CI48); continue; }
        if(token == "ci84") { writeByte(OP_CI84); continue; }
        if(token == "cf48") { writeByte(OP_CF48); continue; }
        if(token == "cf84") { writeByte(OP_CF84); continue; }
        if(token == "cfi4") { writeByte(OP_CFI4); continue; }
        if(token == "cfi8") { writeByte(OP_CFI8); continue; }
        if(token == "cif4") { writeByte(OP_CIF4); continue; }
        if(token == "cif8") { writeByte(OP_CIF8); continue; }
        if(token == "goto")
        {
            writeByte(OP_GOTO);
            m_scanner->nextTokenEOF();
            u32 pos = getLabelPC(name, m_scanner->getToken());
            write(&pos, 4);
            continue;
        }
        if(token == "if") { writeByte(OP_IF); continue; }
        if(token == "ifn") { writeByte(OP_IFN); continue; }
        if(token == "ltnl") { writeByte(OP_LTNL); continue; }
        if(token == "lenl") { writeByte(OP_LENL); continue; }
        if(token == "gtnl") { writeByte(OP_GTNL); continue; }
        if(token == "genl") { writeByte(OP_GENL); continue; }
        if(token == "eqnl") { writeByte(OP_EQNL); continue; }
        if(token == "nenl") { writeByte(OP_NENL); continue; }
        if(token == "cmp4") { writeByte(OP_CMP4); continue; }
        if(token == "cmp8") { writeByte(OP_CMP8); continue; }
        if(token == "icmp4") { writeByte(OP_ICMP4); continue; }
        if(token == "icmp8") { writeByte(OP_ICMP8); continue; }
        if(token == "iucmp4") { writeByte(OP_IUCMP4); continue; }
        if(token == "iucmp8") { writeByte(OP_IUCMP8); continue; }
        if(token == "iucmpr4") { writeByte(OP_IUCMPR4); continue; }
        if(token == "iucmpr8") { writeByte(OP_IUCMPR8); continue; }
        if(token == "fcmp4") { writeByte(OP_FCMP4); continue; }
        if(token == "fcmp8") { writeByte(OP_FCMP8); continue; }
        if(token == "ficmp4") { writeByte(OP_FICMP4); continue; }
        if(token == "ficmp8") { writeByte(OP_FICMP8); continue; }
        if(token == "ficmpr4") { writeByte(OP_FICMPR4); continue; }
        if(token == "ficmpr8") { writeByte(OP_FICMPR8); continue; }
        if(token == "fucmp4") { writeByte(OP_FUCMP4); continue; }
        if(token == "fucmp8") { writeByte(OP_FUCMP8); continue; }
        if(token == "fucmpr4") { writeByte(OP_FUCMPR4); continue; }
        if(token == "fucmpr8") { writeByte(OP_FUCMPR8); continue; }
        if(token == "call")
        {
            writeByte(OP_CALL);
            m_scanner->nextTokenEOF();
            u32 id = functionIDFor(m_scanner->getToken(), false);
            write(&id, 4);
            continue;
        }
        if(token == "return") { writeByte(OP_RETURN); continue; }
        m_log->abort("unrecognized mnemonic \"" + token + "\"");
    }
}
