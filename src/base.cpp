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
 * File: base.cpp
 * Description: 
 * Author: Lovro Kalinovcic
 * 
 */

#include "base.h"

typedef std::map<std::string, u32>::iterator idmap_it;

static std::ifstream* in;
static std::ofstream* out;

static std::string token;

static u32 filepos = 0;

static u16 functionIDCounter = 0;
static u16 globalvarIDCounter = 0;
static u16 localvarIDCounter = 0;
static std::map<std::string, u32> functionIDs;
static std::map<std::string, u32> globalvarIDs;
static std::map<std::string, u32> localvarIDs;

static std::vector<std::string> nativeFunctions;

std::ifstream* getInputStream()
{
    return in;
}

void error(std::string msg)
{
    std::cerr << msg << "\n";
    exit(1);
}

void write(void* ptr, size_t size)
{
    out->write(reinterpret_cast<const char *>(ptr), size);
}

void writeByte(byte b)
{
    out->write(reinterpret_cast<const char *>(&b), 1);
}

u16 nextFunctionID()
{
    functionIDCounter++;
    if(functionIDCounter == 0) error("function ID overflow");
    return functionIDCounter - 1;
}

u16 nextGlobalvarID()
{
    globalvarIDCounter++;
    if(globalvarIDCounter == 0) error("globalvar ID overflow");
    return globalvarIDCounter - 1;
}

u16 nextLocalvarID()
{
    localvarIDCounter++;
    if(localvarIDCounter == 0) error("var ID overflow");
    return localvarIDCounter - 1;
}

bool isNative(std::string name)
{
    return std::find(nativeFunctions.begin(), nativeFunctions.end(), name) != nativeFunctions.end();
}

u16 functionIDFor(std::string name, bool create)
{
    if(functionIDs.find(name) == functionIDs.end())
    {
        if(create) functionIDs[name] = nextFunctionID();
        else error("function \"" + name + "\" not found");
    }

    return functionIDs[name];
}

u16 globalvarIDFor(std::string name, bool create)
{
    if(globalvarIDs.find(name) == globalvarIDs.end())
    {
        if(create) globalvarIDs[name] = nextFunctionID();
        else error("globalvar \"" + name + "\" not found");
    }
    return globalvarIDs[name];
}

u16 localvarIDFor(std::string name, bool create)
{
    if(localvarIDs.find(name) == localvarIDs.end())
    {
        if(create) localvarIDs[name] = nextFunctionID();
        else error("var \"" + name + "\" not found");
    }
    return localvarIDs[name];
}

std::string getToken()
{
    return token;
}

bool nextToken()
{
    *in >> token;
    return !in->eof();
}

std::string nextTokenEOF()
{
    if(!nextToken()) error("unexpected EOF");
    return token;
}

void nativeFunction(std::string name, bool isVoid)
{
    if(isNative(name)) error("duplicate native \"" + name + "\"\n");
    else nativeFunctions.push_back(name);

    std::cout << "seeked native " << name << "\n";
}

void aspelFunction(std::string name)
{
    localvarIDCounter = 0;
    localvarIDs.clear();

    seekFunction(name);

    std::cout << "seeked aspel function " << name << "\n";
}

void function()
{
    if(token.size() > 4) error("invalid function def \"" + token + "\"");

    bool isNative = false, isVoid = false;
    for(unsigned int i = 2; i < token.size(); i++)
        switch(token[i])
        {
        case 'n': if(isNative) error("invalid function def \"" + token + "\""); isNative = true; break;
        case 'v': if(isVoid) error("invalid function def \"" + token + "\""); isVoid = true; break;
        }

    std::string name = nextTokenEOF();

    if(functionIDs.find(name) == functionIDs.end())
        functionIDs[name] = nextFunctionID();
    else error("function \"" + name + "\" redeclared");

    if(isNative) nativeFunction(name, isVoid);
    else aspelFunction(name);
}

void globalvar()
{
    std::string name = nextTokenEOF();
    globalvarIDFor(name, true);

    std::cout << "seeked globalvar " << name << "\n";
}

void writeHeader()
{
    std::cout << "writing header\n";

    writeByte('A');
    writeByte('B');
    writeByte('Y');
    writeByte(27);

    filepos += 4;
}

void writeNativeData()
{
    std::cout << "writing native data\n";

    u16 nativecount = nativeFunctions.size();
    write(&nativecount, 2);

    filepos += 2;

    for(std::vector<std::string>::iterator i = nativeFunctions.begin(); i != nativeFunctions.end(); i++)
    {
        std::string name = *i;
        std::cout << "writing native function \"" << name << "\"\n";

        for(unsigned int i = 0; i < name.size(); i++)
            writeByte(name[i]);
        writeByte(0x00);

        u16 id = functionIDFor(name, false);
        write(&id, 2);

        filepos += name.size() + 3;
    }
}

void writeGlobalvarData()
{
    std::cout << "writing globalvar data\n";

    u16 globalvarc = globalvarIDs.size();
    write(&globalvarc, 2);

    filepos += 2;
}

void writeFunctionData()
{
    std::cout << "writing function data\n";

    u16 functionc = functionIDs.size();
    write(&functionc, 2);

    filepos += 2 + (functionc - nativeFunctions.size())* 6;

    for(idmap_it i = functionIDs.begin(); i != functionIDs.end(); i++)
        if(!isNative(i->first))
        {
            u16 id = i->second;

            write(&id, 2);
            write(&filepos, 4);
            filepos += getFunctionSize(i->first);
        }
}

void writeFunctions()
{
    for(idmap_it i = functionIDs.begin(); i != functionIDs.end(); i++)
        if(!isNative(i->first))
        {
            std::cout << "writing function \"" << i->first << "\"\n";
            translateFunction(i->first);
        }
}

void init(std::ifstream* _in, std::ofstream* _out)
{
    in = _in;
    out = _out;
}

void seek()
{
    std::cout << "seeking...\n";
    while(nextToken())
    {
        if(token.size() < 2 || token[1] != ':')
            error("invalid block def \"" + token + "\"");
        switch(token[0])
        {
        case 'f': function(); break;
        case 'w': globalvar(); break;
        }
    }
}

void write()
{
    std::cout << "writing...\n";

    writeHeader();
    writeNativeData();
    writeGlobalvarData();
    writeFunctionData();
    writeFunctions();
}
