#include "vendingmachine.h"

void VendingMachine::main() {
	printer.print(Printer::Vending, id, 'S', sodaCost);
	nameServer.VMregister(this);	// register to nameServer

	for (;;) {
		try {
			_Accept(~VendingMachine) {
				break;
			} or _When(!isRestocking) _Accept(buy) {
			} or _When(!isRestocking) _Accept(inventory) {	// if there are multiple trucks, prevent them from restocking at the same time
				isRestocking = true;
				printer.print(Printer::Vending, id, 'r');
			} or _When(isRestocking) _Accept(restocked) {
				isRestocking = false;
				printer.print(Printer::Vending, id, 'R');
			}
		} catch(uMutexFailure::RendezvousFailure) {}
	}

	printer.print(Printer::Vending, id, 'F');
}

VendingMachine::VendingMachine( Printer &prt, NameServer &nameServer, unsigned int id, unsigned int sodaCost,
	unsigned int maxStockPerFlavour ) :
	printer(prt), nameServer(nameServer), id(id), sodaCost(sodaCost), maxStockPerFlavour(maxStockPerFlavour),
	isRestocking(false)
{
	// initialize inventory
	for (int i = 0; i < NUM_FLAVOURS; i++) {
		sodaInventory[i] = 0;
	}
}

void VendingMachine::buy( Flavours flavour, WATCard &card ) {
	if (sodaCost > card.getBalance()) {
		_Throw Funds();	// priority on funds, so that student can send a transfer request quicker
	} else if (sodaInventory[flavour] == 0) {
		_Throw Stock();
	} else {
		card.withdraw(sodaCost);
		printer.print(Printer::Vending, id, 'B', flavour, --sodaInventory[flavour]);
	}
}

unsigned int *VendingMachine::inventory() {
	return sodaInventory;
}

void VendingMachine::restocked() {
	// everything moved to main
}

_Nomutex unsigned int VendingMachine::cost() {
	return sodaCost;
}

_Nomutex unsigned int VendingMachine::getId() {
	return id;
}
