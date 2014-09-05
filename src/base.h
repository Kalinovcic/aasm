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
 * File: base.h
 * Description: 
 * Author: Lovro Kalinovcic
 * 
 */

#ifndef BASE_H_
#define BASE_H_

#include <cstdlib>
#include <cstdio>
#include <stdint.h>

#include <iostream>
#include <fstream>

#include <map>

#include "translate.h"

typedef uint16_t u16;
typedef uint32_t u32;
typedef uint8_t byte;

std::ifstream* getInputStream();
void error(std::string msg);

void write(void* ptr, size_t size);
void writeByte(byte b);

u16 nextFunctionID();
u16 nextGlobalvarID();
u16 nextLocalvarID();

u16 functionIDFor(std::string name, bool create);
u16 globalvarIDFor(std::string name, bool create);
u16 localvarIDFor(std::string name, bool create);

std::string getToken();
bool nextToken();
std::string nextTokenEOF();

void nativeFunction(std::string name, bool isVoid);
void aspelFunction(std::string name);
void function();

void globalvar();

void init(std::ifstream* in, std::ofstream* out);
void run();

#endif /* BASE_H_ */
