#include "bottlingplant.h"
#include "MPRNG.h"

extern MPRNG mprng;

BottlingPlant::BottlingPlant( Printer &prt, NameServer &nameServer, unsigned int numVendingMachines,
	unsigned int maxShippedPerFlavour, unsigned int maxStockPerFlavour,
	unsigned int timeBetweenShipments ) :
	printer(prt), nameServer(nameServer),
	numVendingMachines(numVendingMachines), maxShippedPerFlavour(maxShippedPerFlavour),
	maxStockPerFlavour(maxStockPerFlavour), timeBetweenShipments(timeBetweenShipments),
	isClosing(false)
{}

void BottlingPlant::produceSodas() {
	unsigned int generatedBottles = 0;
	for (int i = 0; i < VendingMachine::NUM_FLAVOURS; i++) {
		producedSodas[i] = mprng(maxShippedPerFlavour);
		generatedBottles += producedSodas[i];
	}

	printer.print(Printer::BottlingPlant, 'G', generatedBottles);
}

void BottlingPlant::main() {
	Truck truck(printer, nameServer, *this, numVendingMachines, maxStockPerFlavour);

	printer.print(Printer::BottlingPlant, 'S');

	// produce first before accept truck's getShipment (must be outside loop for no yield)
	produceSodas();

	for (;;) {
		_Accept(~BottlingPlant) {
			isClosing = true; // for notifiying truck of closing

			try {
				_Accept(getShipment) {	// allow truck to call getShipment so exception can be thrown to indicate closing
					break;
				}
			} catch(uMutexFailure::RendezvousFailure) {
				break;
			}
		} or _Accept(getShipment) {
			printer.print(Printer::BottlingPlant, 'P');

			yield(timeBetweenShipments);	// delay before next shipment

			// produce more sodas
			produceSodas();
		}
	}

	printer.print(Printer::BottlingPlant, 'F');
}

void BottlingPlant::getShipment( unsigned int cargo[] ) {
	if (!isClosing) {
		for (int i = 0; i < VendingMachine::NUM_FLAVOURS; i++) {
			// put produced soda onto cargo for shipping
			cargo[i] = producedSodas[i];
			producedSodas[i] = 0;
		}
	} else {
		_Throw Shutdown();
	}
}
