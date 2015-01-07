#ifndef __EX__
#define __EX__

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
};

#endif
