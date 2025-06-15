#ifndef CLASS_BCH_CODE_H
#define CLASS_BCH_CODE_H

#include <cstdint>
#include <vector>
#include <random>
#include <ctime>
#include <iostream>
#include <iomanip>
#include <cmath>

class BCHCode {
    // std::mt19937 rng;
    std::mt19937 rng{static_cast<unsigned int>(std::time(nullptr))};
private:
    int m, n, length, k, t, d;
    std::vector<int> p;
    std::vector<int> alpha_to, index_of, g;
    std::vector<int> recd, data, bb;
    int numerr, decerror;
    std::vector<int> errpos;
    
    // Random number generator for error injection
    
    void generate_gf() {
        int mask = 1;
        alpha_to[m] = 0;
        for (int i = 0; i < m; i++) {
            alpha_to[i] = mask;
            index_of[alpha_to[i]] = i;
            if (p[i] != 0)
                alpha_to[m] ^= mask;
            mask <<= 1;
        }
        index_of[alpha_to[m]] = m;
        mask >>= 1;
        for (int i = m + 1; i < n; i++) {
            if (alpha_to[i - 1] >= mask)
                alpha_to[i] = alpha_to[m] ^ ((alpha_to[i - 1] ^ mask) << 1);
            else
                alpha_to[i] = alpha_to[i - 1] << 1;
            index_of[alpha_to[i]] = i;
        }
        index_of[0] = -1;
    }
    
    void gen_poly() {
        std::vector<std::vector<int>> cycle(1024, std::vector<int>(21));
        std::vector<int> size(1024), min(1024), zeros(1024);
        int ii, jj, ll, kaux;
        int test, aux, nocycles, root, noterms, rdncy;
        
        // Generate cycle sets modulo n
        cycle[0][0] = 0;
        size[0] = 1;
        cycle[1][0] = 1;
        size[1] = 1;
        jj = 1;
        
        do {
            ii = 0;
            do {
                ii++;
                cycle[jj][ii] = (cycle[jj][ii - 1] * 2) % n;
                size[jj]++;
                aux = (cycle[jj][ii] * 2) % n;
            } while (aux != cycle[jj][0]);
            
            ll = 0;
            do {
                ll++;
                test = 0;
                for (ii = 1; ((ii <= jj) && (!test)); ii++)
                    for (kaux = 0; ((kaux < size[ii]) && (!test)); kaux++)
                        if (ll == cycle[ii][kaux])
                            test = 1;
            } while ((test) && (ll < (n - 1)));
            
            if (!(test)) {
                jj++;
                cycle[jj][0] = ll;
                size[jj] = 1;
            }
        } while (ll < (n - 1));
        nocycles = jj;
        
        d = 2 * t + 1;
        
        // Search for roots 1, 2, ..., d-1 in cycle sets
        kaux = 0;
        rdncy = 0;
        for (ii = 1; ii <= nocycles; ii++) {
            min[kaux] = 0;
            test = 0;
            for (jj = 0; ((jj < size[ii]) && (!test)); jj++)
                for (root = 1; ((root < d) && (!test)); root++)
                    if (root == cycle[ii][jj]) {
                        test = 1;
                        min[kaux] = ii;
                    }
            if (min[kaux]) {
                rdncy += size[min[kaux]];
                kaux++;
            }
        }
        noterms = kaux;
        kaux = 1;
        for (ii = 0; ii < noterms; ii++)
            for (jj = 0; jj < size[min[ii]]; jj++) {
                zeros[kaux] = cycle[min[ii]][jj];
                kaux++;
            }
        
        k = length - rdncy;
        
        if (k < 0) {
            std::cerr << "Parameters invalid!" << std::endl;
            return;
        }
        
        // Compute the generator polynomial
        g[0] = alpha_to[zeros[1]];
        g[1] = 1;
        for (ii = 2; ii <= rdncy; ii++) {
            g[ii] = 1;
            for (jj = ii - 1; jj > 0; jj--)
                if (g[jj] != 0)
                    g[jj] = g[jj - 1] ^ alpha_to[(index_of[g[jj]] + zeros[ii]) % n];
                else
                    g[jj] = g[jj - 1];
            g[0] = alpha_to[(index_of[g[0]] + zeros[ii]) % n];
        }
    }
    
    void encode_bch() {
        int feedback;
        
        for (int i = 0; i < length - k; i++)
            bb[i] = 0;
        for (int i = k - 1; i >= 0; i--) {
            feedback = data[i] ^ bb[length - k - 1];
            if (feedback != 0) {
                for (int j = length - k - 1; j > 0; j--)
                    if (g[j] != 0)
                        bb[j] = bb[j - 1] ^ feedback;
                    else
                        bb[j] = bb[j - 1];
                bb[0] = g[0] && feedback;
            } else {
                for (int j = length - k - 1; j > 0; j--)
                    bb[j] = bb[j - 1];
                bb[0] = 0;
            }
        }
    }
    
    bool decode_bch() {
        int i, j, u, q, t2, count = 0, syn_error = 0;
        std::vector<std::vector<int>> elp(1026, std::vector<int>(1024));
        std::vector<int> d(1026), l(1026), u_lu(1026), s(1025);
        std::vector<int> root(200), loc(200), reg(201);
        
        t2 = 2 * t;
        
        // Form the syndromes
        for (i = 1; i <= t2; i++) {
            s[i] = 0;
            for (j = 0; j < length; j++)
                if (recd[j] != 0)
                    s[i] ^= alpha_to[(i * j) % n];
            if (s[i] != 0)
                syn_error = 1;
            s[i] = index_of[s[i]];
        }
        
        if (syn_error) {
            // Berlekamp algorithm
            d[0] = 0;
            d[1] = s[1];
            elp[0][0] = 0;
            elp[1][0] = 1;
            for (i = 1; i < t2; i++) {
                elp[0][i] = -1;
                elp[1][i] = 0;
            }
            l[0] = 0;
            l[1] = 0;
            u_lu[0] = -1;
            u_lu[1] = 0;
            u = 0;
            
            do {
                u++;
                if (d[u] == -1) {
                    l[u + 1] = l[u];
                    for (i = 0; i <= l[u]; i++) {
                        elp[u + 1][i] = elp[u][i];
                        elp[u][i] = index_of[elp[u][i]];
                    }
                } else {
                    q = u - 1;
                    while ((d[q] == -1) && (q > 0))
                        q--;
                    if (q > 0) {
                        j = q;
                        do {
                            j--;
                            if ((d[j] != -1) && (u_lu[q] < u_lu[j]))
                                q = j;
                        } while (j > 0);
                    }
                    
                    if (l[u] > l[q] + u - q)
                        l[u + 1] = l[u];
                    else
                        l[u + 1] = l[q] + u - q;
                    
                    for (i = 0; i < t2; i++)
                        elp[u + 1][i] = 0;
                    for (i = 0; i <= l[q]; i++)
                        if (elp[q][i] != -1)
                            elp[u + 1][i + u - q] = 
                                alpha_to[(d[u] + n - d[q] + elp[q][i]) % n];
                    for (i = 0; i <= l[u]; i++) {
                        elp[u + 1][i] ^= elp[u][i];
                        elp[u][i] = index_of[elp[u][i]];
                    }
                }
                u_lu[u + 1] = u - l[u + 1];
                
                if (u < t2) {
                    if (s[u + 1] != -1)
                        d[u + 1] = alpha_to[s[u + 1]];
                    else
                        d[u + 1] = 0;
                    for (i = 1; i <= l[u + 1]; i++)
                        if ((s[u + 1 - i] != -1) && (elp[u + 1][i] != 0))
                            d[u + 1] ^= alpha_to[(s[u + 1 - i] 
                                          + index_of[elp[u + 1][i]]) % n];
                    d[u + 1] = index_of[d[u + 1]];
                }
            } while ((u < t2) && (l[u + 1] <= t));
            
            u++;
            if (l[u] <= t) {
                // Put elp into index form
                for (i = 0; i <= l[u]; i++)
                    elp[u][i] = index_of[elp[u][i]];
                
                // Chien search: find roots
                for (i = 1; i <= l[u]; i++)
                    reg[i] = elp[u][i];
                count = 0;
                for (i = 1; i <= n; i++) {
                    q = 1;
                    for (j = 1; j <= l[u]; j++)
                        if (reg[j] != -1) {
                            reg[j] = (reg[j] + j) % n;
                            q ^= alpha_to[reg[j]];
                        }
                    if (!q) {
                        root[count] = i;
                        loc[count] = n - i;
                        count++;
                    }
                }
                
                if (count == l[u]) {
                    for (i = 0; i < l[u]; i++)
                        recd[loc[i]] ^= 1;
                    return true; // Successful correction
                } else {
                    std::cout << "Incomplete decoding: errors detected" << std::endl;
                    return false;
                }
            }
        }
        return true; // No errors or successful correction
    }
    
    void initialize_primitive_poly() {
        p.resize(21, 0);
        p[0] = p[m] = 1;
        if (m == 2)         p[1] = 1;
        else if (m == 3)    p[1] = 1;
        else if (m == 4)    p[1] = 1;
        else if (m == 5)    p[2] = 1;
        else if (m == 6)    p[1] = 1;
        else if (m == 7)    p[1] = 1;
        else if (m == 8)    p[4] = p[5] = p[6] = 1;
        else if (m == 9)    p[4] = 1;
        else if (m == 10)   p[3] = 1;
        else if (m == 11)   p[2] = 1;
        else if (m == 12)   p[3] = p[4] = p[7] = 1;
        else if (m == 13)   p[1] = p[3] = p[4] = 1;
        else if (m == 14)   p[1] = p[11] = p[12] = 1;
        else if (m == 15)   p[1] = 1;
        else if (m == 16)   p[2] = p[3] = p[5] = 1;
        else if (m == 17)   p[3] = 1;
        else if (m == 18)   p[7] = 1;
        else if (m == 19)   p[1] = p[5] = p[6] = 1;
        else if (m == 20)   p[3] = 1;
    }

public:
    // Constructor: Initialize BCH code with given parameters
    BCHCode(int field_size = 7, int error_capability = 5, int code_length = 99) {
        m = field_size;
        t = error_capability;
        n = (1 << m) - 1;  // 2^m - 1
        length = (code_length <= n) ? code_length : n;
        
        // Initialize arrays
        alpha_to.resize(1048576);
        index_of.resize(1048576);
        g.resize(548576);
        recd.resize(1048576);
        data.resize(1048576);
        bb.resize(548576);
        errpos.resize(1024);
        
        // Initialize primitive polynomial
        initialize_primitive_poly();
        
        // Generate Galois field and generator polynomial
        generate_gf();
        gen_poly();
        
        numerr = 0;
        decerror = 0;
        
        std::cout << "BCH(" << length << ", " << k << ", " << (2*t+1) << ") code initialized" << std::endl;
        std::cout << "Error correcting capability: " << t << " errors" << std::endl;
    }

    
    // Encode data and return parity bits
    std::vector<int> encode(const std::vector<int>& input_data) {
        if (input_data.size() != k) {
            std::cerr << "Input data size must be " << k << " bits" << std::endl;
            return std::vector<int>();
        }
        
        // Copy input data
        for (int i = 0; i < k; i++) {
            data[i] = input_data[i];
        }
        
        // Encode
        encode_bch();
        
        // Return parity bits
        std::vector<int> parity(length - k);
        for (int i = 0; i < length - k; i++) {
            parity[i] = bb[i];
        }
        
        return parity;
    }
    
    // Decode received data (data + parity)
    std::vector<int> decode(const std::vector<int>& received_data) {
        if (received_data.size() != length) {
            std::cerr << "Received data size must be " << length << " bits" << std::endl;
            return std::vector<int>();
        }
        
        // Copy received data
        for (int i = 0; i < length; i++) {
            recd[i] = received_data[i];
        }
        
        // Decode
        bool success = decode_bch();
        
        // Return decoded data portion
        std::vector<int> decoded_data(k);
        for (int i = 0; i < k; i++) {
            decoded_data[i] = recd[i + length - k];
        }
        
        return decoded_data;
    }
    
    // Inject random errors into data
    std::vector<int> inject_random_errors(const std::vector<int>& input_data, int num_errors) {
        if (num_errors > t) {
            std::cout << "Warning: Injecting " << num_errors << " errors, but code can only correct " << t << std::endl;
        }
        
        std::vector<int> corrupted_data = input_data;
        std::uniform_int_distribution<int> dist(0, 63);
        
        for (int i = 0; i < num_errors; i++) {
            int pos = dist(rng);
            corrupted_data[pos] ^= 1;
            std::cout << "Injected error at position: " << pos << std::endl;
        }
        
        return corrupted_data;
    }
    
    // Convert uint64_t to bit vector
    std::vector<int> uint64_to_bits(uint64_t value, int num_bits) {
        std::vector<int> bits(num_bits,0);
        for (int i = 0; i < num_bits; i++) {
            bits[i] = (value >> i) & 1;
        }
        return bits;
    }
    
    // Convert bit vector to uint64_t
    uint64_t bits_to_uint64(const std::vector<int>& bits) {
        uint64_t value = 0;
        for (int i = 0; i < bits.size() && i < 64; i++) {
            if (bits[i]) {
                value |= (1ULL << i);
            }
        }
        return value;
    }
    
    // Get code parameters
    int get_n() const { return length; }
    int get_k() const { return k; }
    int get_t() const { return t; }
    
    // Print code information
    void print_info() const {
        std::cout << "BCH Code Parameters:" << std::endl;
        std::cout << "  Code length (n): " << length << std::endl;
        std::cout << "  Data length (k): " << k << std::endl;
        std::cout << "  Min distance (d): " << (2*t+1) << std::endl;
        std::cout << "  Error correction capability (t): " << t << std::endl;
        std::cout << "  Galois field: GF(2^" << m << ")" << std::endl;
    }
};

// Initialize static member
// std::mt19937 BCHCode::rng(static_cast<unsigned int>(std::time(nullptr)));

// ========== Example usage ==========
// int main() {
//     // Create BCH(255, 247, 3) - can correct 1 error
//     BCHCode bch(8, 1, 255);
//     bch.print_info();
    
//     // Example data
//     uint64_t original_data = 0x0123456789ABCDEF;
//     std::cout << "Original data: 0x" << std::hex << original_data << std::endl;
    
//     // Convert to bit vector for encoding
//     std::vector<int> data_bits = bch.uint64_to_bits(original_data, bch.get_k());
    
//     // Encode
//     std::vector<int> parity = bch.encode(data_bits);
    
//     // Create codeword (data + parity)
//     std::vector<int> codeword(bch.get_n());
//     for (int i = 0; i < bch.get_n() - bch.get_k(); i++) {
//         codeword[i] = parity[i];
//     }
//     for (int i = 0; i < bch.get_k(); i++) {
//         codeword[i + bch.get_n() - bch.get_k()] = data_bits[i];
//     }
    
//     // Inject errors
//     std::vector<int> corrupted = bch.inject_random_errors(codeword, 1);
    
//     // Decode
//     std::vector<int> decoded = bch.decode(corrupted);
//     uint64_t decoded_data = bch.bits_to_uint64(decoded);
    
//     std::cout << "Decoded data: 0x" << std::hex << decoded_data << std::endl;
//     std::cout << "Correction " << (original_data == decoded_data ? "successful" : "failed") << std::endl;
    
//     return 0;
// }
// ========================================

#endif // CLASS_BCH_CODE_H