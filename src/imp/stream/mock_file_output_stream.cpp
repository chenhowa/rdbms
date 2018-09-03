



#include "mock_file_output_stream.hpp"
#include <assert.h>
#include "filesystem.hpp"

MockFileOutputStream::MockFileOutputStream() {
    
}

MockFileOutputStream::MockFileOutputStream(IFileSystem *fs) : 
        current_file(nullptr), filesystem(fs), 
        is_open_flag(false),
        good_bit(true), eof_bit(false),
        fail_bit(false),
        bad_bit(false)
         {
    
}


std::string MockFileOutputStream::getContent() {
    if(current_file != nullptr) {
        return *current_file;
    } else {
        throw "Tried to get content when no file was set";
    }
}

void MockFileOutputStream::setContent(std::string &s) {
    if(current_file != nullptr) {
        *current_file = std::string(s);
    } else {
        throw "Tried to set content when no file was set";
    }
}

void MockFileOutputStream::open(const std::string& filename,
            std::fstream::ios_base::openmode openmode) {
    if(good()) {
        if(is_open_flag) {
            // If already open, operation fails.
            fail_bit = true;
            good_bit = false;
        } else {
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
        // Nothing happens. Non-good streams need to be
        // cleared to good state for this operation to
        // have meaning.
    }
}

bool MockFileOutputStream::isTrunc(std::ios_base::openmode mode) {
    return (mode & std::ios_base::trunc) != 0;
}

bool MockFileOutputStream::isBinary(std::ios_base::openmode mode) {
    return (mode & std::ios_base::binary) != 0;
}

bool MockFileOutputStream::isIn(std::ios_base::openmode mode) {
    return (mode & std::ios_base::in) != 0;
}

bool MockFileOutputStream::isOut(std::ios_base::openmode mode) {
    return (mode & std::ios_base::out) != 0;
}

bool MockFileOutputStream::isAte(std::ios_base::openmode mode) {
    return (mode & std::ios_base::ate) != 0;
}

bool MockFileOutputStream::isApp(std::ios_base::openmode mode) {
    return (mode & std::ios_base::app) != 0;
}

bool MockFileOutputStream::is_open() const {
    //Whether file is in good or bad state, simply check if
    // it has been successfully opened and has not yet been
    // successfully closed.
    return is_open_flag;
}

void MockFileOutputStream::close() {
    if(good()) {
        if(!is_open_flag) {
            // If already closed, operation fails
            fail_bit = true;
            good_bit = false;
        } else {
            is_open_flag = false;
            current_file = nullptr;
        }
    } else {
        // Nothing happens. Non-good streams
        // need to be cleared to good state for
        // this operation to have meaning.
    }
}

IOutputStream& MockFileOutputStream::put(char c) {
    if(current_file != nullptr) {
        if(current_iterator == current_file->end()) {
            // If iterator points to end of file, insert new character
            current_file->insert(current_iterator, c);
            assert(current_iterator == current_file->end());
        } else {
            // If not pointing to end of file, simply overwrite existing
            // characters.
            *current_iterator = c;
            current_iterator++;
        }
    } else {
        //If no current file, cannot put at all.
        good_bit = false;
        fail_bit = true;
    }
    
    return *this;
}

IOutputStream& MockFileOutputStream::write(unsigned n, const char* s) {
    if(current_file != nullptr) {
        if(current_file != nullptr) {
            if(current_iterator == current_file->end()) {
                //If iterator points to end of file, insert characters from c-string
                // at end.
                for(unsigned i = 0; i < n; i++) {
                    current_file->insert(current_iterator, *s);
                    s++;
                    assert(current_iterator == current_file->end());
                }
            } else {
                // If not pointing to end of file, overwrite existing characters
                // until reach end of file, and then start inserting.
                unsigned bytes_remaining = n;
                while(current_iterator != current_file->end() && bytes_remaining > 0) {
                    *current_iterator = *s;
                    s++;
                    bytes_remaining--;
                    current_iterator++;
                }
                
                assert(current_iterator == current_file->end() || n == 0);
                
                while(bytes_remaining > 0 && current_iterator == current_file->end() ) {
                    current_file->insert(current_iterator, *s);
                    s++;
                    bytes_remaining--;
                    
                    assert(current_iterator == current_file->end() );
                }
            }
        }
    } else {
        good_bit = false;
        fail_bit = true;
    }
    return *this;
}

IOutputStream& MockFileOutputStream::flush() {
    // Do nothing. It's all in memory, so there's nothing to flush.
    return *this;
}

bool MockFileOutputStream::good() const {
    // When good bit is set, no others should be.
    assert(good_bit != eof_bit);
    assert(good_bit != fail_bit);
    assert(good_bit != bad_bit);
    
    return good_bit;
}

bool MockFileOutputStream::eof() const {
    assert(good_bit != eof_bit);
    
    return eof_bit;
}

bool MockFileOutputStream::bad() const {
    assert(good_bit != bad_bit);
    return bad_bit;
}

bool MockFileOutputStream::fail() const {
    assert(good_bit != fail_bit);
    return fail_bit || bad_bit;
}

bool MockFileOutputStream::operator! () const {
    return fail();
}

MockFileOutputStream::operator bool() const {
    return !fail();
}

// This function will only be used to clear state of stream to be GOOD
void MockFileOutputStream::clear(std::iostream::iostate state) {
    good_bit = true;
    bad_bit = false;
    fail_bit = false;
    eof_bit = false;
}


using namespace fruit;

Component<IMockFileOutputStreamFactory> getIMockFileOutputStreamFactory() {
    return createComponent()
        .install(getIFileSystem)
        .registerFactory<std::unique_ptr<IMockFileOutputStream>(
            IFileSystem*) >(
            [](IFileSystem* fs) {
                return std::unique_ptr<IMockFileOutputStream>(new MockFileOutputStream(fs) );
            }
        ); 
}

