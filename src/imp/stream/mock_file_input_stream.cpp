



#include "mock_file_input_stream.hpp"
#include <assert.h>
#include "filesystem.hpp"

extern IFileSystem *g_filesystem;

MockFileInputStream::MockFileInputStream() :
    current_file(nullptr), filesystem(g_filesystem),
    is_open_flag(false), good_bit(true),
    eof_bit(false), fail_bit(false),
    bad_bit(false)
                {
    
}

MockFileInputStream::MockFileInputStream(IFileSystem *fs) : 
    current_file(nullptr), filesystem(fs),
    is_open_flag(false), good_bit(true),
    eof_bit(false), fail_bit(false),
    bad_bit(false)
                    {
}

void MockFileInputStream::setContent(const std::string &s) { 
    if(current_file != nullptr) {
        *current_file = std::string(s);
        
        if (isTrunc(mode) || isAte(mode) || isApp(mode)) {
            current_iterator = current_file->end();
        } else {
            // Other modes default to reading from front.
            current_iterator = current_file->begin();
        }
        
    } else {
        throw "Tried to set content when no file was set";
    }
}

bool MockFileInputStream::isTrunc(std::ios_base::openmode mode) {
    return (mode & std::ios_base::trunc) != 0;
}

bool MockFileInputStream::isBinary(std::ios_base::openmode mode) {
    return (mode & std::ios_base::binary) != 0;
}

bool MockFileInputStream::isIn(std::ios_base::openmode mode) {
    return (mode & std::ios_base::in) != 0;
}

bool MockFileInputStream::isOut(std::ios_base::openmode mode) {
    return (mode & std::ios_base::out) != 0;
}

bool MockFileInputStream::isAte(std::ios_base::openmode mode) {
    return (mode & std::ios_base::ate) != 0;
}

bool MockFileInputStream::isApp(std::ios_base::openmode mode) {
    return (mode & std::ios_base::app) != 0;
}

std::string MockFileInputStream::getContent() { 
    if(current_file != nullptr) {
        return *current_file;
    } else {
        throw "Tried to get content when no file was set";
    }
}

void MockFileInputStream::open(const std::string& filename,
            std::fstream::ios_base::openmode openmode) {
    if(good()) {
        if(is_open_flag || !filesystem->fileExists(filename) ) {
            // If already open, operation fails.
            // If file doesn't exist, operation fails.
            fail_bit = true;
            good_bit = false;
        } else {
            assert(filesystem->fileExists(filename));
            // open in correct mode.
            is_open_flag = true;
            current_file = &( (*filesystem) [filename] );
            
            if(isTrunc(openmode)) {
                //truncate
                std::string empty = std::string();
                setContent( empty );
            }
            
            if (isTrunc(openmode) || isAte(openmode) || isApp(openmode)) {
                current_iterator = current_file->end();
            } else {
                // Other modes default to writing to front.
                current_iterator = current_file->begin();
            }
            
            //Store open mode in case it is needed later.
            this->mode = openmode;
        }
    } else {
        // Do nothing. Can't open if not good.
    }
}

bool MockFileInputStream::is_open() const {
    //Whether file is in good or bad state, simply check if
    // it has been successfully opened and has not yet been
    // successfully closed.
    return this->is_open_flag;
}

void MockFileInputStream::close() {
    if(!fail()) {
        if(!is_open_flag) {
            // If already closed, operation fails
            fail_bit = true;
            good_bit = false;
        } else {
            is_open_flag = false;
            current_file = nullptr;
            good_bit = true;
            fail_bit = false;
            eof_bit = false;
            bad_bit = false;
        }
    } else {
        // Nothing happens. Non-good streams
        // need to be cleared to good state for
        // this operation to have meaning.
    }
}

char MockFileInputStream::get() {
    if(current_file != nullptr) {
        if(current_iterator == current_file->end()) {
            // If iterator points to end of file, return EOF
            assert(current_iterator == current_file->end());
            eof_bit = true;
            good_bit = false;
            return char(-1);
        } else {
            // If not pointing to end of file, return single
            // char that is being pointed to.
            char c = *current_iterator;
            current_iterator++;
            return c;
        }
    } else {
        //If no current file, cannot read at all.
        good_bit = false;
        fail_bit = true;
        return char(-1);
    }
}

char MockFileInputStream::peek() {
    if(current_file != nullptr) {
        if(current_iterator == current_file->end()) {
            // If iterator points to end of file, return EOF
            assert(current_iterator == current_file->end());
            good_bit = false;
            eof_bit = true;
            return char(-1);
        } else {
            // If not pointing to end of file, return single
            // char that is being pointed to.
            char c = *current_iterator;
            return c;
        }
    } else {
        //If no current file, cannot read at all.
        good_bit = false;
        fail_bit = true;
        return char(-1);
    }
}

IInputStream& MockFileInputStream::ignore( unsigned n, char delim) {
    unsigned i = 0;
    while(i < n && (*current_iterator) != delim && current_iterator != current_file->end()) {
        current_iterator++;
        i++;
    }
    
    if(i < n && (*current_iterator) != delim) {
        good_bit = false;
        eof_bit = true;
    }
    
    return *this;
}

IInputStream& MockFileInputStream::read(unsigned n, char* s) {
    unsigned i = 0;
    while(i < n && current_iterator != current_file->end()) {
        *s = *current_iterator;
        current_iterator++;
        s++;
        i++;
    }
    
    if(i < n) {
        good_bit = false;
        eof_bit = true;
    }
    
    return *this;
}

bool MockFileInputStream::good() const {
    // When good bit is set, no others should be.
    if(good_bit) {
        assert(good_bit != eof_bit);
        assert(good_bit != fail_bit);
        assert(good_bit != bad_bit);
    }
    
    return good_bit;
}

bool MockFileInputStream::eof() const { 
    if(good_bit) {
        assert(good_bit != eof_bit);
    }
    
    return eof_bit;
}

bool MockFileInputStream::bad() const {
    if(good_bit) {
        assert(good_bit != bad_bit);
    }
    return bad_bit;
}

bool MockFileInputStream::fail() const {
    if(good_bit) {
        assert(good_bit != fail_bit);
    }
    return fail_bit || bad_bit;
}

bool MockFileInputStream::operator! () const {
    return fail();
}

MockFileInputStream::operator bool() const { 
    return !fail();
}

// This function will only be used to clear state of stream to be GOOD
void MockFileInputStream::clear(std::iostream::iostate state) {
    good_bit = true;
    bad_bit = false;
    fail_bit = false;
    eof_bit = false;
}

IFileSystem* MockFileInputStream::getFileSystem() {
    return filesystem;
}

void MockFileInputStream::setFileSystem(IFileSystem *fs) {
    filesystem = fs;
}

using namespace fruit;

Component<IMockFileInputStreamFactory> getIMockFileInputStreamFactory() {
    return createComponent()
        .registerFactory<std::unique_ptr<IMockFileInputStream>() >(
            []() {
                return std::unique_ptr<IMockFileInputStream>(new MockFileInputStream() );
            }
        ); 
}

Component<Required<IFileSystem>, 
        IMockFileInputStreamFactory>  getIMockFileInputStreamFactory_req_fs() {
    return createComponent()
        .registerFactory<std::unique_ptr<IMockFileInputStream>(
            IFileSystem*) >(
            [](IFileSystem* fs) {
                return std::unique_ptr<IMockFileInputStream>(new MockFileInputStream(fs) );
            }
        );
}

