#include <iostream>
#include <cstdint>
#include <list>
#include <chrono>
#include <vector>
#include <random>

using namespace std;
using namespace std::chrono;

// --- 1. Circular List ---
struct CNode { int data; CNode* next; };
class CircularList {
    CNode* tail; int count;
public:
    CircularList() : tail(nullptr), count(0) {}
    
    // чистимо пам'ять
    ~CircularList() {
        if (!tail) return;
        CNode* curr = tail->next;
        tail->next = nullptr;
        while (curr) {
            CNode* temp = curr->next;
            delete curr;
            curr = temp;
        }
    }

    void push_back(int data) {
        CNode* newNode = new CNode{data, nullptr};
        if (!tail) { tail = newNode; tail->next = tail; } 
        else { newNode->next = tail->next; tail->next = newNode; tail = newNode; }
        count++;
    }
    int size() { return count; }
};

// --- 2. Sentinel List ---
struct DNode { int data; DNode* prev; DNode* next; };
class SentinelList {
    DNode* header; DNode* trailer; int count;
public:
    SentinelList() {
        header = new DNode{0, nullptr, nullptr};
        trailer = new DNode{0, nullptr, nullptr};
        header->next = trailer; trailer->prev = header; count = 0;
    }

    // чистимо пам'ять
    ~SentinelList() {
        DNode* curr = header;
        while (curr) {
            DNode* temp = curr->next;
            delete curr;
            curr = temp;
        }
    }

    void push_back(int data) {
        DNode* newNode = new DNode{data, trailer->prev, trailer};
        trailer->prev->next = newNode; trailer->prev = newNode; count++;
    }
    int size() { return count; }
};

// --- 3. XOR List ---
struct XORNode { int data; XORNode* npx; };
class XORList {
    XORNode* head; XORNode* tail; int count;
    XORNode* XOR(XORNode* a, XORNode* b) { return (XORNode*)((uintptr_t)(a) ^ (uintptr_t)(b)); }
public:
    XORList() : head(nullptr), tail(nullptr), count(0) {}

    // чистимо пам'ять
    ~XORList() {
        XORNode* curr = head;
        XORNode* prev = nullptr;
        XORNode* next;
        while (curr) {
            next = XOR(prev, curr->npx);
            prev = curr;
            XORNode* temp = curr;
            curr = next;
            delete temp;
        }
    }

    void push_back(int data) {
        XORNode* newNode = new XORNode{data, XOR(tail, nullptr)};
        if (!head) { head = tail = newNode; } 
        else { 
            tail->npx = XOR(tail->npx, newNode); 
            tail = newNode; 
        }
        count++;
    }
    int size() { return count; }
};

int main() {
    // 1. Проведення Unit-тестів
    cout << "=== 1. UNIT TESTS ===" << endl;
    CircularList cl_test; cl_test.push_back(1); cl_test.push_back(2);
    cout << (cl_test.size() == 2 ? "[PASS] CircularList" : "[FAIL] CircularList") << endl;

    SentinelList sl_test; sl_test.push_back(1);
    cout << (sl_test.size() == 1 ? "[PASS] SentinelList" : "[FAIL] SentinelList") << endl;

    XORList xl_test; xl_test.push_back(1); xl_test.push_back(2);
    cout << (xl_test.size() == 2 ? "[PASS] XORList" : "[FAIL] XORList") << endl;

    // 2. Аналіз пам'яті
    cout << "\n=== 2. MEMORY ANALYSIS (1 000 000 ELEMENTS) ===" << endl;
    double memC = (sizeof(CNode) * 1000000) / 1048576.0;
    double memS = (sizeof(DNode) * 1000000) / 1048576.0;
    double memX = (sizeof(XORNode) * 1000000) / 1048576.0;
    cout << "Circular List: " << memC << " MB\n";
    cout << "Sentinel List: " << memS << " MB\n";
    cout << "XOR List:      " << memX << " MB\n";

    // 3. Генерація CSV для Benchmarking
    cout << "\n=== 3. RAW DATA (SAVE AS CSV) ===" << endl;
    cout << "Elements_Count,CircularList_ms,SentinelList_ms,XORList_ms,StdList_ms" << endl;

    vector<int> data_sizes = {10000, 50000, 100000, 200000};

    // Генерація випадкових даних
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> distrib(1, 100000);

    for (int N : data_sizes) {
        cout << N << ",";
        
        // масив випадкових чисел
        vector<int> random_data(N);
        for(int i=0; i<N; i++) random_data[i] = distrib(gen);

        // Circular List
        CircularList cl; 
        auto start = high_resolution_clock::now();
        for(int i=0; i<N; i++) cl.push_back(random_data[i]);
        cout << duration<double, std::milli>(high_resolution_clock::now() - start).count() << ",";

        // Sentinel List
        SentinelList sl; 
        start = high_resolution_clock::now();
        for(int i=0; i<N; i++) sl.push_back(random_data[i]);
        cout << duration<double, std::milli>(high_resolution_clock::now() - start).count() << ",";

        // XOR List
        XORList xl; 
        start = high_resolution_clock::now();
        for(int i=0; i<N; i++) xl.push_back(random_data[i]);
        cout << duration<double, std::milli>(high_resolution_clock::now() - start).count() << ",";

        // std::list
        list<int> std_l; 
        start = high_resolution_clock::now();
        for(int i=0; i<N; i++) std_l.push_back(random_data[i]);
        cout << duration<double, std::milli>(high_resolution_clock::now() - start).count() << endl;
    }

    return 0;
}