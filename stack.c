#define MODULE STACKM

#include <stack.h>

void (stack_push)(sanchor *a, stack *s){
    assert(!a->n);
    assert(a != s->top);
    
    a->n = s->top;
    s->top = a;
}

sanchor *(stack_pop)(stack *s){
    sanchor *t = s->top;
    if(!t)
        return NULL;
    s->top = t->n;
    
    t->n = NULL;
    return t;
}

sanchor *stack_peek(const stack *s){
    return s->top;
}

sanchor *sanchor_next(sanchor *a){
    return a->n;
}

static
sanchor *sanc_read(sanchor *a){
    return a->n;
}

static
void sanc_write(sanchor *n, sanchor *to){
    to->n = n;
}

cnt (lfstack_push)(sanchor *a, lfstack *s){
    assert(!a->n);
    return lfstack_push_spanc(a, s, (void (*)(spanc *, spanc *)) sanc_write);
}

sanchor *(lfstack_pop)(lfstack *s){
    sanchor *a = lfstack_pop_spanc(s, (spanc *(*)(spanc *)) sanc_read);
    if(a)
        a->n = NULL;
    return a;
}

/* When reading s in lfstack_pop(), bad read ordering can bring out ABA
   despite the use of version counts. In particular, if you read t :=
   s->top and n := t->n before s->gen, then it can be that t is popped and
   pushed just before the s->gen read. Your CAS will succeed even though
   t->n != n. */
struct lfstack lfstack_read(lfstack *s){
    lfstack r;
    r.gen = s->gen;
    fuzz_atomics();
    r.top = s->top;
    return r;
}

cnt (lfstack_push_spanc)(spanc *a, lfstack *s, spanc_writer *w){
    for(struct lfstack x = *s;;){
        w(x.top, a);
        if(cas2_won(rup(x, .top=a), s, &x))
            return x.gen;
    }
}

spanc *(lfstack_pop_spanc)(lfstack *s, spanc_reader *r){
    for(struct lfstack x = lfstack_read(s);;){
        if(!x.top)
            return NULL;
        if(cas2_won(rup(x, .top=r(x.top), .gen++), s, &x)){
            return x.top;
        }
    }
}

/* Callers can avoid gen updates if they only ever pop_all from s, or can
   repurpose it to communicate something extra. */
struct stack (lfstack_clear)(cnt incr, lfstack *s){
    for(struct lfstack x = *s;;){
        if((!x.top && !incr)
           || cas2_won(rup(x, .top=NULL, .gen+=incr), s, &x))
            return lfstack_convert(&x);
    }
}

bool (lfstack_clear_cas_won)(uptr ngen, lfstack *s, struct lfstack *o){
    return cas2_won(((lfstack){.top=NULL, .gen=ngen}), s, o);
}

bool (lfstack_push_cas_won)(sanchor *a, uptr ngen, lfstack *s, struct lfstack *o){
    assert(!a->n);
    a->n = o->top;
    bool won = cas2_won(((lfstack){.top=a, .gen=ngen}), s, o);
    if(!won)
        a->n = NULL;
    return won;
}

sanchor *(lfstack_pop_iff)(sanchor *head, uptr gen, lfstack *s){
    if(!head)
        return NULL;
    for(struct lfstack x = lfstack_read(s);;){
        if(x.top != head || x.gen != gen)
            return NULL;
        if(cas2_won(rup(x, .top=x.top->n, .gen++), s, &x)){
            x.top->n = NULL;
            return x.top;
        }
    }
}

uptr (lfstack_push_iff)(sanchor *a, uptr gen, lfstack *s){
    assert(!a->n);
    for(struct lfstack x = *s;;){
        if(x.gen != gen){
            a->n = NULL;
            return x.gen;
        }
        a->n = x.top;
        if(cas2_won(rup(x, .top = a), s, &x))
            return x.gen;
    }
}

uptr lfstack_gen(const lfstack *s){
    return s->gen;
}

sanchor *lfstack_peek(const lfstack *s){
    return s->top;
}

struct stack lfstack_convert(const lfstack *s){
    return (stack){s->top};
}

