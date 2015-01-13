#ifndef __EX__
#define __EX__
// ex.h includes all the classes related
// to exceptions

#include <iostream>

// Exception classes
namespace ex {
    class OutOfBounds: public std::exception{
        public:
            const char* what() const throw() {
                return "Out of Bounds!";
            }
    };
    class DimensionMismatch: public std::exception{
        public:
            const char* what() const throw() {
                return "Dimension Mismatch!";
            }
    };
    class DivideByZero: public std::exception{
        public:
            const char* what() const throw() {
                return "Divide by Zero!";
            }
    };
    // Meant for debugging only
    class Wtf: public std::exception{
        public:
            const char* what() const throw() {
                return "What the fcuk!";
            }
    };

};

#endif
