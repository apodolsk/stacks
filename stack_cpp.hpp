#pragma once

struct sanchor{
    sanchor *n = NULL;

    sanchor *next();
};

struct stack{
    sanchor *top = NULL;

    void push(sanchor &a);
    sanchor *pop();

    sanchor *peek() const;
};

struct top{
    uptr gen = 0;
    sanchor *top = NULL;

    operator stack() const;
    sanchor *peek() const;
};

struct lfstack : std::atomic<top>{
    uptr push(sanchor &a);
    sanchor *pop();
    
    bool clear_cas_won(uptr ngen, top &os);
    bool push_cas_won(sanchor &a, uptr ngen, top &os);

    lfstack() = default;
    lfstack(const lfstack& l){
        *this = l;
        /* *(std::atomic<top> *)this = (top)l; */
    }

    lfstack &operator=(const lfstack &other){
        this->store((top) other);
        return *this;
    }
};
