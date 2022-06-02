#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

#define STR_SIZE 15


enum {
	BEG,
	END
};
enum {
	NEXT,
	PREV
};


class element {
public:
	int wartosc;
	element* xorAdresow;
	element(int value) {
		wartosc = value;
		xorAdresow = nullptr;
	};
	element(int value, element* xorAdr) {
		xorAdresow = xorAdr;
		wartosc = value;
	};
};

class kolejka {
public:
	element* first, * last;
	element* actual, * next, * prev;
	element* front, * back;
	int size;

	kolejka() {
		first = last = actual = prev = next = front = back = nullptr;
		size = 0;
	}
	~kolejka();

	element* operacjaXOR(element* a, element* b) {
		return (element*)((size_t)a ^ (size_t)b);
	}

	//metody listowe:
	void ustawPrevNext();
	void act();
	void move(int dir);
	void addBegEnd(int value, int where);
	void addAct(int value);
	void delBegEnd(element*& kraniecUsuwany, element*& kraniecModyfikowany);
	void delAct();
	void delVal(int value);
	void del(element* usuwany, element* sasiad1, element* sasiad2);
	void print(int from);
	int getSize() {
		return size;
	}

	//metody kolejkowe:
	void push(int n);
	void pop();
	void printQ();
	int count();
	void zerujListe();
	void czyscListe();
};


// metody klasy list:
//ustawienie wartosci previous i next
void kolejka::ustawPrevNext() {
	if (!prev) prev = operacjaXOR(actual->xorAdresow, next);
	if (!next) next = operacjaXOR(actual->xorAdresow, prev);
};

//wyczytanie wartosci ACTUAL
void kolejka::act() {
	if (actual)
		printf("%i\n", actual->wartosc);
	else printf("NULL\n");
};

//wyczytanie wartosci nastepnego/poprzedniego elementu i przesuniecie ACTUAL
void kolejka::move(int dir) {
	if (size == 0) {
		//cout << "NULL\n";
		return;
	}
	ustawPrevNext();
	if (dir == NEXT) {
		prev = actual;
		actual = next;
		next = operacjaXOR(prev, actual->xorAdresow);
	}
	else {
		next = actual;
		actual = prev;
		prev = operacjaXOR(next, actual->xorAdresow);
	}
	//cout << actual->wartosc << endl;
};

//dodawanie elementu przed aktualnym
void kolejka::addAct(int value) {
	element* nowy;
	if (!actual) return;	//lista pusta
	else if (size == 1) {	//lista jednoelementowa
		nowy = new element(value, operacjaXOR(actual, actual));
		actual->xorAdresow = operacjaXOR(nowy, nowy);
		prev = next = first = nowy;
	}
	else {
		ustawPrevNext();
		nowy = new element(value, operacjaXOR(actual, prev));
		prev->xorAdresow = operacjaXOR(nowy, operacjaXOR(prev->xorAdresow, actual));
		actual->xorAdresow = operacjaXOR(nowy, next);
		prev = nowy;
		if (actual == first) first = nowy;
	}
	size++;
};

//dodawanie elementu na poczatek lub koniec listy
void kolejka::addBegEnd(int value, int where) {
	element* nowy = new element(value);
	size++;
	if (size == 1) {
		first = last = actual = prev = next = nowy;
		nowy->xorAdresow = operacjaXOR(nowy, nowy);
		return;
	}
	else if (size == 2) {
		prev = next = nowy;
		if (where == BEG) first = nowy;
		else last = nowy;
		nowy->xorAdresow = operacjaXOR(actual, actual);
		return;
	}

	last->xorAdresow = operacjaXOR(nowy, operacjaXOR(first, last->xorAdresow));
	first->xorAdresow = operacjaXOR(nowy, operacjaXOR(last, first->xorAdresow));
	nowy->xorAdresow = operacjaXOR(first, last);

	if (actual == first) prev = nowy;
	else if (actual == last) next = nowy;

	if (where == BEG) {
		first = nowy;
	}
	else if (where == END) {
		last = nowy;
	}
};

//usuwanie elementu
void kolejka::del(element* usuwany, element* sasiad1, element* sasiad2) {
	sasiad1->xorAdresow = operacjaXOR(sasiad2, operacjaXOR(usuwany, sasiad1->xorAdresow));
	sasiad2->xorAdresow = operacjaXOR(sasiad1, operacjaXOR(usuwany, sasiad2->xorAdresow));
	delete usuwany;
}

//usuwanie z poczatku lub konca listy
void kolejka::delBegEnd(element*& kraniecUsuwany, element*& kraniecModyfikowany) {
	if (size == 0) return;
	
	//obsluga usuwanych krancow kolejki
	if (kraniecUsuwany == back) {
		if (kraniecUsuwany == first) {
			back = last;
		}
		else back = operacjaXOR(kraniecModyfikowany, kraniecUsuwany->xorAdresow);
	}
	else if (kraniecUsuwany == front) {
		if (kraniecUsuwany == last) {
			front = first;
		}
		else front = operacjaXOR(kraniecModyfikowany, kraniecUsuwany->xorAdresow);
	}

	if (size == 2) {
		kraniecModyfikowany->xorAdresow = operacjaXOR(kraniecModyfikowany, kraniecModyfikowany);
		delete kraniecUsuwany;
		kraniecUsuwany = prev = next = actual = kraniecModyfikowany;
	}
	else if (size == 1) {
		delete kraniecUsuwany;
		kraniecUsuwany = kraniecModyfikowany = actual = prev = next = nullptr;
	}
	else {
		ustawPrevNext();
		if (actual == kraniecUsuwany) {
			prev = operacjaXOR(prev->xorAdresow, actual);
			actual = operacjaXOR(actual->xorAdresow, next);
		}
		else if (prev == kraniecUsuwany) {
			if (kraniecUsuwany == first) prev = last;
			else prev = operacjaXOR(last->xorAdresow, first);
		}
		else if (next == kraniecUsuwany) {
			if (kraniecUsuwany == last) next = first;
			else next = operacjaXOR(first->xorAdresow, last);
		}
		element* second = operacjaXOR(kraniecUsuwany->xorAdresow, kraniecModyfikowany);
		del(kraniecUsuwany, kraniecModyfikowany, second);
		kraniecUsuwany = second;
	}
	size--;
}

//usuwanie aktualnego elementu i aktualizacja wskaznika ACTUAL
void kolejka::delAct() {
	if (actual == first) {
		delBegEnd(first, last);
		return;
	}
	else if (actual == last) {
		delBegEnd(last, first);
		return;
	}
	//tablica 0-, 1- i 2elementowa spełniają powyższe warunki i są obsługiwane w funkcji delBegEnd()

	ustawPrevNext();
	del(actual, prev, next);
	actual = prev;
	prev = operacjaXOR(next, actual->xorAdresow);
	size--;
}

//usuwanie elementow o zadanej wartosci
void kolejka::delVal(int value) {
	while (actual && actual->wartosc == value) delAct();	// po tym bloku instrukcji elementy
	if (size == 0) return;									// actual i prev zostana zaktualizowane
	ustawPrevNext();										// i przechowane we wskaznikach tmp i tmPrev
	while (prev && prev->wartosc == value) {
		next = actual;
		actual = prev;
		prev = operacjaXOR(actual->xorAdresow, next);
		delAct();

		prev = actual;
		actual = next;
		next = operacjaXOR(prev, actual->xorAdresow);
	}

	if (!actual || actual == prev) return;
	element* tmp = actual;
	element* tmPrev = prev;
	int S = size;

	actual = first;
	next = operacjaXOR(last, first->xorAdresow);
	prev = last;

	for (int i = 0; i < S; i++) {
		if (actual->wartosc == value) delAct(); // wykorzystanie metody DEL_ACT do oczyszczenia listy
		prev = actual;
		actual = next;
		next = operacjaXOR(actual->xorAdresow, prev);
	}

	actual = tmp;
	prev = tmPrev;
	next = operacjaXOR(actual->xorAdresow, prev);
}

//drukowanie listy
void kolejka::print(int from) {
	if (!actual) {
		printf("NULL\n");
		return;
	}
	else if (size == 1) {
		printf("%i\n", first->wartosc);
		return;
	}
	element* curr, * currNext, * tmp;
	if (from == BEG) {
		curr = first;
		currNext = operacjaXOR(first->xorAdresow, last);
	}
	else {
		curr = last;
		currNext = operacjaXOR(last->xorAdresow, first);
	}

	printf("%i ", curr->wartosc);
	for (int i = 1; i < size; i++) {
		tmp = curr;
		curr = currNext;
		currNext = operacjaXOR(curr->xorAdresow, tmp);
		printf("%i ", curr->wartosc);
	}
	printf("\n");
}

//dekonstruktor
kolejka::~kolejka() {
	while (actual) delAct();
}


//metody rozszerzające listę na kolejkę:
// dodawanie elementu
void kolejka::push(int n) {
	int s = count();
	if (s == size) {
		if (s == 0) {
			//printf("push q=l=0\n");
			addBegEnd(n, BEG);
			front = back = first;
		}
		else {
			//printf("push q=l!=0\n");
			addAct(n);
			move(PREV);
			front = actual;
		}
	}
	else {
		if (s == 0) {
			//printf("push q!=l /=0\n");
			actual->wartosc = n;
			front = back = actual;
			return;
		}
		//printf("push q!=l /!=0\n");
		prev->wartosc = n;
		move(PREV);
		front = actual;
	}
}

// usuwanie elementu
void kolejka::pop() {
	if (count() > 0) {
		printf("%i\n", back->wartosc);
		if (front == back) {
			front = back = nullptr;
			return;
		}
		else {
			ustawPrevNext();
			element* tmp = actual;
			element* tmPrev = prev;
			while (next != back) move(NEXT);
			back = actual;
			actual = tmp;
			prev = tmPrev;
			next = operacjaXOR(actual->xorAdresow, prev);
		}
	}
	else printf("NULL\n");
}

//drukowanie kolejki
void kolejka::printQ() {
	int s = count();
	//printf("c: %i\n", s);
	if (s == 0) {
		printf("NULL\n");
		return;
	}
	for (int i = 1; i < s; i++) move(NEXT); //ustawienie actual na back
	for (int i = 1; i < s; i++) { //wypisywanie kolejki poprzez cofanie actual
		printf("%i ", actual->wartosc);
		move(PREV);
	}
	printf("%i\n", actual->wartosc);
}

//zliczanie elementów kolejki
int kolejka::count() {
	if (!front) return 0;
	int s = 1;
	while (front != actual) {
		move(NEXT);
	}
	while (actual != back) {
		move(NEXT);
		s++;
	}
	for (int i = 1; i < s; i++) move(PREV);
	return s;
}

//uzupełnianie nieużywanych elementów listy zerami
void kolejka::zerujListe() {
	if (!front) {
		int s = size;
		for (int i = 0; i < s; i++) {
			actual->wartosc = 0;
			move(NEXT);
		}
		return;
	}
	
	while (prev != back) {
		prev->wartosc = 0;
		move(PREV);
	}
	if (count() > size / 2) {
		while (actual != front) move(NEXT);
	}
	else {
		while (actual != front) move(PREV);
	}
}

//usuwanie nieużywanych elementów listy
void kolejka::czyscListe() {
	//if (count() == 0) return;
	move(PREV);
	while (actual != back) {
		delAct();
	}
	move(NEXT);
}


//porównywanie ciągów znaków
bool same(char* s1, const char* s2) {
	int i = 0;
	for (; s1[i] != '\0'; i++) {
		if (s1[i] != s2[i]) return false;
	}
	return true;
}


int main() {
	char s[STR_SIZE];
	int n;
	kolejka k;
	
	while (scanf("%s", &s) != EOF) {
		if (same(s, "ACTUAL")) k.act();
		else if (same(s, "NEXT")) {
			k.move(NEXT);
			k.act();
		}
		else if (same(s, "PREV")) {
			k.move(PREV);
			k.act();
		}
		else if (same(s, "ADD_BEG")) {
			scanf("%i", &n);
			k.addBegEnd(n, BEG);
		}
		else if (same(s, "ADD_END")) {
			scanf("%i", &n);
			k.addBegEnd(n, END);
		}
		else if (same(s, "ADD_ACT")) {
			scanf("%i", &n);
			k.addAct(n);
		}
		else if (same(s, "DEL_BEG")) k.delBegEnd(k.first, k.last);
		else if (same(s, "DEL_END")) k.delBegEnd(k.last, k.first);
		else if (same(s, "DEL_VAL")) {
			scanf("%i", &n);
			k.delVal(n);
		}
		else if (same(s, "DEL_ACT")) k.delAct();
		else if (same(s, "PRINT_FORWARD")) k.print(BEG);
		else if (same(s, "PRINT_BACKWARD")) k.print(END);
		else if (same(s, "SIZE")) printf("%i\n", k.getSize());
		else if (same(s, "PUSH")) {
			scanf("%i", &n);
			k.push(n);
		}
		else if (same(s, "POP")) k.pop();
		else if (same(s, "PRINT_QUEUE")) k.printQ();
		else if (same(s, "COUNT")) printf("%i\n", k.count());
		else if (same(s, "GARBAGE_SOFT")) k.zerujListe();
		else if (same(s, "GARBAGE_HARD")) k.czyscListe();
		else if (same(s, "Q")) break;
		//else cout << "np\n";
	}
	return 0;
}