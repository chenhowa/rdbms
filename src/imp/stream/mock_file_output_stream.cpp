



#include "mock_file_output_stream.hpp"
#include <assert.h>
#include "filesystem.hpp"

extern IFileSystem *g_filesystem;

MockFileOutputStream::MockFileOutputStream() :
        current_file(nullptr), filesystem(g_filesystem), 
        is_open_flag(false),
        good_bit(true), eof_bit(false),
        fail_bit(false),
        bad_bit(false)
                {
    
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
        
        if (isTrunc(mode) || isAte(mode) || isApp(mode)) {
            current_iterator = current_file->end();
        } else {
            // Other modes default to writing to front.
            current_iterator = current_file->begin();
        }
        
    } else {
        throw "Tried to set content when no file was set";
    }
}

void MockFileOutputStream::open(const std::string& filename,
            std::fstream::ios_base::openmode openmode) {
    if(good()) {
        if(is_open_flag ) {
            // If already open, operation fails.
            fail_bit = true;
            good_bit = false;
        } else {
            // if file does not exist, create it.
            if(!filesystem->fileExists(filename)) {
                filesystem->createFile(filename);
            }
            
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
    if( (current_file != nullptr) && good() ) {
        //printf("Trying to put char %c in string %s\n", c, current_file->c_str());
        //printf("\n****FILE CAPACITY IS %u ***\n\n", current_file->capacity());
        if(current_iterator == current_file->end()) {
            // If iterator points to end of file, insert new character
            *current_file = *current_file + c;
            current_iterator = std::string::iterator( current_file->end() );
            
            /*
            //printf("Before insert: %c", *current_iterator);
            current_file->insert(current_iterator, c);
            //printf("After insert: %c in string %s", *current_iterator, current_file->c_str());
            
            //printf("\n\n**NOT EQUAL**\n\n");
            current_iterator++;
            
            //printf("\n****NEW FILE CAPACITY IS %u ***\n\n", current_file->capacity());
            */
            
            //printf("\n****NEW FILE CAPACITY IS %u ***\n\n", current_file->capacity());

            
            //printf("After increment: %c", *current_iterator);
            assert(current_iterator == current_file->end());
        } else {
            // If not pointing to end of file, simply overwrite existing
            // characters.
            *current_iterator = c;
            current_iterator++;
        }
        
        //printf("Contents of file are now: %s\n", current_file->c_str());
    } else {
        //If no current file, cannot put at all.
        good_bit = false;
        fail_bit = true;
    }
    
    return *this;
}

IOutputStream& MockFileOutputStream::write(unsigned n, const char* s) {
    if(current_file != nullptr) {
        if(current_iterator == current_file->end() && good()) {
            //If iterator points to end of file, insert characters from c-string
            // at end.
            for(unsigned i = 0; i < n; i++) {
                current_file->insert(current_iterator, *s);
                s++;
                current_iterator++;
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
                current_iterator++;
                bytes_remaining--;
                
                assert(current_iterator == current_file->end() );
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
    if(good_bit) {
        assert(good_bit != eof_bit);
        assert(good_bit != fail_bit);
        assert(good_bit != bad_bit);
    }
    
    return good_bit;
}

bool MockFileOutputStream::eof() const {
    if(good_bit) {
        assert(good_bit != eof_bit);
    }
    
    return eof_bit;
}

bool MockFileOutputStream::bad() const {
    if(good_bit) {
        assert(good_bit != bad_bit);
    }
    return bad_bit;
}

bool MockFileOutputStream::fail() const {
    if(good_bit) {
        assert(good_bit != fail_bit);
    }
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

IFileSystem* MockFileOutputStream::getFileSystem() {
    return filesystem;
}

void MockFileOutputStream::setFileSystem(IFileSystem *fs) {
    filesystem = fs;
}


using namespace fruit;

Component<IMockFileOutputStreamFactory> getIMockFileOutputStreamFactory() {
    return createComponent()
        .registerFactory<std::unique_ptr<IMockFileOutputStream>() >(
            []() {
                return std::unique_ptr<IMockFileOutputStream>(new MockFileOutputStream() );
            }
        ); 
}

Component<Required<IFileSystem>, 
        IMockFileOutputStreamFactory>  getIMockFileOutputStreamFactory_req_fs() {
    return createComponent()
        .registerFactory<std::unique_ptr<IMockFileOutputStream>(
            IFileSystem*) >(
            [](IFileSystem* fs) {
                return std::unique_ptr<IMockFileOutputStream>(new MockFileOutputStream(fs) );
            }
        ); 
}

/*
Component<IFileOutputStreamFactory> getIFileOutputStream() {
    return createComponent()
        .install(getIFileSystem)
        .registerFactory<std::unique_ptr<IFileOutputStream>(
            IFileSystem*) > (
            [](IFileSystem* fs) {
                return std::unique_ptr<IFileOutputStream>(new MockFileOutputStream(fs)) ;
            }
        );
}*/



