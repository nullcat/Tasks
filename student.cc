#include "student.h"
#include "MPRNG.h"

extern MPRNG mprng;

Student::Student( Printer &prt, NameServer &nameServer, WATCardOffice &cardOffice, Groupoff &groupoff,
	unsigned int id, unsigned int maxPurchases ) :
	printer(prt), nameServer(nameServer), groupoff(groupoff), cardOffice(cardOffice), id(id), maxPurchases(maxPurchases),
	numBought(0) {}

void Student::main() {
	VendingMachine::Flavours favouriteFlavour = (VendingMachine::Flavours) mprng(VendingMachine::NUM_FLAVOURS - 1);	// select fav colour
	unsigned int numPurchases = mprng(1, maxPurchases);	// determine number of purchases

	printer.print(Printer::Student, id, 'S', favouriteFlavour, numPurchases);

	WATCard::FWATCard watcard = cardOffice.create(id, 5);
	WATCard::FWATCard giftcard = groupoff.giftCard();

	unsigned int numBought = 0; // keep track of how many sodas were bought
	
	// obtain a machine
	VendingMachine *vendingMachine = nameServer.getMachine(id);
	printer.print(Printer::Student, id, 'V', vendingMachine->getId());

	bool lostWatCard = false;

	// loop for # of purchases
	while(numBought < numPurchases) {
		if(lostWatCard) {	// do not yield if watcard was lost (no call to buy has occured)
			lostWatCard = false;
		} else {
			yield(mprng(1, 10));	// before each "attempt" to buy a soda
		}

		_Select(giftcard) {
			try {
				vendingMachine->buy(favouriteFlavour, *giftcard());
				numBought++;
				printer.print(Printer::Student, id, 'G', giftcard()->getBalance());
				
				delete giftcard();
				giftcard.reset();	// prevent further usage (can only be used once)
			} catch (VendingMachine::Stock) {
				vendingMachine = nameServer.getMachine(id);
				printer.print(Printer::Student, id, 'V', vendingMachine->getId());
			}
		} or _Select(watcard) {
			try {
				vendingMachine->buy(favouriteFlavour, *(watcard()));
				numBought++;
				printer.print(Printer::Student, id, 'B', watcard()->getBalance());
			} catch (VendingMachine::Funds) {
				watcard = cardOffice.transfer(id, 5 + vendingMachine->cost(), watcard());
			} catch (VendingMachine::Stock) {
				vendingMachine = nameServer.getMachine(id);
				printer.print(Printer::Student, id, 'V', vendingMachine->getId());
			} catch (WATCardOffice::Lost) {
				watcard = cardOffice.create(id, 5);
				printer.print(Printer::Student, id, 'L');
				lostWatCard = true;	// no call to buy has occured, skip yield
			}
		}
	}

	try {
		delete watcard();
	} catch (WATCardOffice::Lost) {
		// this case happens when an exception is delivered rather than a real watcard
		// when the student uses a giftcard for their last purchase (watcard may be lost after that)
	}

	printer.print(Printer::Student, id, 'F');
}
