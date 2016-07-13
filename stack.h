#pragma once

#include <dial_util.h>

typedef volatile struct sanchor{
    volatile struct sanchor *n;
} sanchor;
#define SANCHOR {}

typedef volatile struct stack{
    sanchor *top;
} stack;
#define STACK {}

sanchor *stack_pop(stack *s);
void stack_push(sanchor *a, stack *s);
sanchor *stack_peek(const stack *s);

align(sizeof(dptr))
typedef volatile struct lfstack{
    sanchor *top;
    uptr gen;
} lfstack;
#define LFSTACK {}
#define GEN_LFSTACK(_gen) {.gen=_gen}

uptr lfstack_push(sanchor *a, lfstack *s);
sanchor *lfstack_pop(lfstack *s);
struct stack lfstack_clear(cnt incr, lfstack *s);

uptr lfstack_gen(const lfstack *s);
sanchor *lfstack_peek(const lfstack *s);

bool lfstack_clear_cas_won(uptr ngen, lfstack *s, struct lfstack *os);
bool lfstack_push_upd_won(sanchor *a, uptr ngen, lfstack *s, struct lfstack *os);

uptr lfstack_push_iff(sanchor *a, uptr gen, lfstack *s);
sanchor *lfstack_pop_iff(sanchor *head, uptr gen, lfstack *s);

struct stack lfstack_convert(const lfstack *s);

typedef volatile void spanc;
typedef spanc *(spanc_reader)(spanc *from);
typedef void (spanc_writer)(spanc *p, spanc *to);
cnt lfstack_push_spanc(spanc *a, lfstack *s, spanc_writer *w);
spanc *lfstack_pop_spanc(lfstack *s, spanc_reader *r);

sanchor *sanchor_next(sanchor *a);

#define pudef (struct stack, "(stack){top:%}", a->top)
#include <pudef.h>
#define pudef (struct lfstack, "(lfstack){top:%,gen:%}", a->top, a->gen)
#include <pudef.h>

#define lfstack_clear_cas_won(ngen, s, os)                          \
    trace(STACKM, 1, lfstack_clear_cas_won, PUN(uptr, ngen), s, os)
#define lfstack_push_upd_won(a, ngen, s, os)                    \
    trace(STACKM, 1, lfstack_push_upd_won, a, PUN(uptr, ngen), s, os)


#define lfstack_push(as...) trace(STACKM, 1, lfstack_push, as)
#define lfstack_pop(as...) trace(STACKM, 1, lfstack_pop, as)
#define lfstack_clear(as...) trace(STACKM, 1, lfstack_clear, as)

#define lfstack_push_iff(as...) trace(STACKM, 1, lfstack_push_iff, as)
#define lfstack_pop_iff(as...) trace(STACKM, 1, lfstack_pop_iff, as)

#define stack_pop(as...) trace(STACKM, 1, stack_pop, as)
#define stack_push(as...) trace(STACKM, 1, stack_push, as)

#ifndef LOG_STACKM
#define LOG_STACKM 1
#endif
