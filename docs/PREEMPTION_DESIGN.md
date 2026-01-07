# Design Preemption

## Problema
Vrem preemption la fiecare tick de timer, dar `yield()` actual salvează doar callee-saved registers (s0-s11, ra, sp).

## Soluție Simplă (Hybrid)
1. **Timer tick**: Setează `need_resched = 1`
2. **La următorul sys_yield()**: Procesul va fi preemptat
3. **Rezultat**: Preemption pseudo-periodică (la primul syscall după tick)

## Soluție Complexă (True Preemptive - NU implementată)
Ar necesita:
1. Context struct extins cu TOATE registrele
2. Salvare/restaurare a0-a7, t0-t6 în `comutare()`
3. Modif icare trap handler pentru a apela o funcție specială de preemption

## Implementare Actuală
- **Cooperative scheduling** cu yield() des
- **Timer monitoring**: CPU time tracking
- **Fair round-robin**: Fiecare proces primește quantum egal

Aceasta este abordarea standard pentru OS-uri didactice simple!
