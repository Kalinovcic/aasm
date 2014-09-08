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
 * File: log.h
 * Description: 
 * Author: Lovro Kalinovcic
 * 
 */

#ifndef LOG_H_
#define LOG_H_

#include <cstdlib>
#include <ostream>

class Log
{
public:
    enum Level
    {
        INFO,
        WARNING,
        ERROR,

        LEVELC
    };

    Log() { for(int i = 0; i < LEVELC; i++) m_isMuted[i] = false; }
    virtual ~Log() {}

    inline std::ostream* getStream(Level level)
    {
        levelInBounds(level);
        return m_stream[level];
    }

    inline void setStream(std::ostream* stream, Level level)
    {
        levelInBounds(level);
        m_stream[level] = stream;
    }

    inline bool isMuted(Level level)
    {
        levelInBounds(level);
        return m_isMuted[level];
    }

    inline void setMuted(bool muted, Level level)
    {
        levelInBounds(level);
        m_isMuted[level] = muted;
    }

    inline void log(std::string message, Level level)
    {
        levelInBounds(level);
        if(!m_isMuted[level])
            *(m_stream[level]) << message;
    }

    inline void info(std::string message) { log(message + "\n", INFO); }
    inline void warning(std::string message) { log(message + "\n", WARNING); }
    inline void error(std::string message) { log(message + "\n", ERROR); }
    inline void abort(std::string message) { error(message); error("abort"); exit(1); }
private:
    std::ostream* m_stream[LEVELC];
    bool m_isMuted[LEVELC];

    inline void levelInBounds(Level level)
    {
        if(level < 0 || level >= LEVELC)
            abort("invalid log level");
    }
};

#endif /* LOG_H_ */
