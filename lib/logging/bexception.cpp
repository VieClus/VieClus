/*
    Bernhard Schuster
    Implementation and Evaluation of Approximation Algorithms for Noah's Ark Problems
    Master's Thesis
    University of Vienna
    2017
*/



#include "bexception.h"
#include <sstream>
#include <iostream>

using namespace std;


BException::BException(const std::string &msg, const std::string &file,
                       const unsigned long line, const std::string &func)
{
    ostringstream ss;
    ss << file <<  ":" << line << ": " << func << "(): " << msg;
    m_msg = ss.str();
}


BException::~BException() throw()
{

}


const char *BException::what() const throw()
{
    return m_msg.c_str();
}
