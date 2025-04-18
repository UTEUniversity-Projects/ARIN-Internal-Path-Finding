#include <bits/stdc++.h>
#define endl '\n';

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
const string testcase_input = "INPAFI.INP";
const string testcase_output = "INPAFI.OUT";

ifstream vertex_file(vertex_input_path);
ifstream edge_file(edge_input_path);
ifstream cell_vertex_file(cell_vertex_input_path);
ifstream grave_row_file(grave_row_input_path);
ifstream grid_file(grid_input_path);

ofstream input_gen(testcase_input);
ofstream output_gen(testcase_output);

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

struct path_type{
    double len;
    deque<int> nodes;
};
struct testcase_type{
    coordinate coor;
    int row_id;
};

struct output_type{
    int row, col, cell;
    int far_west, far_east;
    path_type path;
    string state = "success";
} output;

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

// --------------------------- FIND PATH START HERE ----------------------------

double dis_hor, dis_ver, dis_lat, dis_lon;

double haversine_dis(coordinate &a, coordinate &b){
    double d_lat = (b.lat - a.lat) * PI / 180.0;
    double d_lon = (b.lon - a.lon) * PI / 180.0;

    double x = (a.lat) * PI / 180.0;
    double y = (b.lat) * PI / 180.0;

    double one = pow(sin(d_lat / 2), 2) + pow(sin(d_lon / 2), 2) * cos(x) * cos(y);
    double rad = 6371.0;
    double two = 2.0 * asin(sqrt(one));
    return rad * two * 1000.0;
}

void preprocess_dis(){
    dis_hor = haversine_dis(grid[1], grid[2]);
    dis_ver = haversine_dis(grid[1], grid[4]);
    dis_lat = dis_ver / 16.0;
    dis_lon = dis_hor / 16.0;
}

int get_cell_id(coordinate &user_coor){
    double a = dis_hor, b = dis_ver;
    double c = haversine_dis(user_coor, grid[1]);
    double d = haversine_dis(user_coor, grid[2]);
    double e = haversine_dis(user_coor, grid[4]);

    // d^2 = a^2 + c^2 - 2*a*c*cos(x)
    double cos_x = (a*a + c*c - d*d) / (2.0 * a * c);
    double dis_col = cos_x * c;
    double sin_x = sqrt(1.0 - cos_x * cos_x);
    double dis_row = sin_x * c;

    int id_row = ceil(dis_row / dis_lat);
    int id_col = ceil(dis_col / dis_lon);

    output.row = id_row;
    output.col = id_col;
    
    return (id_row - 1) * 16 + id_col;
}

// ---------------------------- DIJKSTRA ALGORITHM -----------------------------

const double inf_len = 1e9;

vector<double> dis;
vector<bool> check;
vector<int> trace;

struct comp{
    bool operator() (pair<double, int> &a, pair<double, int> &b) {
        return a.first > b.first;
    }
};

double dijkstra(int &sour, int &dest){
    fill(dis.begin(), dis.end(), inf_len);
    fill(check.begin(), check.end(), false);
    fill(trace.begin(), trace.end(), -1);

    priority_queue<pair<double, int>, vector<pair<double, int>>, comp> que;
    dis[sour] = 0;
    que.push({dis[sour], sour});

    while (!que.empty()){
        auto it = que.top(); que.pop();
        int u = it.second;

        if (check[u]) continue;
        check[u] = true;

        for (auto &e:graph[u]){
            int idx = e.first;
            int v = e.second.first;
            double w =  e.second.second;

            if (dis[v] > dis[u] + w){
                dis[v] = dis[u] + w;
                que.push({dis[v], v});
                trace[v] = u;
            }

            if (v == dest) return dis[v];
        }
    }

    return 0.0;
}

deque<int> trace_path(int &sour, int &dest){
    if (dis[dest] == inf_len) return deque<int>();

    int u = dest;
    deque<int> out(1, u);
    while (trace[u] != -1){
        u = trace[u];
        out.push_back(u);
    }

    return out;
}

path_type find_path(testcase_type &test){
    int cell_id = get_cell_id(test.coor);
    output.cell = cell_id;

    // test out of grid here
    if (output.col < 1 || output.col > 16 
    ||  output.row < 1 || output.row > 16){
        output.state = "fail";
        return path_type();
    }

    dis.resize(num_vertices + 2);
    check.resize(num_vertices + 2);
    trace.resize(num_vertices + 2);

    path_type path;
    path.len = inf_len;
    int far_west = output.far_west = grave_rows[test.row_id].far_west;
    int far_east = output.far_east = grave_rows[test.row_id].far_east;

    for (auto &sour:suggest[cell_id]){
        double len_west = dijkstra(sour, far_west) + haversine_dis(vertices[sour], vertices[far_west]);
        if (path.len > len_west){
            path.len = len_west;
            path.nodes = trace_path(sour, far_west);
        }

        if (far_east == far_west) continue;

        double len_east = dijkstra(sour, far_east) + haversine_dis(vertices[sour], vertices[far_east]);
        if (path.len > len_east){
            path.len = len_east;
            path.nodes = trace_path(sour, far_east);
        }
    }

    return path;
}

// ---------------------- GENERATE OUTPUT TEST START HERE ----------------------

string get_current_timestamp() {
    time_t now = time(0);
    tm *gmtm = gmtime(&now);
    char buf[100];
    strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%S", gmtm);
    string timestamp(buf);
    timestamp += ".000000000";
    return timestamp + "Z";
}

void write_input(testcase_type &test) {
    input_gen << fixed << setprecision(12);
    input_gen << test.coor.lat << " " << test.coor.lon << " " << test.row_id << endl;
    input_gen << endl;

    input_gen << num_fars << endl;
    for (int i = 1; i <= num_fars; i++){
        input_gen << grid[i].lat << " " << grid[i].lon << endl;
    }
    input_gen << endl;

    input_gen << num_vertices << endl;
    for (int i = 1; i <= num_vertices; i++){
        input_gen << vertices[i].lat << " " << vertices[i].lon << endl;
    }
    input_gen << endl;

    input_gen << num_edges << endl;
    for (int i = 1; i <= num_edges; i++){
        input_gen << edges[i].u << " " << edges[i].v << " " << edges[i].dis << endl;
    }
    input_gen << endl;

    input_gen << num_graverows << endl;
    for (int i = 1; i <= num_graverows; i++){
        input_gen << grave_rows[i].area_name << " " << grave_rows[i].row_name << " ";
        input_gen << grave_rows[i].far_west << " " << grave_rows[i].far_east << endl;
    }
    input_gen << endl;

    input_gen << num_suggest << endl;
    for (int c = 1; c <= NUM_CELLS; c++){
        for (auto &v:suggest[c]){
            input_gen << c << " " << v << endl;
        }
    }
}

void write_output(output_type &result) {
    if (output.state == "success"){
        for (size_t i = 0; i < result.path.nodes.size(); ++i) {
            output_gen << result.path.nodes[i] << " ";
        }
        output_gen << endl;
    } else if (output.state == "fail") {
        output_gen << -1 << endl;
    }
}

void generate_single_test(){
    testcase_type test = generate_input();
    write_input(test);

    output.state = "success";
    output.path = find_path(test);
    write_output(output);
}

void generate_multi_test(){
    for (int i = 0; i < NUM_TESTCASES; i++){
        generate_single_test();
    }
}

int main(){
    ios_base::sync_with_stdio(false);
    cin.tie(0); cout.tie(0);

    read_data();
    
    random_device rd;
    srand((unsigned int ) time(0));
    pre_generation();
    preprocess_dis();
    generate_multi_test();

    return 0;
}