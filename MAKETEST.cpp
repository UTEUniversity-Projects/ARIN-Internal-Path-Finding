#include <bits/stdc++.h>
#define inf 1000000007
#define endl '\n'

using namespace std;

// -------------------- DEFINE CONSTANTS AND VARIABLES HERE --------------------

const int NUM_CELLS = 256;
const int NUM_TESTCASES = 1;
const double PI = 3.14159265358980;
int num_vertices, num_edges, num_graverows, num_suggest, num_fars;
double min_lat, max_lat, min_lon, max_lon;

// --------------------------- DEFINE FILE PATH HERE ---------------------------

const string vertex_input_path = "data_txt/vertex.txt";
const string edge_input_path = "data_txt/edge.txt";
const string cell_vertex_input_path = "data_txt/cell_vertex.txt";
const string grave_row_input_path = "data_txt/grave_row.txt";
const string grid_input_path = "data_txt/grid.txt";

ifstream vertex_file(vertex_input_path);
ifstream edge_file(edge_input_path);
ifstream cell_vertex_file(cell_vertex_input_path);
ifstream grave_row_file(grave_row_input_path);
ifstream grid_file(grid_input_path);

// --------------------------- READ DATA START HERE ---------------------------

struct coordinate{
    double lat, lon;
};
struct edge_type{
    int u, v;
    double dis;
};
struct grave_row_type{
    string created_at, updated_at;
    string area_name, row_name;
    int far_west, far_east;
};

vector<coordinate> vertices;
vector<edge_type> edges;
vector<coordinate> grid;
vector<grave_row_type> grave_rows;
vector<vector<int>> suggest;
vector<vector<pair<int, pair<int, double>>>> graph;

void read_data(){

    vertex_file >> num_vertices;
    vertices.resize(num_vertices + 2);
    for (int i = 0; i < num_vertices; i++){
        int idx; vertex_file >> idx;

        coordinate v;
        vertex_file >> v.lat >> v.lon;

        vertices[idx] = v;
    }

    edge_file >> num_edges;
    edges.resize(num_edges + 2);
    graph.resize(num_vertices + 2, vector<pair<int, pair<int, double>>>());
    for(int i = 0; i < num_edges; i++){
        int idx; edge_file >> idx;

        edge_type e;
        edge_file >> e.u >> e.v >> e.dis;

        edges[idx] = e;
        graph[e.u].push_back({idx, {e.v, e.dis}});
        graph[e.v].push_back({idx, {e.u, e.dis}});
    }

    grave_row_file >> num_graverows;
    grave_rows.resize(num_graverows + 2);
    for (int i = 0; i < num_graverows; i++){
        int idx; grave_row_file >> idx;

        grave_row_type gr;
        grave_row_file >> gr.created_at >> gr.updated_at >> gr.area_name >> gr.row_name;
        grave_row_file >> gr.far_west >> gr.far_east;
        
        grave_rows[idx] = gr;
    }

    cell_vertex_file >> num_suggest;
    suggest.resize(NUM_CELLS + 2, vector<int>());
    for (int i = 0; i < num_suggest; i++){
        int c, v;
        cell_vertex_file >> c >> v;
        suggest[c].push_back(v);
    }

    grid_file >> num_fars;
    grid.resize(num_fars + 2);
    for (int i = 0; i < num_fars; i++){
        int idx; grid_file >> idx;

        coordinate g;
        grid_file >> g.lat >> g.lon;
        string name;
        grid_file >> name;

        grid[idx] = g;
    }
}

// ---------------------- GENERATE INPUT TEST START HERE -----------------------

struct testcase_type{
    coordinate coor;
    int row_id;
};

void pre_generation(){
    min_lat = max_lat = grid[1].lat;
    min_lon = max_lon = grid[1].lon;

    for (int i = 2; i <= num_fars; i++){
        min_lat = min(min_lat, grid[i].lat);
        max_lat = max(max_lat, grid[i].lat);
        min_lon = min(min_lon, grid[i].lon);
        max_lon = max(max_lon, grid[i].lon);
    }
}

testcase_type generate_input(){
    mt19937 rng(chrono::steady_clock().now().time_since_epoch().count());
    uniform_int_distribution<int> _row_id(1, num_graverows);
    uniform_real_distribution<double> _lat_coor(min_lat, max_lat);
    uniform_real_distribution<double> _lon_coor(min_lon, max_lon);

    testcase_type test;

    test.coor.lat = _lat_coor(rng);
    test.coor.lon = _lon_coor(rng);
    test.row_id = _row_id(rng);

    return test;
}

// ---------------------- GENERATE OUTPUT TEST START HERE ----------------------

void write_input(testcase_type &test) {
    cout << fixed << setprecision(12);
    cout << test.coor.lat << " " << test.coor.lon << " " << test.row_id << endl;
    cout << endl;

    cout << num_fars << endl;
    for (int i = 1; i <= num_fars; i++){
        cout << grid[i].lat << " " << grid[i].lon << endl;
    }
    cout << endl;

    cout << num_vertices << endl;
    for (int i = 1; i <= num_vertices; i++){
        cout << vertices[i].lat << " " << vertices[i].lon << endl;
    }
    cout << endl;

    cout << num_edges << endl;
    for (int i = 1; i <= num_edges; i++){
        cout << edges[i].u << " " << edges[i].v << " " << edges[i].dis << endl;
    }
    cout << endl;

    cout << num_graverows << endl;
    for (int i = 1; i <= num_graverows; i++){
        cout << grave_rows[i].area_name << " ";
        cout << grave_rows[i].far_west << " " << grave_rows[i].far_east << endl;
    }
    cout << endl;

    cout << num_suggest << endl;
    for (int c = 1; c <= NUM_CELLS; c++){
        for (auto &v:suggest[c]){
            cout << c << " " << v << endl;
        }
    }
}

void generate_single_test(){
    testcase_type test = generate_input();
    write_input(test);
}

void generate_multi_test(){
    for (int i = 0; i < NUM_TESTCASES; i++){
        generate_single_test();
    }
}

int main(){
    random_device rd;
    srand((unsigned int ) time(0));
    mt19937 rng(chrono::steady_clock().now().time_since_epoch().count());

    read_data();
    pre_generation();
    generate_multi_test();

    return 0;
}
