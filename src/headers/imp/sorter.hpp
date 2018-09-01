


#ifndef FILE_SORT_HPP
#define FILE_SORT_HPP

#include "i_sorter.hpp"
#include "i_block_buffer.hpp"
#include <fruit/fruit.h>
#include <vector>


using IBlockBufferFactory = std::function<std::unique_ptr<IBlockBuffer>(unsigned)>;

class Sorter : public ISorter {
private:
    Sorter();
    unsigned blocksize;
    unsigned _numWorkers;
    std::unique_ptr<IBlockBuffer> stage;
    std::vector<std::unique_ptr<IBlockBuffer> > workers;
public:
    Sorter(unsigned num_bufs, IBlockBufferFactory fac);
    Sorter(unsigned num_bufs, unsigned blocksize, IBlockBufferFactory fac);
    virtual unsigned getNumWorkers();
    virtual unsigned getBlockSize();
    virtual void sort(IInputStream& in, IOutputStream& out) override;
    virtual ~Sorter() { };
};


using ISorterFactory = std::function<std::unique_ptr<ISorter>(unsigned, unsigned) >;
fruit::Component<ISorterFactory> getISorterFactory();


#endif //FILE_SORT_HPP
