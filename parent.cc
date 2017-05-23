#include "parent.h"
#include "MPRNG.h"

extern MPRNG mprng;

Parent::Parent( Printer &prt, Bank &bank, unsigned int numStudents, unsigned int parentalDelay ) :
  printer(prt), bank(bank), numStudents(numStudents), parentalDelay(parentalDelay) {}

void Parent::main() {
  printer.print(Printer::Parent, 'S');

  for (;;) {
    yield(parentalDelay);

    _Accept(~Parent) {
      break;
    } _Else {
      unsigned int sid = mprng(numStudents - 1);
      unsigned int amount = mprng(1, 3);
      bank.deposit(sid, amount);

      printer.print(Printer::Parent, 'D', sid, amount);
    }
  }

  printer.print(Printer::Parent, 'F');
}
