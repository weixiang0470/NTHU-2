#ifndef CLASS_HAMMING_CODE_H
#define CLASS_HAMMING_CODE_H

#include <cstdint>
#include <random>
#include <ctime>
#include <iostream>
#include <iomanip>

class HammingCode {
private:
    static const int total_bits = 71;
    static const int parity_count = 7;
    int parity_positions[parity_count] = {1, 2, 4, 8, 16, 32, 64};

public:
    uint8_t encode(uint64_t data) {
        uint8_t parity = 0;
        int bits[total_bits + 1] = {0};
        int data_idx = 0;
        for (int i = 1; i <= total_bits; i++) {
            bool is_parity_pos = false;
            for (int j = 0; j < parity_count; j++) {
                if (i == parity_positions[j]) {
                    is_parity_pos = true;
                    break;
                }
            }
            if (!is_parity_pos) {
                bits[i] = (data >> data_idx) & 1;
                data_idx++;
            }
        }
        for (int i = 0; i < parity_count; i++) {
            int p = parity_positions[i];
            int parity_bit = 0;
            for (int j = 1; j <= total_bits; j++) {
                if (j & p) {
                    parity_bit ^= bits[j];
                }
            }
            parity |= (parity_bit << i);
        }
        return parity;
    }

    uint64_t decode(uint64_t corrupted_data, uint8_t received_parity) {
        int bits[total_bits + 1] = {0};
        int data_idx = 0;
        for (int i = 1; i <= total_bits; i++) {
            bool is_parity_pos = false;
            for (int j = 0; j < parity_count; j++) {
                if (i == parity_positions[j]) {
                    is_parity_pos = true;
                    break;
                }
            }
            if (!is_parity_pos) {
                bits[i] = (corrupted_data >> data_idx) & 1;
                data_idx++;
            }
        }
        for (int i = 0; i < parity_count; i++) {
            int pos = parity_positions[i];
            bits[pos] = (received_parity >> i) & 1;
        }
        int syndrome = 0;
        for (int i = 0; i < parity_count; i++) {
            int p = parity_positions[i];
            int parity_bit = 0;
            for (int j = 1; j <= total_bits; j++) {
                if (j & p) {
                    parity_bit ^= bits[j];
                }
            }
            if (parity_bit) {
                syndrome |= p;
            }
        }
        if (syndrome != 0 && syndrome <= total_bits) {
            bits[syndrome] ^= 1;
            std::cout << std::dec << "Corrected bit at position: " << syndrome << std::endl;
        } else if (syndrome > total_bits) {
            std::cout << "Error position out of bounds, uncorrectable." << std::endl;
        } else {
            std::cout << "No error detected." << std::endl;
        }
        uint64_t corrected_data = 0;
        int corrected_idx = 0;
        for (int i = 1; i <= total_bits; i++) {
            bool is_parity_pos = false;
            for (int j = 0; j < parity_count; j++) {
                if (i == parity_positions[j]) {
                    is_parity_pos = true;
                    break;
                }
            }
            if (!is_parity_pos) {
                corrected_data |= ((uint64_t)bits[i] << corrected_idx);
                corrected_idx++;
            }
        }
        return corrected_data;
    }

    uint64_t inject_random_error(uint64_t data) {
        static std::mt19937 rng(static_cast<unsigned int>(std::time(nullptr)));
        std::uniform_int_distribution<int> dist(0, 63); // 64 bits
        int pos = dist(rng);
        return data ^ (1ULL << pos);
    }
};
// ========== example usage ==========
//  int main() {
//      HammingCode hamming;
//      uint64_t data = 0x058B420000000000;
//      std::cout << "原始資料: 0x" << std::hex << std::setw(16) << std::setfill('0') << data << std::endl;
//      uint8_t parity = hamming.encode(data);
//      std::cout << "產生的parity: 0x" << std::hex << std::setw(2) << std::setfill('0') << (int)parity << std::endl;
//      uint64_t corrupted = hamming.inject_random_error(data);
//      std::cout << "隨機損壞後資料: 0x" << std::hex << std::setw(16) << std::setfill('0') << corrupted << std::endl;
//      uint64_t recovered = hamming.decode(corrupted, parity);
//      std::cout << "修正後資料: 0x" << std::hex << std::setw(16) << std::setfill('0') << recovered << std::endl;
//      return 0;
//  }
// ========================================
#endif // CLASS_HAMMING_CODE_H
