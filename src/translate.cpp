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
 * File: translate.cpp
 * Description: 
 * Author: Lovro Kalinovcic
 * 
 */

#include "translate.h"

#include <stdint.h>

#include <string>
#include <vector>
#include <map>

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

struct FunctionData
{
    std::streampos inpos;
    std::map<std::string, u32> labels;
    u32 size;
};

static u32 pc;
static std::map<std::string, FunctionData> functions;

static bool isBoolean(std::string s)
{
    return s == "true" || s == "false";
}

static bool isInteger(std::string s)
{
    for(unsigned int i = 0; i < s.size(); i++)
        if(s[i] < '0' || s[i] > '9')
            return false;
    return true;
}

static bool isFloat(std::string s)
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

static u32 getLabelPC(std::string functionName, std::string labelName)
{
    FunctionData fdat = functions[functionName];
    if(fdat.labels.find(labelName) == fdat.labels.end())
        error("label \"" + labelName + "\" not found");
    return fdat.labels[labelName];
}

static void addPC(std::string name)
{
    std::string token = getToken();
    if(token[token.size() - 1] == ':')
    {
        functions[name].labels[token.substr(0, token.size() - 1)] = pc;
        return;
    }

    pc++;
    if(token == "push")
    {
        std::string value = nextTokenEOF();
        if(isBoolean(value)) { pc++; }
        else if(isInteger(value)) { pc += 8; }
        else if(isFloat(value)) { pc += 8; }
        else error("unknown push type");
    }
    else
    {
        if(token == "load" || token == "fetch" || token == "fetchwide"
        || token == "if" || token == "ifn" || token == "goto" || token == "call")
        {
            pc += 2;
            if(token == "if" || token == "ifn" || token == "goto" || token == "call")
                pc += 2;
        }
    }
}

static void translate(std::string name)
{
    std::string token = getToken();
    if(token[token.size() - 1] == ':') return;
    if(token == "nop") { writeByte(NOP_CODE); return; }
    if(token == "push")
    {
        std::string strval = nextTokenEOF();
        if(isBoolean(strval)) { writeByte(PUSHB_CODE); writeByte(strval == "true" ? 0x01 : 0x00); return; }
        if(isInteger(strval)) { writeByte(PUSHI_CODE); long value = std::atol(strval.c_str()); write(&value, 8); return; }
        if(isFloat(strval)) { writeByte(PUSHF_CODE); double value = std::atof(strval.c_str()); write(&value, 8); return; }
        error("unknown push type");
    }
    if(token == "load") { writeByte(LOAD_CODE); u16 id = localvarIDFor(nextTokenEOF(), true); write(&id, 2); return; }
    if(token == "loadwide") { writeByte(LOADWIDE_CODE); u16 id = globalvarIDFor(nextTokenEOF(), true); write(&id, 2); return; }
    if(token == "fetch") { writeByte(FETCH_CODE); u16 id = localvarIDFor(nextTokenEOF(), false); write(&id, 2); return; }
    if(token == "fetchwide") { writeByte(FETCHWIDE_CODE); u16 id = globalvarIDFor(nextTokenEOF(), false); write(&id, 2); return; }
    if(token == "add") { writeByte(ADD_CODE); return; }
    if(token == "sub") { writeByte(SUB_CODE); return; }
    if(token == "mul") { writeByte(MUL_CODE); return; }
    if(token == "div") { writeByte(DIV_CODE); return; }
    if(token == "rem") { writeByte(REM_CODE); return; }
    if(token == "neg") { writeByte(NEG_CODE); return; }
    if(token == "not") { writeByte(NOT_CODE); return; }
    if(token == "and") { writeByte(AND_CODE); return; }
    if(token == "or") { writeByte(OR_CODE); return; }
    if(token == "xor") { writeByte(XOR_CODE); return; }
    if(token == "lnot") { writeByte(LNOT_CODE); return; }
    if(token == "land") { writeByte(LAND_CODE); return; }
    if(token == "lor") { writeByte(LOR_CODE); return; }
    if(token == "shl") { writeByte(SHL_CODE); return; }
    if(token == "shr") { writeByte(SHR_CODE); return; }
    if(token == "eq") { writeByte(EQ_CODE); return; }
    if(token == "ne") { writeByte(NE_CODE); return; }
    if(token == "lt") { writeByte(LT_CODE); return; }
    if(token == "le") { writeByte(LE_CODE); return; }
    if(token == "gt") { writeByte(GT_CODE); return; }
    if(token == "ge") { writeByte(GE_CODE); return; }
    if(token == "if") { writeByte(IF_CODE); u32 pc = getLabelPC(name, nextTokenEOF()); write(&pc, 4); return; }
    if(token == "ifn") { writeByte(IFN_CODE); u32 pc = getLabelPC(name, nextTokenEOF()); write(&pc, 4); return; }
    if(token == "goto") { writeByte(GOTO_CODE); u32 pc = getLabelPC(name, nextTokenEOF()); write(&pc, 4); return; }
    if(token == "call")
    {
        writeByte(CALL_CODE);
        u16 id = functionIDFor(nextTokenEOF(), false);
        u8 argc = (u8) std::atoi(nextTokenEOF().c_str());
        write(&id, 2);
        write(&argc, 1);
        return;
    }
    if(token == "return") { writeByte(RETURN_CODE); return; }
    error("unrecognized mnemonic \"" + token + "\"");
}

u32 getFunctionSize(std::string name)
{
    return functions[name].size;
}

void seekFunction(std::string name)
{
    pc = 0;
    functions[name].inpos = getInputStream()->tellg();
    functions[name].labels.clear();

    while(true)
    {
        if(nextTokenEOF() == ".") break;
        addPC(name);
    }

    functions[name].size = pc;
}

void translateFunction(std::string name)
{
    getInputStream()->seekg(functions[name].inpos);
    while(true)
    {
        if(nextTokenEOF() == ".") break;
        translate(name);
    }
}
