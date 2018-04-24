/*
    Bernhard Schuster
    Implementation and Evaluation of Approximation Algorithms for Noah's Ark Problems
    Master's Thesis
    University of Vienna
    2017
*/



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
