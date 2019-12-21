#include "insertion_ordered_map.h"
#include <iostream>
#include <vector>
#include <string>
#include <cassert>
using namespace std;
int ile = 0;
string message;
bool rzucaj = false;
bool wypisuj = true;
class XD {};
class Key {
public:
    int v=0;
    Key() {}
    explicit Key(int w): v(w) {}
    Key(const Key &k) {
        v = k.v;
        if (ile > 0) {
            if (wypisuj) cout << message << "\n";
            ile--;
        }
        if (!wypisuj && ile==0) throw XD{};
        if (rzucaj) throw XD{};
    }
    Key(Key &&k) noexcept{
        v = std::move(k.v);
    }

    Key& operator=(Key k) noexcept {
        v=std::move(k.v);
        return *this;
    }
};

bool operator==(Key const &lhs, Key const &rhs) {
    return lhs.v == rhs.v;
}
bool operator!=(Key const &lhs, Key const &rhs) {
    return lhs.v != rhs.v;
}

struct Hash {
    size_t operator()(const Key &k) const {
        return std::hash<int>()(k.v);
    }
};
void check(const insertion_ordered_map<Key,int,Hash> &m,
           const vector<int>&order, const vector<int>&values) {
    auto o=order.begin();
    auto v=values.begin();
    for (auto &i:m) {
        assert(i.first == Key(*o));
        assert(i.second == *v);
        o++;
        v++;
    }
}

void check2(const insertion_ordered_map<Key,Key,Hash> &m,
            const vector<int>&order, const vector<int>&values) {
    auto o=order.begin();
    auto v=values.begin();
    for (auto &i:m) {
        assert(i.first == Key(*o));
        assert(i.second == Key(*v));
        o++;
        v++;
    }
}

void check3(const insertion_ordered_map<Key,Key,Hash> &m,
            vector<typename insertion_ordered_map<Key,Key,Hash>::iterator> &order) {
    size_t xd=0;
    for (auto it=m.begin(); it!=m.end(); ++it, ++xd) {
        assert(it == order[xd]);
        assert(it->first == order[xd]->first);
        assert(it->second == order[xd]->second);
    }
}

int main() {
    cout << "Jeżeli program przejdzie do końca i wypisze komunikat \"Accepted\""
         << " to znaczy, że przeszedł test. Przy odpalaniu z valgrindem wyrzuć fragment "
         << "z tworzeniem dużej tablicy (valgrind nie umie obsłużyć bad_alloc rzuconego "
         << "przez new[])." << std::endl << std::endl;
//    try {
//        insertion_ordered_map<int, int> *t = new insertion_ordered_map<int, int>[1000000000];
//        delete[] t;
//        cout << "czemu to nie wyjebało pamięci xD\n";
//    } catch (bad_alloc &e) {
//        cout << "bad_alloc (ok)\n";
//    }
    insertion_ordered_map<Key, int, Hash> m1;
    m1.insert(Key{2},3);
    ile = 1;
    message = "gunwo";
    insertion_ordered_map<Key, int, Hash> m2(m1); // nic nie powinno sie wypisać
    try {
        auto &ref = m1.at(Key(1));
        cout << "miałeś wyjątek rzucić\n";
    } catch (lookup_error &e) {
        cout << "ok wyjątek\n";
    }
    insertion_ordered_map<Key, int, Hash> m3 = m1; // dalej nic nie powinno się wypisać
    message = "kopiuję się (ok)";
    rzucaj = true;
    try {
        auto &ref = m1[Key(2)];
        cout << "XDDDDDDDD?\n";
        ref = 1;
        assert(m3[Key(2)] == 3);
    } catch (XD &e) {
        cout <<"ok\n";
    } catch (...) {
        cout << "????????\n";
        throw;
    }
    rzucaj = false;
    auto &ref = m1[Key(2)];
    m1.insert(Key(3), -1); // m1 powinien być czysty
    rzucaj = true;
    try {
        m2 = m1;
        insertion_ordered_map<Key, int, Hash> mm(m1);
        cout << "ok (nie kopiuje się)\n";
    } catch (XD &e) {
        cout << "Nie powinno się skopiować\n";
        throw;
    }
    rzucaj = false;
    auto &r = m1[Key(2)];
    m1.erase(Key(2)); // dalej powinien być czysty – zmiana słownika unieważnia referencje do obiektów
    rzucaj = true;
    try {
        insertion_ordered_map<Key, int, Hash> ddd(m1);
        cout << "Ok\n";
    } catch (XD &e) {
        cout << "źle\n";
        throw;
    }
    insertion_ordered_map<Key, int, Hash> c = std::move(m1);
    size_t s = m1.size(); // Peczar mówił że jak jest segfault to jest źle
//    assert(m1.begin() == m1.begin() && m1.end() == m1.end());
//    rzucaj = false;
//    m1.clear();
//    for (int i = 5; i > 0; i--) m1.insert(Key(i),i+2);
//    int dd=5;
//    for (insertion_ordered_map<Key, int, Hash>::iterator it = m1.begin(); it != m1.end(); ++it, dd--) {
//        if (dd == 5) assert(it == m1.begin());
//        assert(it->first == Key(dd) && it->second == dd+2);
//        assert((*it).first == Key(dd) && (*it).second == dd+2);
//        insertion_ordered_map<Key, int, Hash>::iterator j(it);
//        assert(it == j);
//        assert(j->first == Key(dd) && j->second == dd+2);
//    }
//    assert(m1.begin() != m1.end());
//    m1.merge(m1);
//    assert(m1.size() == 5);
//    check(m1, {5, 4, 3, 2, 1}, {7, 6, 5, 4, 3});
//    m2.clear();
//    for (int i=1; i<=3; i++) m2[Key(i)]=i+2;
//    check(m2, {1,2,3}, {3,4,5});
//    m1.merge(m2);
//    check(m1, {5, 4, 1, 2, 3}, {7, 6, 3,4,5});
//    for (int i=1; i<=3; i++) m2[Key(i)] = i;
//    m1.merge(m2);
//    check(m1, {5, 4, 1, 2, 3}, {7, 6, 3, 4, 5});
//    m1.clear();
//    vector<int> v1;
//    for (int i=1; i<10000000; i++) {
//        v1.push_back(i);
//        m1.insert(Key(i),i);
//    }
//    check(m1, v1, v1);
//    m1.merge(m1);//*/
//    check(m1, v1, v1); //może się wyjebać przez rehash
//    // -------------
//    insertion_ordered_map<Key, Key, Hash> fajna;
//    v1.clear();
//    vector<typename insertion_ordered_map<Key,Key,Hash>::iterator> order;
//    for (int i=1; i<10; i++) {
//        fajna.insert(Key(i), Key(i));
//        v1.push_back(i);
//    }
//    for (auto it=fajna.begin(); it!=fajna.end(); ++it) order.push_back(it);
//    insertion_ordered_map<Key, Key, Hash> fajna2(fajna);
//    for (int i = 0; i < 50; i++) {
//        try {
//            wypisuj=true;
//            ile=0;
//            Key k(i), v(i-2);
//            wypisuj=false;
//            ile=i;
//            fajna.insert(k,v);
//            fajna = fajna2;
//            break;
//        } catch (XD &e) {
//            wypisuj=true; ile=0;
//            check2(fajna, v1, v1);
//            check2(fajna2, v1, v1);
//            check3(fajna, order);
//        }
//    }
//    for (int i = 0; i < 50; i++) {
//        try {
//            wypisuj=true;
//            ile=0;
//            Key k(i), v(i-2);
//            wypisuj=false;
//            ile=i;
//            fajna.erase(k);
//            fajna = fajna2;
//            break;
//        } catch (XD &e) {
//            wypisuj=true; ile=0;
//            check2(fajna, v1, v1);
//            check2(fajna2, v1, v1);
//            check3(fajna, order);
//        } catch (lookup_error &e) {
//        } catch (exception &e) {
//            cout << "chyba coś nie tak" << endl;
//            throw;
//        } catch (...) {
//            cout << "X D\n";
//            throw;
//        }
//    }
//    for (int i = 0; i < 50; i++) {
//        try {
//            wypisuj=true;
//            ile=0;
//            Key k(i), v(i-2);
//            wypisuj=false;
//            ile=i;
//            auto &ref = fajna.at(k);
//            ile=0;
//            wypisuj=true;
//            ref.v++;
//            assert(fajna.at(k) != fajna2.at(k));
//            fajna = fajna2;
//            break;
//        } catch (XD &e) {
//            wypisuj=true; ile=0;
//            check2(fajna, v1, v1);
//            check2(fajna2, v1, v1);
//            check3(fajna, order);
//        } catch (lookup_error &e) {
//        } catch (exception &e) {
//            cout << "chyba coś nie tak" << endl;
//            throw;
//        } catch (...) {
//            cout << "X D\n";
//            throw;
//        }
//    }
//    for (int i = 0; i < 50; i++) {
//        try {
//            wypisuj=true;
//            ile=0;
//            Key k(i);
//            wypisuj=false;
//            ile=i;
//            const auto &ref = fajna.at(k);
//            fajna = fajna2;
//            break;
//        } catch (XD &e) {
//        } catch (lookup_error &e) {
//        } catch (exception &e) {
//            cout << "chyba coś nie tak" << endl;
//            throw;
//        } catch (...) {
//            cout << "X D\n";
//            throw;
//        }
//    }
//    for (int i = 0; i < 50; i++) {
//        try {
//            wypisuj=true;
//            ile=0;
//            Key k(i), v(i-2);
//            wypisuj=false;
//            ile=i;
//            auto &ref =fajna[k];
//            ile=0;
//            wypisuj=true;
//            ref.v++;
//            assert(fajna.at(k) != fajna2.at(k));
//            fajna = fajna2;
//            break;
//        } catch (XD &e) {
//            wypisuj=true; ile=0;
//            check2(fajna, v1, v1);
//            check2(fajna2, v1, v1);
//            check3(fajna, order);
//        } catch (lookup_error &e) {
//            fajna = fajna2;
//            break;
//        } catch (exception &e) {
//            cout << "chyba coś nie tak" << endl;
//            throw;
//        } catch (...) {
//            cout << "X D\n";
//            throw;
//        }
//    }
//    srand(2137);
//    for (int i = 0; i < 50; i++) {
//        try {
//            wypisuj=true;
//            ile=0;
//            insertion_ordered_map<Key,Key,Hash> map2;
//            for (int i=0; i<20; ++i) map2.insert(Key(rand()%20),Key(rand()%20));
//            wypisuj=false;
//            ile=i;
//            fajna.merge(map2);
//            fajna = fajna2;
//            break;
//        } catch (XD &e) {
//            wypisuj=true; ile=0;
//            check2(fajna, v1, v1);
//            check2(fajna2, v1, v1);
//            check3(fajna, order);
//        } catch (lookup_error &e) {
//            fajna = fajna2;
//            break;
//        } catch (exception &e) {
//            cout << "chyba coś nie tak" << endl;
//            throw;
//        } catch (...) {
//            cout << "X D\n";
//            throw;
//        }
//    }
//    // sprawdzam czy jest konstruktor bezparametrowy i kopiujący
//    insertion_ordered_map<int,int> it, it2(it);
//    cout << "Accepted\n";
}
