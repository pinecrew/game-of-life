#include <set>

using namespace std;
set< pair< int, int > > alive;

bool check_neighbours(pair< int, int > x){
    int count = 0;
    bool empty = (alive.find(x) == alive.end());
    x.first += 1;
    if (alive.find(x) != alive.end()) count++;
    x.second += 1;
    if (alive.find(x) != alive.end()) count++;
    x.first -= 1;
    if (alive.find(x) != alive.end()) count++;
    x.first -= 1;
    if (alive.find(x) != alive.end()) count++;
    x.second -= 1;
    if (alive.find(x) != alive.end()) count++;
    x.second -= 1;
    if (alive.find(x) != alive.end()) count++;
    x.first += 1;
    if (alive.find(x) != alive.end()) count++;
    x.first += 1;
    if (alive.find(x) != alive.end()) count++;
    return (count == 2 && !empty) || (count == 3);
}

void next() {
/*
    весьма и весьма наивный способ расчитать следующее состояние

    очевидно, что изменения могу происходить только с клетками,
    составляющими объединение единичных окрестностей живых клеток

    для каждой из клеток рассматриваемой окрестности нужно просто
    посчитать число соседей, и в соответствии с правилами, изменить
    её состояние
*/
    set< pair< int, int > > next;
    for(auto i = alive.begin(); i != alive.end(); i++) {
        auto e = *i;
        if (check_neighbours(e)) next.insert( e );
        e.first += 1;
        if (check_neighbours(e)) next.insert( e );
        e.second += 1;
        if (check_neighbours(e)) next.insert( e );
        e.first -= 1;
        if (check_neighbours(e)) next.insert( e );
        e.first -= 1;
        if (check_neighbours(e)) next.insert( e );
        e.second -= 1;
        if (check_neighbours(e)) next.insert( e );
        e.second -= 1;
        if (check_neighbours(e)) next.insert( e );
        e.first += 1;
        if (check_neighbours(e)) next.insert( e );
        e.first += 1;
        if (check_neighbours(e)) next.insert( e );
    }
    alive = next;
}
