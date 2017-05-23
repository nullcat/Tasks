#include "groupoff.h"
#include "MPRNG.h"

extern MPRNG mprng;

Groupoff::Groupoff( Printer &prt, unsigned int numStudents, unsigned int sodaCost, unsigned int groupoffDelay ) :
	printer(prt), numStudents(numStudents), sodaCost(sodaCost), groupoffDelay(groupoffDelay)
{
	futureGiftCards = new WATCard::FWATCard[numStudents];
}

Groupoff::~Groupoff() {
	// delete unused giftcards
	for(unsigned int j = 0; j < numStudents; j++) {
		_Select(futureGiftCards[j]) { // check if student has already used it/deleted it, if yes, just continue (_Else)
			delete futureGiftCards[j]();
		} _Else {}
	}
	
	delete [] futureGiftCards;
}

void Groupoff::main() {
	bool isGifted[numStudents] = { false };
	printer.print(Printer::Groupoff, 'S');

	for(cardIndex = 0; cardIndex < numStudents; cardIndex++) {
		_Accept(giftCard); // wait for all students to call giftcard() before assigning real WATCards
	}

	cardIndex = 0;
	for(unsigned i = 0; i < numStudents; i++) {
		yield(groupoffDelay);

		// program may end before all students receive their gift cards
		_Accept(~Groupoff) { // do not need to send all students gift cards if the program already ended (they don't need the gift cards)
			break;
		} _Else {
			cardIndex = rand() % numStudents; // choose random card/student
			
			while(isGifted[cardIndex]) { // find a card that has not been gifted yet
				cardIndex = (cardIndex + 1) % numStudents;
			}

			isGifted[cardIndex] = true;

			WATCard *giftCard = new WATCard();
			giftCard->deposit(sodaCost);

			futureGiftCards[cardIndex].delivery(giftCard); // deliver real card to student
			printer.print(Printer::Groupoff, 'D', sodaCost); // gift card will always have the amount of sodaCost
		}

		cardIndex++;
	}

	printer.print(Printer::Groupoff, 'F');
}

WATCard::FWATCard Groupoff::giftCard() {
	return futureGiftCards[cardIndex];
}
