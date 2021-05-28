#include <iostream>
#include <vector>
#include <set>
#include <fstream>
#include <sstream>

using namespace std;

class Edge;
class Vertex
{
public:
    int index;
    double cost;

    //vector krawędzi wychodzących
    vector<Edge> adjacent_edges;
    bool if_root=false;

    Vertex(int _index):index(_index), cost(numeric_limits<double>::max()){};

    //funkcja wypisująca
    void dump() const;

    //operator potrzebny do uporządkowania zbioru vertices - sortowanie po koszcie dotarcia
    bool operator<(const Vertex&right) const { return cost <= right.cost;}
};

class Edge
{
    //pojedynczy obiekt Edge zawiera wskaźnik na wierzchołki początkowy i końcowy krawędzi oraz jej wagę)
public:
    double cost;
    Vertex* start_v;
    Vertex* end_v;

    Edge(Vertex* _start, Vertex* _end, double _cost):start_v(_start), end_v(_end), cost(_cost){};

    //funkcja wypisująca
    void dump() const{cout<<start_v->index<<"\t"<<end_v->index<<"\t"<<cost<<endl;};
};


void Vertex::dump() const {
    cout<<index<<"\t"<<cost<<"\t"<<if_root<<"\t"<<"Successors: ";
    for(auto e:adjacent_edges)
        cout<<e.end_v->index<<" "<<e.end_v->cost<<" | ";
    cout<<endl;
}

bool is_digit(char c)
{
    return (c-'0'>=0 && c-'9' <=0);
}

void open_file(set<Vertex*> &vertices)
{
    // 1. otwarcie pliku "lista.txt"
    // 2. ekstrakcja wartości z linii
    // 3. jeśli nie ma wierzchołków w vertices, utworzenie i dodanie do vertices
    // 4. utworzenie krawędzi i dodanie jej do zbioru krawędzi wychodzących z początkowego wierzchołka
    // cykl się powtarza, dopóki są linie w pliku

    int index, line_count = 0;  //line_count to numer wierzchołka początkowego, który jest równy numerowi czytanej linii
    double edge_cost;
    string data, pom;

    // 1.
    ifstream input;
    input.open("lista.txt");
    cout<<"Lista sąsiedztwa: "<<endl;
    //2. i 3.
    while(getline(input, data))
    {
        cout<<data<<endl;
        if(data[0]==']') break;
        int found_s=0;
        Vertex *v1;
        for(auto v:vertices)
            if (v->index == line_count)
            {
                v1=v;
                found_s = 1;
            }
        if(!found_s)
        {
            v1 = new Vertex(line_count);
            vertices.insert(v1);
        }

        istringstream iss(data);
        while (getline(iss, pom, '('))
        {
            int found_e=0;
            getline(iss, pom, ',');
            if (!is_digit(pom[0])) break;
            index = stoi(pom);
            Vertex *v2;
            for(auto v:vertices)
                if (v->index == index)
                {
                    v2=v;
                    found_e = 1;
                }
            if(!found_e)
            {
                v2 = new Vertex(index);
                vertices.insert(v2);
            }
            getline(iss, pom, ')');

            //4.
            edge_cost = stod(pom);
            Edge edge(v1, v2, edge_cost);
            v1->adjacent_edges.push_back(edge);
        }
        ++line_count;
    }
    cout<<endl;
}

int if_vertex_in_vertices(vector<Vertex*> &vert, Vertex*v1)
{
    for (auto a: vert)
        if(a==v1)
            return 1;
    return 0;
}

Edge find_min(vector<Vertex*> &vert)
{
    double min_cost=numeric_limits<double>::max();
    Edge edge(0,0,0);
    for(auto v:vert)
        for(auto e:v->adjacent_edges)
        {
            if(if_vertex_in_vertices(vert, e.end_v)) continue;
            if(min_cost > v->cost + e.cost)
            {
                min_cost = v->cost + e.cost;
                edge=e;
            }
        }
    return edge;
}

void modify_costs(vector<Vertex*>&vert)
{
    for(auto a:vert)
        for(auto e:a->adjacent_edges)
            if(a->cost + e.cost < e.end_v->cost)
                e.end_v->cost = a->cost + e.cost;
}

void set_root(set <Vertex*>&vertices, vector<Vertex*>&shortest, int index)
{
    for(auto ver:vertices)
    {
        if(ver->index == index)
        {
            ver->if_root = true;
            ver->cost = 0;
            shortest.push_back(ver);
            modify_costs(shortest);
            vertices.erase(ver);
            break;
        }
    }
}

void dijkstra(set<Vertex*> &vertices, vector<Vertex*> &shortest)
{
    while(!vertices.empty())
    {
        Edge edge = find_min(shortest);
        modify_costs(shortest);
        vertices.erase(edge.end_v);
        shortest.push_back(edge.end_v);
    }
}


int main() {
    //zbiór wszystkich wierzchołków
    set<Vertex*> vertices;
    //zbiór kolejno odwiedzanych wierzchołków
    vector<Vertex*> shortest_path;

    //otwórz plik, odczytaj listę sąsiedztwa, stwórz obiekty Vertex* i zapisz do zbioru vertices
    open_file(vertices);
    //ustaw dany wierzchołek jako startowy
    set_root(vertices, shortest_path, 0);

    //przejdź algorytmem Dijkstry
    dijkstra(vertices, shortest_path);

    //wypisz indeksy wierzchołków i koszt dotarcia do nich
    cout<<"Najkrótsza ścieżka: ";
    for(auto i:shortest_path)
        cout<<i->index<<" - koszt "<<i->cost<<", ";

    return 0;
}
