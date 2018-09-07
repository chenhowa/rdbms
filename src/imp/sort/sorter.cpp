


#include "sorter.hpp"
#include "block_buffer.hpp"
#include "file_input_stream.hpp"
#include "file_output_stream.hpp"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <assert.h>
#include <algorithm>
#include <string>
#include <cstring>
#include <cstdio>
#include "file_manager.hpp"
#include <queue>
#include <boost/log/trivial.hpp>

namespace logging = boost::log;

unsigned calcBlockSize() {
    struct stat s;
    stat("/", &s);
    BOOST_LOG_TRIVIAL(trace) << "Blocksize: " << (int)s.st_blksize;
    return (unsigned)(s.st_blksize);
}


Sorter::Sorter(unsigned num_bufs, IBlockBufferFactory fac,
                IFileInputStreamFactory in, IFileOutputStreamFactory out,
                IFileManager *man) {
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
    this->manager = man;
    
    assert(workers.size() == this->_numWorkers);
    assert(in_streams.size() == workers.size());
    assert(out_streams.size() == workers.size());
}

Sorter::Sorter(unsigned num_bufs, unsigned blocksize, IBlockBufferFactory fac,
                    IFileInputStreamFactory in, IFileOutputStreamFactory out,
                    IFileManager* man) {
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
    this->manager = man;
    
    assert(workers.size() == this->_numWorkers);
    assert(in_streams.size() == workers.size());
    assert(out_streams.size() == workers.size());
}

Sorter::Sorter(IBlockBuffer* stage_buffer, 
                            std::vector<IBlockBuffer*> &buffers,
                            std::vector<IFileInputStream*> &readers, 
                            std::vector<IFileOutputStream*> &writers,
                            IFileManager* man) {
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
    
    this->manager = man;
}

unsigned Sorter::getNumWorkers() {
    assert(workers.size() == this->_numWorkers);
    return this->_numWorkers;
}

unsigned Sorter::getBlockSize() {
    return this->blocksize;
}

void Sorter::sort(IInputStream& in, IOutputStream& out) {
    std::queue<std::string> files;
    
    for(unsigned i = 0; i < getNumWorkers(); i++) {
        assert(workers[i]->isEmpty());
        assert(!out_streams[i]->is_open() && out_streams[i]->good());
        assert(!in_streams[i]->is_open() && in_streams[i]->good());
    }
    BOOST_LOG_TRIVIAL(trace) << "Sorting Input Into Sorted Files\n";
    sortInputIntoSortedFiles(in, files);
    
    for(unsigned i = 0; i < getNumWorkers(); i++) {
        assert(workers[i]->isEmpty());
        assert(!out_streams[i]->is_open() && out_streams[i]->good());
        assert(!in_streams[i]->is_open() && in_streams[i]->good());
    }
    
    BOOST_LOG_TRIVIAL(trace) << "Merging sorted files and then writing it\n";
    mergeSortedFilesAndWrite(files, out);
    
    for(unsigned i = 0; i < getNumWorkers(); i++) {
        assert(workers[i]->isEmpty());
        assert(!out_streams[i]->is_open() && out_streams[i]->good());
        assert(!in_streams[i]->is_open() && in_streams[i]->good());
    }
    
    //manager->removeTempFiles();
}

void Sorter::sortInputIntoSortedFiles(IInputStream& in, std::queue<std::string> &files) {
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
        
        assert(workers[nextWorker]->isEmpty());
        
        nextWorker = (nextWorker + 1) % getNumWorkers();
    }
}

void Sorter::writeStageToWorker(unsigned workerIndex) {
    assert(workerIndex < getNumWorkers());
    BOOST_LOG_TRIVIAL(trace) << "Writing stage to worker " << workerIndex;
    this->stage->write( *(this->workers[workerIndex]) );
    BOOST_LOG_TRIVIAL(trace) << "\n";
    assert(this->stage->isEmpty());
}
    
void Sorter::sortWorker(unsigned workerIndex) {
    assert(workerIndex < getNumWorkers());
    IBlockBuffer& worker = *(this->workers[workerIndex]);
    unsigned size = worker.getBlockSize() + 1;
    char* temp = new char[size];
    memset(temp, '\0', size);
    BOOST_LOG_TRIVIAL(trace) << "Worker " << workerIndex << " : Writing for sort: ";
    worker.write(size - 1, temp);
    BOOST_LOG_TRIVIAL(trace) << "\n";
    assert(worker.isEmpty());
    std::string string_to_sort(temp);
    std::sort(string_to_sort.begin(), string_to_sort.end());
    memset(temp, '\0', size);
    std::strcpy(temp, string_to_sort.c_str() );
    worker.read(string_to_sort.length(), temp);
    assert(!worker.isEmpty());
    delete [] temp;
}

void Sorter::writeWorkerToNewTempFileAndStore(unsigned workerIndex, std::queue<std::string> &files) {
    assert(workerIndex < getNumWorkers());
    std::string path = manager->generateName();
    files.push(path);  // stores the filepath for later access to file.    
    IFileOutputStream& writer = *(this->out_streams[workerIndex]);
    IBlockBuffer& worker = *(this->workers[workerIndex]);
    
    BOOST_LOG_TRIVIAL(trace) << "Worker " << workerIndex << ": Writing to temp file " << path.c_str() << " : ";
    writer.open( path, std::ios_base::out );
    assert(writer.is_open());
    assert(writer.good());
    worker.write( writer );
    writer.close();
    BOOST_LOG_TRIVIAL(trace) << "\n";
}

void Sorter::mergeSortedFilesAndWrite(std::queue<std::string> &files, 
                                        IOutputStream &out) {
    BOOST_LOG_TRIVIAL(trace) << "\n***Doing initial merge\n\n";
    doInitialMerge(files);
    
    for(unsigned i = 0; i < getNumWorkers(); i++) {
        assert(workers[i]->isEmpty());
        assert(!out_streams[i]->is_open() && out_streams[i]->good());
        assert(!in_streams[i]->is_open() && in_streams[i]->good());
    }
    
    BOOST_LOG_TRIVIAL(trace) << "Doing final merge\n";
    doFinalMergeAndWrite(files, out);
    
    for(unsigned i = 0; i < getNumWorkers(); i++) {
        assert(workers[i]->isEmpty());
        assert(!out_streams[i]->is_open() && out_streams[i]->good());
        assert(!in_streams[i]->is_open() && in_streams[i]->good());
    }
}

void Sorter::doInitialMerge(std::queue<std::string> &files) {
    while(! (files.size() < getNumWorkers())  ) {
        connectDataToWorkers(files, getNumWorkers());
        
        std::string tmp_path = manager->generateName();
        BOOST_LOG_TRIVIAL(trace) << "Writing to temp file " << tmp_path.c_str() << " : ";
        this->out_streams[0]->open(tmp_path, std::ios_base::out);
        mergeConnectedWorkersAndWrite( *(this->out_streams[0]) );
        disconnectDataFromWorkers();
        
        // Once done writing, push it so it can be read
        // in the next merge run.
        files.push(tmp_path);
        
        this->out_streams[0]->close();
    }
}

void Sorter::connectDataToWorkers(std::queue<std::string> &files, 
                                        unsigned numWorkers) {
    assert(numWorkers <= getNumWorkers() );
    for(unsigned i = 0; i < numWorkers; i++) {
        if(!files.empty()) {
            this->in_streams[i]->open(files.front(), std::ios_base::in);
            BOOST_LOG_TRIVIAL(trace) << "Worker " << i << " connected to " << files.front().c_str() << "\n";
            files.pop();
            assert(this->in_streams[i]->is_open());
            assert(this->in_streams[i]->good());
        }
    }
}

struct MinData {
    char val;
    unsigned index;
};

void Sorter::mergeConnectedWorkersAndWrite(IOutputStream &out) {
    // This function takes connected workers.
    // While there is more data in their associated 
    // input streams, the workers, read data
    // in to full, and then write the min value
    // to stage. If the stage is full,
    // write it. If the minWorker is now empty,
    // fill it.
    while(moreDataRemaining()) {
        readInputIntoEmptyConnectedWorkers();
        while(anyConnectedWorkerHasData()) {
            MinData minData = findMinWorkerData();
            writeDataToStage(minData);
            
            if(workers[minData.index]->isEmpty()) {
                BOOST_LOG_TRIVIAL(trace) << "Reading data to fill empty worker: ";
                workers[minData.index]->read( *(in_streams[minData.index]) );
                BOOST_LOG_TRIVIAL(trace) << "\n";
            }
            
            if(this->stage->isFull()) {
                BOOST_LOG_TRIVIAL(trace) << "Stage is full. Writing: ";
                this->stage->write(out);
                assert(stage->isEmpty());
                BOOST_LOG_TRIVIAL(trace) << "\n";
            }
        }
    }
    
    // If no data is remaining, we must again flush the buffer.
    BOOST_LOG_TRIVIAL(trace) << "\nflushing stage: ";
    this->stage->write(out);
    BOOST_LOG_TRIVIAL(trace) << "\n";
    assert(stage->isEmpty());
    
    /*
    while(moreDataRemaining()) {
        readInputIntoEmptyConnectedWorkers();
        MinData minData = findMinWorkerData();
        
        // THIS ONLY WRITES ONE CHARACTER.
        // SO HOW WILL IT WRITE ALL OF IT BEFORE
        // THE DATA HAS ALL BEEN READ?
        writeDataToStage(minData);
        
        if(this->stage->isFull()) {
            this->stage->write(out);
        }
    }
     * */
}

bool Sorter::moreDataRemaining() {
    bool existsMoreData = false;
    for(unsigned i = 0; i < getNumWorkers(); i++) {
        if( in_streams[i]->is_open() && !in_streams[i]->eof() ) {
            existsMoreData = true;
            break;
        }
    }
    
    return existsMoreData;
}

void Sorter::readInputIntoEmptyConnectedWorkers() {
    for(unsigned i = 0; i < getNumWorkers(); i++) {
        if(workers[i]->isEmpty() && in_streams[i]->is_open()) {
            BOOST_LOG_TRIVIAL(trace) << "!!!!! Reading data from worker " << i;
            workers[i]->read(*(in_streams[i]));
            BOOST_LOG_TRIVIAL(trace) << "\n";
        }
    }
}

bool Sorter::anyConnectedWorkerHasData() {
    bool workerHasData = false;
    for(unsigned i = 0; i < getNumWorkers(); i++) {
        if( in_streams[i]->is_open() && !workers[i]->isEmpty() ) {
            workerHasData = true;
        }
    }
    
    return workerHasData;
}

MinData Sorter::findMinWorkerData() {
    bool found = false;
    MinData min;
    min.val = char(127);
    min.index = getNumWorkers();
    char candidate;
    for(unsigned i = 0; i < getNumWorkers(); i++) {
        IBlockBuffer& worker = *(workers[i]);
        candidate = min.val;
        if(!worker.isEmpty()) {
            candidate = worker.peek();
            BOOST_LOG_TRIVIAL(trace) << "candidate from worker " << i << " : " << candidate << "\n";
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
    BOOST_LOG_TRIVIAL(trace) << "\nMIN FOUND WAS: " << min.val << " from worker : " << min.index << "\n";
    
    return min;
}

void Sorter::writeDataToStage(MinData data) {
    BOOST_LOG_TRIVIAL(trace) << "Writing a datum to stage: ";
    this->workers[data.index]->write(1, &(data.val) );
    BOOST_LOG_TRIVIAL(trace) << "\n";
    this->stage->read(1, &(data.val) );
}


void Sorter::disconnectDataFromWorkers() {
    for(unsigned i = 0; i < getNumWorkers(); i++) {
        IFileInputStream& reader = *( in_streams[i] );
        assert(reader);
        if(reader.is_open()) {
            // If data disconnected, delete the file
            std::string filename = reader.get_file_name();
            
            BOOST_LOG_TRIVIAL(trace) << "Worker " << i << " disconnected\n";
            reader.close();
            
            manager->removeFile(filename);
        }
    }
}

void Sorter::doFinalMergeAndWrite(std::queue<std::string> &files,
                                        IOutputStream &out) {
    BOOST_LOG_TRIVIAL(trace) << "\n \nDOING FINAL MERGE \n \n";
    assert(files.size() < getNumWorkers());
    connectDataToWorkers(files, files.size());
    mergeConnectedWorkersAndWrite( out );
    flushStage(out);
    disconnectDataFromWorkers();
}

void Sorter::flushStage(IOutputStream &out) {
    BOOST_LOG_TRIVIAL(trace) << "flushing stage: ";
    this->stage->write(out);
    BOOST_LOG_TRIVIAL(trace) << "\n";
}

using namespace fruit;
Component<ISorterFactory> getISorterFactory() {
    return createComponent()
        .install(getIBlockBufferFactory)
        .install(getIFileInputStreamFactory)
        .install(getIFileOutputStreamFactory)
        .install(getIFileManager)
        .registerFactory<std::unique_ptr<ISorter>(
                fruit::Assisted<unsigned>, 
                fruit::Assisted<unsigned>, 
                IBlockBufferFactory,
                IFileInputStreamFactory,
                IFileOutputStreamFactory,
                IFileManager*)> (
            [](unsigned num_workers, unsigned blocksize, IBlockBufferFactory fac,
                        IFileInputStreamFactory in, IFileOutputStreamFactory out,
                        IFileManager* man) {
                return std::unique_ptr<ISorter>(
                    new Sorter(num_workers, blocksize, fac, in, out, man)
                );
            });
}
