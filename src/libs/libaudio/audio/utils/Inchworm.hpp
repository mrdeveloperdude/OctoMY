#ifndef INCHWORM_HPP
#define INCHWORM_HPP

#include <atomic>
#include <QtGlobal>

template <size_t SIZE, typename T>
struct Inchworm{

    // The indices of the next read/write operations. read == write implies that buffer is empty.
    std::atomic<qint64> readHead{ 0 };
    std::atomic<qint64> writeHead{ 0 };
	T buf[SIZE]{0};
	size_t bytes{sizeof(T)};

    static inline qint64 modulo(qint64 i) {
        return (SIZE + i) % SIZE;
    }

    // How many bytes can we read before reaching write head of ring buffer?
    inline qint64 readLen(){
        if(readHead<writeHead){
            return readHead+(SIZE-writeHead);
        }
        else{
            return writeHead-readHead;
        }
    }
    // How many bytes can we write before reaching read head of ring buffer?
    inline qint64 writeLen(){
        if(writeHead<readHead){
            return readHead-writeHead;
        }
        else{
            return (SIZE-writeHead)+readHead;
        }
    }

    inline qint64 write(const T *data, qint64 maxlen) {
        qint64 r = readHead;
        qint64 w = writeHead;
        qint64 len=0;
        for(;len<maxlen;++len) {
            buf[w] = data[len];
            w = modulo(w + 1);
            if (w == r) break;
        }
        writeHead = w;
        return len;
    }

    inline qint64 read(T *data, qint64 maxlen) {
        qint64 r = readHead;
        qint64 w = writeHead;
        qint64 len=0;
        for(;len<maxlen;++len) {
            // Note when  r==w the buffer is empty, in write this means we have a lot of space, but for read it means 0 space
            if (w == r) break;
            r = modulo(r + 1);
            data[len] = buf[r];
        }
        readHead = r;
        return len;
    }

    inline qint64 readData(char *data, qint64 maxlen){
        auto s=reinterpret_cast<T *>(data);
        return read(s, maxlen/sizeof(T));
    }
    inline qint64 writeData(const char *data, qint64 maxlen){
        auto s=reinterpret_cast<const T *>(data);
        return write(s, maxlen/sizeof(T));
    }

};




#endif // INCHWORM_HPP
