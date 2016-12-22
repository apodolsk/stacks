#define MODULE STACKM

#include <stack.hpp>

sanchor *sanchor::next(){
    return n;
}

void stack::push(sanchor &a){
    assert(!a.n);
    assert(a != top);
    
    a.n = top;
    top = &a;
}

sanchor *stack::pop(){
    sanchor *t = top;
    if(!t)
        return NULL;
    top = t->n;
    
    t->n = NULL;
    return t;
}

sanchor *stack::peek() const{
    return top;
}

uptr lfstack::push(sanchor &a){
    assert(!a.n);
    for(top x = *this;;){
        a.n = x.top;
        if(this->compare_exchange_strong(x, rup(x, .top = &a)))
            return x.gen;
    }
}

sanchor *lfstack::pop(){
    for(top x = *this; x.top;)
        if(this->compare_exchange_strong(x, rup(x, .top = x.top->next())))
            return x.top;
    return NULL;
}

bool lfstack::clear_cas_won(uptr ngen, top &o){
    return this->compare_exchange_strong(o, (top){ngen, NULL});
}

bool lfstack::push_cas_won(sanchor &a, uptr ngen, top &o){
    assert(!a.n);
    a.n = o.top;
    bool won = this->compare_exchange_strong(o, (top){ngen, &a});
    if(!won)
        a.n = NULL;
    return won;
}

/* uptr lfstack::gen() const{ */
/*     top t = tx; */
/*     return t.gen; */
/* } */

sanchor *top::peek() const{
    return top;
}

top::operator stack() const{
    return (stack){top};
}

