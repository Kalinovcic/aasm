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

static std::ifstream* in;
static std::ofstream* out;

static std::string token;

static u16 functionIDCounter = 0;
static u16 globalvarIDCounter = 0;
static u16 localvarIDCounter = 0;
static std::map<std::string, u32> functionIDs;
static std::map<std::string, u32> globalvarIDs;
static std::map<std::string, u32> localvarIDs;

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
    out->write((char*) ptr, 1);
}

void writeByte(byte b)
{
    out->write((char*) (&b), 1);
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
    std::cout << "loaded native " << name << "\n";
}

void aspelFunction(std::string name)
{
    localvarIDCounter = 0;
    localvarIDs.clear();

    seekFunction(name);

    std::cout << "loaded aspfun " << name << "\n";
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

    if(isNative) nativeFunction(name, isVoid);
    else aspelFunction(name);
}

void globalvar()
{
    std::string name = nextTokenEOF();

    std::cout << "loaded globalvar " << name << "\n";
}

void init(std::ifstream* _in, std::ofstream* _out)
{
    in = _in;
    out = _out;
}

void run()
{
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
