



#ifndef I_SORTER_HPP
#define I_SORTER_HPP

#include "i_input_stream.hpp"
#include "i_output_stream.hpp"

class ISorter {
public:
    virtual void sort(IInputStream& in, IOutputStream& out) = 0;
    virtual ~ISorter() { };
};


#endif // I_SORTER_HPP