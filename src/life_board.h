#pragma once

#include "thread_loop.h"


class LifeBoard {
private:
    size_t _w;
    size_t _h;
    size_t _sz;

    uint8_t* _primary;
    uint8_t* _secondary;

    ThreadLoopPtrs _threads;

    void cycle(size_t off_from, size_t off_to) {
        for(size_t off = off_from; off < off_to; ++off) {
            uint8_t lb = _primary[off];
            size_t n = neighbors(off);

            if(lb > 0x80) {
                if(n < 3 || n > 4)
                    _secondary[off] = 0x80;
                else
                    _secondary[off] = lb - 1;
            } else {
                if(n == 3)
                    _secondary[off] = 0xff;
                else if(lb > 0)
                    _secondary[off] = lb - 1;
                else
                    _secondary[off] = lb;
            }
        }
    }

    size_t neighbors(size_t off) {
        size_t n = 0;
        for(size_t oy = 0; oy < 3; ++oy, off += _w - 3)
            for(size_t ox = 0; ox < 3; ++ox, ++off) {
                size_t o = off;
                if(o < _w + 1)
                    o += _sz;
                o -= _w + 1;
                if(o > _sz)
                    o -= _sz;
                if(_primary[o] > 0x80)
                    ++n;
            }

        return n;
    }

public:
    LifeBoard(size_t w, size_t h) : _w(w), _h(h), _sz(_w * _h), _primary(new uint8_t[_sz]), _secondary(new uint8_t[_sz]) {
        size_t cores = std::thread::hardware_concurrency() >> 1;
        size_t off = 0;
        while(off < _sz) {
            size_t left_cores = cores - _threads.size();
            size_t off_diff = (_sz - off) / left_cores;
            _threads.emplace_back(std::make_unique<ThreadLoop>([this, off, off_diff](){
                cycle(off, off + off_diff);
            }));
            off += off_diff;
        }
    }
    ~LifeBoard() {
        for(auto &t : _threads)
            t->join();

        delete[] _primary;
        delete[] _secondary;
    }

    void fill_random(double p) {
        std::random_device rd;
        std::default_random_engine re(rd());
        std::uniform_real_distribution<double> uniform_dist(0.0, 1.0);
        
        for(size_t n = 0; n < _w * _h; ++n)
            _primary[n] = uniform_dist(re) < p ? 0xff : 0x00;
    }

    void next() {
        for(auto &t : _threads)
            t->next();
    }
    void wait() {
        for(auto &t : _threads)
            t->wait();

        std::swap(_primary, _secondary);
    }

    void cycle() {
        next();
        wait();
    }

    inline uint8_t* board() { return _primary; }
    inline uint8_t cell(int x, int y) { 
        if(x < 0)
            x += _w;
        if(x >= _w)
            x -= _w;
        if(y < 0)
            y += _h;
        if(y >= _h)
            y -= _h;
        if(x >= 0 && y >= 0 && x < _w && y < _h)
            return _primary[y * _w + x]; 
        else
            return 0;
    }
    inline size_t neighbors(size_t x, size_t y) { return neighbors(y * _w + x); }
};