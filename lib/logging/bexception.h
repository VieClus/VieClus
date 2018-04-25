/******************************************************************************
 * bexception.h
 *
 * Source of VieClus -- Vienna Graph Clustering 
 ******************************************************************************
 * Copyright (C) 2017 Sonja Biedermann, Christian Schulz and Bernhard Schuster
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 *****************************************************************************/



#ifndef BEXCEPTION_H
#define BEXCEPTION_H

#include <exception>
#include <string>


/**
    Own class for exception that contains a string message.
*/
class BException : public std::exception
{
    public:
        BException(const std::string &msg, const std::string &file,
                   const unsigned long line, const std::string &func);
        virtual ~BException() throw();      // need to add the deprecated "throw()" as the base class has it too

        virtual const char *what() const throw();   // need to add the deprecated "throw()" as the base class has it too
    protected:
        std::string m_msg;  /**< Error message. */

    private:
};

#endif // BEXCEPTION_H
