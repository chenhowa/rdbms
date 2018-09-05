


#include "sorter.hpp"
#include "block_buffer.hpp"
#include "file_input_stream.hpp"
#include "file_output_stream.hpp"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <assert.h>
#include <boost/filesystem.hpp>
#include <algorithm>
#include <string>
#include <cstring>
#include <cstdio>

using namespace boost;

unsigned calcBlockSize() {
    struct stat s;
    stat("/", &s);
    printf("Blocksize: %i\n", (int)s.st_blksize);
    return (unsigned)(s.st_blksize);
}


Sorter::Sorter(unsigned num_bufs, IBlockBufferFactory fac,
                IFileInputStreamFactory in, IFileOutputStreamFactory out) {
    this->blocksize = calcBlockSize();
    this->_numWorkers = num_bufs;
    this->stage = fac(this->blocksize);
    for(unsigned i = 0; i < this->_numWorkers; i++) {
        workers.push_back(fac(this->blocksize));
        in_streams.push_back(in());
        out_streams.push_back(out());
    }
    
    this->in_fac = in;
    this->out_fac = out;
    
    assert(workers.size() == this->_numWorkers);
    assert(in_streams.size() == workers.size());
    assert(out_streams.size() == workers.size());
}

Sorter::Sorter(unsigned num_bufs, unsigned blocksize, IBlockBufferFactory fac,
                    IFileInputStreamFactory in, IFileOutputStreamFactory out) {
    this->blocksize = blocksize;
    this->_numWorkers = num_bufs;
    this->stage = fac(this->blocksize);
    for(unsigned i = 0; i < this->_numWorkers; i++) {
        workers.push_back(fac(this->blocksize));
        in_streams.push_back(in());
        out_streams.push_back(out());
    }
    
    this->in_fac = in;
    this->out_fac = out;
    
    assert(workers.size() == this->_numWorkers);
    assert(in_streams.size() == workers.size());
    assert(out_streams.size() == workers.size());
}

Sorter::Sorter(IBlockBuffer* stage_buffer, 
                            std::vector<IBlockBuffer*> &buffers,
                            std::vector<IFileInputStream*> &readers, 
                            std::vector<IFileOutputStream*> &writers ) {
    assert(buffers.size() >= 2);
    assert(buffers.size() == readers.size());
    assert(buffers.size() == writers.size());
    
    this->blocksize = 0;
    this->_numWorkers = buffers.size();
    this->stage.reset(stage_buffer);
    
    for(unsigned i = 0; i < buffers.size(); i++) {
        workers.push_back(std::unique_ptr<IBlockBuffer>(buffers[i]) );
        in_streams.push_back(std::unique_ptr<IFileInputStream>(readers[i]) );
        out_streams.push_back(std::unique_ptr<IFileOutputStream>(writers[i]) );
    }
}

unsigned Sorter::getNumWorkers() {
    assert(workers.size() == this->_numWorkers);
    return this->_numWorkers;
}

unsigned Sorter::getBlockSize() {
    return this->blocksize;
}

void Sorter::sort(IInputStream& in, IOutputStream& out) {
    std::vector<std::string> files;
    
    sortInputIntoSortedFiles(in, files);
    
    for(unsigned i = 0; i < getNumWorkers(); i++) {
        assert(workers[i]->isEmpty());
        assert(!writers[i]->is_open() && writers[i]->good());
        assert(!readers[i]->is_open() && readers[i]->good());
    }
    
    mergeSortedFilesAndWrite(files, out);
    
    for(unsigned i = 0; i < getNumWorkers(); i++) {
        assert(workers[i]->isEmpty());
        assert(!writers[i]->is_open() && writers[i]->good());
        assert(!readers[i]->is_open() && readers[i]->good());
    }
}

void Sorter::sortInputIntoSortedFiles(IInputStream& in, std::vector<std::string> &files) {
    unsigned nextWorker = 0;
    while(in.good()) {
        // read in next batch of bytes.
        unsigned bytes_read = this->stage->read(in);
        if(bytes_read == 0) {
            // If didn't read any bytes, we are
            // done; EOF has been reached.
            break;
        }
        
        writeStageToWorker(nextWorker);
        sortWorker(nextWorker);
        writeWorkerToNewTempFileAndStore(nextWorker, files);
        
        nextWorker = (nextWorker + 1) % getNumWorkers();
    }
}

void Sorter::writeStageToWorker(unsigned workerIndex) {
    assert(workerIndex < getNumWorkers());
    this->stage->write( *(this->workers[workerIndex]) );
}
    
void Sorter::sortWorker(unsigned workerIndex) {
    assert(workerIndex < getNumWorkers());
    IBlockBuffer* worker = this->workers[workerIndex];
    unsigned size = worker->getBlockSize() + 1;
    char* temp = new char[size];
    memset(temp, '\0', size);
    worker->write(size - 1, temp);
    assert(worker->isEmpty());
    std::string string_to_sort(temp);
    std::sort(string_to_sort.begin(), string_to_sort.end());
    memset(temp, '\0', size);
    std::strcpy(temp, string_to_sort.c_str() );
    worker->read(sort_stage.length(), temp);
    delete [] temp;
}

void Sorter::writeWorkerToNewTempFileAndStore(unsigned workerIndex, std::vector<std::string> &files) {
    assert(workerIndex < getNumWorkers());
    std::string path = filesystem::unique_path().string();
    files.push_back(path);  // stores the filepath for later access to file.
    generateTempFile(path);
    
    IOutputStream* writer = this->writers[workerIndex];
    IBlockBuffer* worker = this->workers[workerIndex];
    
    writer->open( path );
    worker->write( *writer );
    writer->close();
}

void Sorter::mergeSortedFilesAndWrite(std::vector<std::string> &files, 
                                        IOutputFile &out) {
    doInitialMerge(files);
    doFinalMergeAndWrite(out);
}

void Sorter::doInitialMerge(std::vector<std::string> &files) {
    while(!  files.size() < getNumWorkers()) {
        connectDatatoWorkers(files, getNumWorkers());
        
        std::string tmp_path = filesystem::unique_path().string();
        files.push_back(path);
        generateTempFile(tmp_path);
        this->writers[0]->open(tmp_path);
        mergeConnectedWorkersAndWrite( *(this->writers[0]) );
        disconnectDataFromWorkers();
        
        this->writers[0]->close();
    }
}

void Sorter::connectDataToWorkers(std::vector<std::string> &files, 
                                        unsigned numWorkers) {
    for(unsigned i = 0; i < numWorkers; i++) {
        this->readers[i]->open(files.back());
        files.pop_back();
        assert(this->readers[i]->is_open());
        assert(this->readers[i]->good());
        this->readers[i]->close();
    }
}

struct MinData {
    char val;
    unsigned index;
    // bool found;
};

void Sorter::mergeConnectedWorkersAndWrite(IOutputStream &out) {
    while(moreDataRemaining()) {
        readInputIntoEmptyConnectedWorkers();
        MinData minData = findMinWorkerData();
        writeDataToStage(minData);
        
        if(this->stage->isFull()) {
            this->stage->write(out);
        }
    }
}

bool Sorter::moreDataRemaining() {
    bool existsMoreData = false;
    for(unsigned i = 0; i < getNumWorkers(); i++) {
        if( !readers[i]->eof() ) {
            existsMoreData = true;
            break;
        }
    }
    
    return existsMoreData;
}

void Sorter::readInputIntoEmptyConnectedWorkers() {
    for(unsigned i = 0; i < getNumWorkers(); i++) {
        if(workers[i]->isEmpty() && readers[i]->is_open()) {
            workers[i]->read(*(readers[i]));
        }
    }
}

MinData Sorter::findMinWorkerData() {
    bool found = false;
    MinData min;
    min.val = char(255);
    min.index = getNumWorkers();
    char candidate;
    for(unsigned i = 0; i < getNumWorkers(); i++) {
        IBlockBuffer* worker = workers[i];
        if(!worker->isEmpty()) {
            candidate = worker->peek();
        }
        if( candidate < min.val) {
            found = true;
            min.index = i;
            min.val = candidate;
        }
    }
    
    if(!found) {
        throw "Somehow no min was found while merging";
    }
    
    return min;
}

void Sorter::writeDataToStage(MinData data) {
    this->workers[data.index]->write(1, &(data.val) );
    this->stage->read(1, &(data.val) );
}


void Sorter::disconnectDataFromWorkers() {
    for(unsigned i = 0; i < getNumWorkers(); i++) {
        IInputStream* reader = readers[i];
        assert(*reader);
        if(reader->is_open()) {
            reader->close();
        }
    }
}

void Sorter::doFinalMergeAndWrite(std::vector<std::string> &files,
                                        IOutputFile &out) {
    assert(files.size() < getNumWorkers());
    connectDataToWorkers(files, files.size());
    mergeConnectedWorkersAndWrite( out );
    disconnectDataFromWorkers();
}

using namespace fruit;
Component<ISorterFactory> getISorterFactory() {
    return createComponent()
        .install(getIBlockBufferFactory)
        .install(getIFileInputStreamFactory)
        .install(getIFileOutputStreamFactory)
        .registerFactory<std::unique_ptr<ISorter>(
                fruit::Assisted<unsigned>, 
                fruit::Assisted<unsigned>, 
                IBlockBufferFactory,
                IFileInputStreamFactory,
                IFileOutputStreamFactory)> (
            [](unsigned num_workers, unsigned blocksize, IBlockBufferFactory fac,
                        IFileInputStreamFactory in, IFileOutputStreamFactory out) {
                return std::unique_ptr<ISorter>(
                    new Sorter(num_workers, blocksize, fac, in, out)
                );
            });
}
