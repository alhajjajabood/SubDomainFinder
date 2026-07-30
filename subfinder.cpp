#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_set>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <chrono>
#include <atomic>

#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
#else
    #include <netdb.h>
    #include <arpa/inet.h>
#endif

using namespace std;
//momen
template<typename T>
class SafeQueue {
    queue<T> q;
    mutex mtx;
    condition_variable cv;
    bool finished = false;
public:
    void push(T v) {
        lock_guard<mutex> lock(mtx);
        q.push(v);
        cv.notify_one();
    }
    bool pop(T& v) {
        unique_lock<mutex> lock(mtx);
        cv.wait(lock, [this]{ return !q.empty() || finished; });
        if (finished && q.empty()) return false;
        v = q.front(); q.pop();
        return true;
    }
    void set_finished() {
        lock_guard<mutex> lock(mtx);
        finished = true;
        cv.notify_all();
    }
    size_t size() {
        lock_guard<mutex> lock(mtx);
        return q.size();
    }
};
//leen
bool resolve(const string& domain) {
    struct addrinfo hints{}, *res = nullptr;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    int status = getaddrinfo(domain.c_str(), nullptr, &hints, &res);
    if (status == 0) {
        freeaddrinfo(res);
        return true;
    }
    return false;
}

atomic<size_t> processed{0};
atomic<size_t> found{0};
chrono::steady_clock::time_point start;

void worker(SafeQueue<string>& queue,
            unordered_set<string>& discovered,
            mutex& set_mtx,
            const string& target) {
    string sub;
    while (queue.pop(sub)) {
        string full = sub + "." + target;
        if (resolve(full)) {
            lock_guard<mutex> lock(set_mtx);
            if (discovered.insert(full).second) {
                found++;
                cout << "\r[+] Found: " << full << "\n";
            }
        }
        processed++;
        if (processed % 1000 == 0) {
            auto elapsed = chrono::duration_cast<chrono::seconds>(
                chrono::steady_clock::now() - start).count();
            cout << "\r[*] Processed: " << processed << " | Found: " << found
                      << " | Time: " << elapsed << "s   " << flush;
        }
    }
}


vector<string> load_wordlist(const string& path) {
    vector<string> words;
    ifstream f(path);
    if (!f) return words;
    string line;
    while (getline(f, line)) if (!line.empty()) words.push_back(line);
    return words;
}

//abood
int main(int argc, char* argv[]) {
#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cerr << "WSAStartup failed.\n";
        return 1;
    }
#endif

    string target, wordlist_path;
    unsigned int threads = 0;

    if (argc >= 2) target = argv[1];
    if (argc >= 3) wordlist_path = argv[2];

    if (target.empty()) {
        cout << "Target domain: ";
        getline(cin, target);
    }
    if (wordlist_path.empty()) {
        cout << "Wordlist path: ";
        getline(cin, wordlist_path);
    }

    string thr_input;
    cout << "Number of threads (" << thread::hardware_concurrency() << ")";
    threads = thread::hardware_concurrency();

    auto wordlist = load_wordlist(wordlist_path);
    if (wordlist.empty()) {
        cerr << "Error: Could not load wordlist.\n";
#ifdef _WIN32
        WSACleanup();
#endif
        return 1;
    }

    cout << "\n[*] Loaded " << wordlist.size() << " subdomains.\n";
    cout << "[*] Using " << threads << " threads.\n";

    SafeQueue<string> queue;
    for (const auto& s : wordlist) queue.push(s);

    unordered_set<string> discovered;
    mutex set_mtx;
    processed = 0;
    found = 0;
    start = chrono::steady_clock::now();

    vector<thread> workers;
    for (unsigned int i = 0; i < threads; ++i) {
        workers.emplace_back(worker, ref(queue), ref(discovered),
                             ref(set_mtx), ref(target));
    }

    while (queue.size() > 0) this_thread::sleep_for(chrono::milliseconds(200));
    queue.set_finished();
    for (auto& t : workers) t.join();

    auto elapsed = chrono::duration_cast<chrono::seconds>(
        chrono::steady_clock::now() - start).count();
    cout << "\n\n[*] Done. Processed: " << wordlist.size()
              << " | Found: " << discovered.size()
              << " | Time: " << elapsed << "s\n";
    if (!discovered.empty()) {
        cout << "\n[+] Discovered:\n";
        for (const auto& d : discovered) cout << "    " << d << '\n';
    }

    cout << "\nGoodbye!\n";

#ifdef _WIN32
    WSACleanup();
#endif
    return 0;
}
