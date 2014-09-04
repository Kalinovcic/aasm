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
 * File: asm.cpp
 * Description: 
 * Author: Lovro Kalinovcic
 * 
 */

#include <cstdlib>
#include <cstdio>
#include <stdint.h>

#include <iostream>
#include <fstream>

#include <map>

typedef uint32_t u32;

std::ifstream in;
std::ofstream out;

std::string token;

u32 functionIDCounter = 0;
u32 globalvarIDCounter = 0;
u32 localvarIDCounter = 0;
std::map<std::string, u32> functionIDs;
std::map<std::string, u32> globalvarIDs;
std::map<std::string, u32> localvarIDs;

inline void error(std::string msg)
{
    std::cerr << msg << "\n";
    exit(1);
}

inline u32 nextFunctionID()
{
    functionIDCounter++;
    if(functionIDCounter == 0) error("function ID overflow");
    return functionIDCounter - 1;
}

inline u32 nextGlobalvarID()
{
    globalvarIDCounter++;
    if(globalvarIDCounter == 0) error("globalvar ID overflow");
    return globalvarIDCounter - 1;
}

inline u32 nextLocalvarID()
{
    localvarIDCounter++;
    if(localvarIDCounter == 0) error("var ID overflow");
    return localvarIDCounter - 1;
}

inline bool nextToken()
{
    in >> token;
    return !in.eof();
}

inline std::string nextTokenEOF()
{
    if(!nextToken()) error("unexpected EOF");
    return token;
}

inline void nativeFunction(std::string name, bool isVoid)
{
    std::cout << "loaded native " << name << "\n";
}

inline void aspelFunction(std::string name)
{
    std::cout << "loaded aspfun " << name << "\n";
}

inline void function()
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

inline void globalvar()
{
    std::string name = nextTokenEOF();

    std::cout << "loaded globalvar " << name << "\n";
}

int main()
{
    in.open("../aspelc/test.aml", std::ios::in);
    out.open("../aspelc/test.aby", std::ios::out | std::ios::binary);

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

    return EXIT_SUCCESS;
}
