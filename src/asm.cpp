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

#include <fstream>
#include "base.h"

int main()
{
    std::ifstream in;
    std::ofstream out;

    in.open("../aspelc/test.aml", std::ios::in);
    out.open("../aspelc/test.aby", std::ios::out | std::ios::binary);
    init(&in, &out);
    seek();
    in.close();
    out.close();

    in.open("../aspelc/test.aml", std::ios::in);
    out.open("../aspelc/test.aby", std::ios::out | std::ios::binary);
    init(&in, &out);
    write();
    in.close();
    out.close();


    std::cout << "assembler finished\n";

    return EXIT_SUCCESS;
}
