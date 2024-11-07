#include <iostream>
#include <vector>
#include <deque>
#include <unordered_map>

enum ReplacementPolicy { LRU, FIFO };

class Cache {
public:
    Cache(int cache_size, int block_size, int associativity, ReplacementPolicy policy)
        : cache_size(cache_size), block_size(block_size), associativity(associativity), policy(policy) {
        num_blocks = cache_size / block_size;
        num_sets = num_blocks / associativity;
        cache.resize(num_sets);
    }

    bool access(int address) {
        int block_address = address / block_size;
        int set_index = block_address % num_sets;

        if (policy == LRU) {
            return accessLRU(set_index, block_address);
        } else {
            return accessFIFO(set_index, block_address);
        }
    }

private:
    int cache_size;
    int block_size;
    int associativity;
    int num_blocks;
    int num_sets;
    ReplacementPolicy policy;

    // Cache storage for each set (either a deque for FIFO or a map for LRU)
    std::vector<std::deque<int>> cache;
    std::unordered_map<int, std::deque<int>::iterator> lru_map;

    bool accessLRU(int set_index, int block_address) {
        auto& cache_set = cache[set_index];
        
        // Cache hit
        if (lru_map.count(block_address) > 0) {
            // Move the block to the back (most recently used position)
            cache_set.erase(lru_map[block_address]);
            cache_set.push_back(block_address);
            lru_map[block_address] = --cache_set.end();
            return true;
        }

        // Cache miss
        if (cache_set.size() >= associativity) {
            int removed_block = cache_set.front();
            cache_set.pop_front();
            lru_map.erase(removed_block);
        }

        // Insert the new block and update LRU map
        cache_set.push_back(block_address);
        lru_map[block_address] = --cache_set.end();
        return false;
    }

    bool accessFIFO(int set_index, int block_address) {
        auto& cache_set = cache[set_index];

        // Cache hit
        for (const int& block : cache_set) {
            if (block == block_address) return true;
        }

        // Cache miss
        if (cache_set.size() >= associativity) {
            cache_set.pop_front();
        }

        // Insert the new block
        cache_set.push_back(block_address);
        return false;
    }
};

int main() {
    int cache_size = 64;         // Cache size in bytes
    int block_size = 4;          // Block size in bytes
    int associativity = 2;       // Set associativity (2 for 2-way set associative)
    ReplacementPolicy policy = LRU;

    Cache cache(cache_size, block_size, associativity, policy);

    std::vector<int> addresses = {0, 4, 8, 0, 4, 16, 20, 8, 4, 24};

    for (int address : addresses) {
        if (cache.access(address)) {
            std::cout << "Address " << address << " is a hit.\n";
        } else {
            std::cout << "Address " << address << " is a miss.\n";
        }
    }

    return 0;
}
