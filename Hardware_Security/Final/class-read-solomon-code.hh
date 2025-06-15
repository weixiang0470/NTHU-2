#ifndef CLASS_READ_SOLOMON_CODE_H
#define CLASS_READ_SOLOMON_CODE_H

#include <iostream>
#include <iomanip>
#include <vector>
#include <cstdint>
#include <cstring>
#include <random>

using namespace std;

typedef unsigned long RS_WORD;

static const unsigned int primes[] = {0, 0, 0, 11, 19, 37, 67, 131, 285};

class GaloisField {
public:
    std::vector<RS_WORD> powTable;
    std::vector<RS_WORD> logTable;
    int fieldPower;
    RS_WORD characteristic;
    RS_WORD primitivePoly;

    GaloisField(int fieldPower);
    RS_WORD mult(RS_WORD a, RS_WORD b) const;
    RS_WORD div(RS_WORD a, RS_WORD b) const;
    RS_WORD inv(RS_WORD x) const;

private:
    void initializeTables();
};

inline GaloisField::GaloisField(int power)
    : fieldPower(power)
{
    primitivePoly = primes[fieldPower];
    characteristic = 1;
    for (int i = 0; i < fieldPower; ++i) {
        characteristic *= 2;
    }
    characteristic -= 1;

    powTable.resize(characteristic * 2, 0);
    logTable.resize(characteristic + 1, 0);
    initializeTables();
}

inline void GaloisField::initializeTables() {
    RS_WORD value = 1;

    for (RS_WORD i = 0; i < characteristic; ++i) {
        powTable[i] = value;
        logTable[value] = i;

        RS_WORD next = value * 2;
        if (next >= (characteristic + 1)) {
            next = next ^ primitivePoly;
        }
        value = next;
    }

    // Copy for overflow access
    for (RS_WORD i = 0; i < characteristic; ++i) {
        powTable[i + characteristic] = powTable[i];
    }
}

inline RS_WORD GaloisField::mult(RS_WORD a, RS_WORD b) const {
    if (a == 0 || b == 0) return 0;
    int sumIndex = logTable[a] + logTable[b];
    return powTable[sumIndex];
}

inline RS_WORD GaloisField::div(RS_WORD a, RS_WORD b) const {
    if (a == 0) return 0;
    if (b == 0) return static_cast<RS_WORD>(-1);  // error
    int diff = logTable[a] - logTable[b];
    if (diff < 0) diff += characteristic;
    return powTable[diff];
}

inline RS_WORD GaloisField::inv(RS_WORD x) const {
    if (x == 0) return 0;
    int inverseIndex = characteristic - logTable[x];
    return powTable[inverseIndex];
}

class Poly {
public:
    int n;
    RS_WORD* coef;
    Poly();
    Poly(int n, RS_WORD* data);
    ~Poly();
    void setCopy(int n, RS_WORD* src);
    void setRef(int n, RS_WORD* src);
};

inline Poly::Poly() : n(0), coef(nullptr) {}

inline Poly::Poly(int n, RS_WORD* data) : n(n), coef(nullptr) {
    coef = new RS_WORD[n]();
    if (data) {
        std::copy(data, data + n, coef);
    }
}

inline Poly::~Poly() {
    delete[] coef;
}

inline void Poly::setCopy(int n, RS_WORD* src) {
    delete[] coef;
    this->n = n;
    coef = new RS_WORD[n]();
    if (src) {
        std::copy(src, src + n, coef);
    }
}

inline void Poly::setRef(int n, RS_WORD* src) {
    delete[] coef;
    this->n = n;
    coef = src;
}

#define MAX(x, y) ((x) > (y) ? (x) : (y))

inline void Poly_Add(Poly* out, Poly* a, Poly* b) {
    int maxSize = MAX(a->n, b->n);
    std::vector<RS_WORD> result(maxSize, 0);
    for (int i = 0; i < a->n; ++i)
        result[maxSize - a->n + i] = a->coef[i];
    for (int i = 0; i < b->n; ++i)
        result[maxSize - b->n + i] ^= b->coef[i];
    out->setCopy(maxSize, result.data());
}

inline void Poly_Scale(Poly* out, Poly* in, RS_WORD scale, GaloisField* gf) {
    std::vector<RS_WORD> result(in->n);
    for (int i = 0; i < in->n; ++i)
        result[i] = gf->mult(in->coef[i], scale);
    out->setCopy(in->n, result.data());
}

inline void Poly_Mult(Poly* out, Poly* a, Poly* b, GaloisField* gf) {
    std::vector<RS_WORD> result(a->n + b->n - 1, 0);
    for (int i = 0; i < a->n; ++i) {
        for (int j = 0; j < b->n; ++j) {
            result[i + j] ^= gf->mult(a->coef[i], b->coef[j]);
        }
    }
    out->setCopy(result.size(), result.data());
}

inline void Poly_Div(Poly* quotient, Poly* remainder, Poly* a, Poly* b, GaloisField* gf) {
    std::vector<RS_WORD> temp(a->coef, a->coef + a->n);
    std::vector<RS_WORD> q(a->n, 0);
    RS_WORD normalizer = b->coef[0];

    for (int i = 0; i <= a->n - b->n; ++i) {
        RS_WORD coef = gf->div(temp[i], normalizer);
        q[i] = coef;
        for (int j = 0; j < b->n; ++j) {
            temp[i + j] ^= gf->mult(b->coef[j], coef);
        }
    }

    if (quotient) quotient->setCopy(a->n - b->n + 1, q.data());
    if (remainder) remainder->setCopy(b->n - 1, temp.data() + a->n - b->n + 1);
}

inline RS_WORD Poly_Eval(Poly* poly, RS_WORD x, GaloisField* gf) {
    RS_WORD result = 0;
    RS_WORD power = 1;
    for (int i = poly->n - 1; i >= 0; --i) {
        result ^= gf->mult(poly->coef[i], power);
        power = gf->mult(power, x);
    }
    return result;
}

inline void Poly_ChienSearch(std::vector<unsigned int>* out, Poly* poly, int max, GaloisField* gf) {
    for (int i = 0; i < max; ++i) {
        RS_WORD result = Poly_Eval(poly, gf->powTable[i], gf);
        if (result == 0) {
            out->push_back(i);
        }
    }
}

inline void Poly_Pad(Poly* poly, int left, int right) {
    std::vector<RS_WORD> result(poly->n + left + right, 0);
    std::copy(poly->coef, poly->coef + poly->n, result.begin() + left);
    poly->setCopy(result.size(), result.data());
}

inline void Poly_Trim(Poly* poly, int left, int right) {
    std::vector<RS_WORD> result(poly->coef + left, poly->coef + poly->n - right);
    poly->setCopy(result.size(), result.data());
}

inline void Poly_Reverse(Poly* out, Poly* in) {
    std::vector<RS_WORD> result(in->n);
    for (int i = 0; i < in->n; ++i) {
        result[i] = in->coef[in->n - i - 1];
    }
    out->setCopy(result.size(), result.data());
}

class ReedSolomon {
public:
    GaloisField gf;
    ReedSolomon(int fieldPower);
    void createGenerator(Poly* out, int nsym);
    void encode(RS_WORD* out, RS_WORD* data, int k, int nsym);
    void calcSyndromes(Poly* out, Poly* msg, int nsym);
    bool findErrorLocator(Poly* out, Poly* synd, int nsym);
    bool findErrors(std::vector<unsigned int>* out, Poly* errLoc, int n);
    void forneySyndromes(Poly* out, Poly* synd);
    bool correctErrata(Poly* msg, Poly* synd, std::vector<unsigned int>* errPos);
    bool decode(RS_WORD* out, RS_WORD* data, int k, int nsym);
};

inline ReedSolomon::ReedSolomon(int fieldPower) : gf(fieldPower) {}
inline void ReedSolomon::createGenerator(Poly* out, int nsym) {
    std::vector<RS_WORD> generator = {1};
    for (int i = 0; i < nsym; ++i) {
        std::vector<RS_WORD> factor = {this->gf.powTable[i], 1};
        std::vector<RS_WORD> newGenerator(generator.size() + 1, 0);
        for (int j = 0; j < generator.size(); ++j) {
            newGenerator[j] ^= this->gf.mult(generator[j], factor[1]);
            newGenerator[j + 1] ^= this->gf.mult(generator[j], factor[0]);
        }
        generator = std::move(newGenerator);
    }
    out->setCopy(generator.size(), generator.data());
}
inline void ReedSolomon::encode(RS_WORD* out, RS_WORD* data, int k, int nsym) {
    Poly msg(k + nsym, nullptr);
    memcpy(msg.coef, data, sizeof(RS_WORD) * k);
    memset(msg.coef + k, 0, sizeof(RS_WORD) * nsym);

    Poly generator;
    this->createGenerator(&generator, nsym);

    for (int i = 0; i < k; ++i) {
        RS_WORD coef = msg.coef[i];
        if (coef == 0) continue;
        for (int j = 0; j < generator.n; ++j) {
            msg.coef[i + j] ^= this->gf.mult(coef, generator.coef[j]);
        }
    }

    memcpy(out, data, sizeof(RS_WORD) * k);
    memcpy(out + k, msg.coef + k, sizeof(RS_WORD) * nsym);
}
inline void ReedSolomon::calcSyndromes(Poly* out, Poly* msg, int nsym) {
    std::vector<RS_WORD> syndromes(nsym + 1, 0);
    for (int i = 0; i < nsym; ++i) {
        RS_WORD sum = 0;
        RS_WORD x = 1;
        for (int j = msg->n - 1; j >= 0; --j) {
            sum ^= this->gf.mult(msg->coef[j], x);
            x = this->gf.mult(x, this->gf.powTable[i]);
        }
        syndromes[nsym - i - 1] = sum;
    }
    out->setCopy(nsym + 1, syndromes.data());
}
inline bool ReedSolomon::findErrorLocator(Poly* out, Poly* synd, int nsym) {
    // Berlekamp-Massey algorithm
    std::vector<RS_WORD> errLoc = {1};
    std::vector<RS_WORD> prevLoc = {1};
    int L = 0, m = 1;
    RS_WORD b = 1;

    for (int n = 0; n < nsym; ++n) {
        RS_WORD d = synd->coef[n];
        for (int i = 1; i <= L; ++i)
            d ^= this->gf.mult(errLoc[i], synd->coef[n - i]);
        if (d == 0) {
            ++m;
        } else {
            std::vector<RS_WORD> tmp = errLoc;
            RS_WORD coef = this->gf.div(d, b);
            // Shift prevLoc by m and scale
            std::vector<RS_WORD> shifted(prevLoc.size() + m, 0);
            for (int i = 0; i < prevLoc.size(); ++i)
                shifted[i + m] = this->gf.mult(prevLoc[i], coef);
            // Pad errLoc if needed
            if (shifted.size() > errLoc.size())
                errLoc.resize(shifted.size(), 0);
            for (int i = 0; i < shifted.size(); ++i)
                errLoc[i] ^= shifted[i];
            if (2 * L <= n) {
                L = n + 1 - L;
                prevLoc = tmp;
                b = d;
                m = 1;
            } else {
                ++m;
            }
        }
    }
    // Remove leading zeros
    int leading = 0;
    while (leading < errLoc.size() && errLoc[leading] == 0) ++leading;
    if (leading == errLoc.size()) {
        out->setCopy(1, nullptr);
        out->coef[0] = 1;
        return true;
    }
    int errs = errLoc.size() - leading - 1;
    if (errs * 2 > nsym)
        return false;
    out->setCopy(errLoc.size() - leading, errLoc.data() + leading);
    return true;
}
inline bool ReedSolomon::findErrors(std::vector<unsigned int>* out, Poly* errLoc, int n) {
    int numErrors = errLoc->n - 1;
    Poly reversed;
    Poly_Reverse(&reversed, errLoc);

    std::vector<unsigned int> positions;
    for (int i = 0; i < n; ++i) {
        RS_WORD sum = 0;
        RS_WORD x = this->gf.powTable[i];
        RS_WORD x_pow = 1;
        for (int j = reversed.n - 1; j >= 0; --j) {
            sum ^= this->gf.mult(reversed.coef[j], x_pow);
            x_pow = this->gf.mult(x_pow, x);
        }
        if (sum == 0) {
            positions.push_back(i);
        }
    }

    if ((int)positions.size() != numErrors)
        return false;

    for (int i = 0; i < positions.size(); ++i) {
        if (positions[i] >= (unsigned int)n)
            return false;
        out->push_back(n - positions[i] - 1);
    }
    return true;
}
inline void ReedSolomon::forneySyndromes(Poly* out, Poly* synd) {
    Poly fsynd(synd->n - 1, synd->coef);
    out->setCopy(fsynd.n, fsynd.coef);
}
inline bool ReedSolomon::correctErrata(Poly* msg, Poly* synd, std::vector<unsigned int>* errPos) {
    // Build error locator polynomial from error positions
    Poly errLoc(1, nullptr); errLoc.coef[0] = 1;
    for (auto pos : *errPos) {
        RS_WORD root = this->gf.powTable[msg->n - 1 - pos];
        RS_WORD factor_coef[2] = {root, 1};
        Poly factor(2, factor_coef);
        Poly_Mult(&errLoc, &errLoc, &factor, &this->gf);
    }

    // Compute error evaluator polynomial
    Poly errEval;
    Poly_Mult(&errEval, synd, &errLoc, &this->gf);
    Poly_Trim(&errEval, errEval.n - errLoc.n, 0);

    // Prepare error magnitude calculation
    std::vector<RS_WORD> x(errPos->size());
    for (int i = 0; i < errPos->size(); ++i)
        x[i] = this->gf.powTable[msg->n - 1 - errPos->at(i)];

    Poly errorPoly(msg->n, nullptr);

    for (int i = 0; i < x.size(); ++i) {
        RS_WORD xi_inv = this->gf.powTable[this->gf.characteristic - (msg->n - 1 - errPos->at(i))];
        RS_WORD denom = 1;
        for (int j = 0; j < x.size(); ++j) {
            if (i != j) {
                RS_WORD term = this->gf.mult(xi_inv, x[j]);
                denom = this->gf.mult(denom, 1 ^ term);
            }
        }
        if (denom == 0) return false;
        RS_WORD magnitude = this->gf.mult(x[i], Poly_Eval(&errEval, xi_inv, &this->gf));
        errorPoly.coef[errPos->at(i)] = this->gf.div(magnitude, denom);
    }

    Poly_Add(msg, msg, &errorPoly);
    return true;
}
inline bool ReedSolomon::decode(RS_WORD* out, RS_WORD* data, int k, int nsym) {
    Poly codeword(k + nsym, data);
    Poly syndromes;
    this->calcSyndromes(&syndromes, &codeword, nsym);

    bool error_detected = false;
    for (int i = 0; i < syndromes.n; ++i) {
        if (syndromes.coef[i] != 0) {
            error_detected = true;
            break;
        }
    }

    if (error_detected) {
        Poly locator, synd_trimmed;
        std::vector<unsigned int> error_positions;

        // Prepare syndromes for error locator search
        this->forneySyndromes(&synd_trimmed, &syndromes);

        // Find error locator polynomial
        if (!this->findErrorLocator(&locator, &synd_trimmed, nsym))
            return false;

        // Find error positions
        if (!this->findErrors(&error_positions, &locator, k + nsym))
            return false;

        // Attempt to correct errors
        if (!this->correctErrata(&codeword, &syndromes, &error_positions))
            return false;
    }

    if (out != nullptr)
        std::copy(codeword.coef, codeword.coef + k, out);

    return true;
}

// ========== Reed-Solomon Code Class ==========
class RSCode {
public:
    static const int k = 8;
    static const int n = 12;
    static const int nsym = n - k;
    ReedSolomon rs;

    RSCode();
    uint32_t get_parity(uint64_t data);
    uint64_t correct(uint64_t data, uint32_t parity);
    uint64_t inject_two_byte_errors(uint64_t data);
};

inline RSCode::RSCode() : rs(8) {}

inline uint32_t RSCode::get_parity(uint64_t data) {
    RS_WORD data_bytes[k];
    for (int i = 0; i < k; ++i)
        data_bytes[k - 1 - i] = (data >> (i * 8)) & 0xFF;
    RS_WORD encoded[n] = {0};
    rs.encode(encoded, data_bytes, k, nsym);
    uint32_t parity = 0;
    for (int i = 0; i < nsym; ++i)
        parity = (parity << 8) | (encoded[k + i] & 0xFF);

    return parity;


}

inline uint64_t RSCode::correct(uint64_t data, uint32_t parity) {
    RS_WORD data_bytes[k];
    for (int i = 0; i < k; ++i)
        data_bytes[k - 1 - i] = (data >> (i * 8)) & 0xFF;
    RS_WORD encoded[n] = {0};
    rs.encode(encoded, data_bytes, k, nsym);
    for (int i = 0; i < nsym; ++i)
        encoded[k + i] = (parity >> ((nsym - 1 - i) * 8)) & 0xFF;
    RS_WORD decoded[k] = {0};
    bool success = rs.decode(decoded, encoded, k, nsym);
    if (!success) {
        std::cout << "Decoding failed!" << std::endl;
        return 0;
    }
    uint64_t corrected_data = 0;
    for (int i = 0; i < k; ++i)
        corrected_data = (corrected_data << 8) | (decoded[i] & 0xFF);
    return corrected_data;
}

inline uint64_t RSCode::inject_two_byte_errors(uint64_t data) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> pos_dist(0, 7);
    std::uniform_int_distribution<> val_dist(1, 255);

    int pos1 = pos_dist(gen);
    int pos2;
    do {
        pos2 = pos_dist(gen);
    } while (pos2 == pos1);

    uint8_t err1 = val_dist(gen);
    uint8_t err2 = val_dist(gen);

    data ^= (uint64_t(err1) << (pos1 * 8));
    data ^= (uint64_t(err2) << (pos2 * 8));
    return data;
}


// ========== example usage ==========
// int main() {
//     RSCode rscode;
//     uint64_t data = 0xDEADC0DE1234ABCD;
//     std::cout << "原始資料: 0x" << std::hex << std::setw(16) << std::setfill('0') << data << std::endl;
//     uint32_t parity = rscode.get_parity(data);
//     std::cout << "產生的parity: 0x" << std::hex << std::setw(8) << std::setfill('0') << parity << std::endl;
//     uint64_t corrupted = rscode.inject_two_byte_errors(data);
//     std::cout << "損壞後資料: 0x" << std::hex << std::setw(16) << std::setfill('0') << corrupted << std::endl;
//     uint64_t recovered = rscode.correct(corrupted, parity);
//     std::cout << "修正後資料: 0x" << std::hex << std::setw(16) << std::setfill('0') << recovered << std::endl;
//     return 0;
// }
// ========================================

#endif // CLASS_READ_SOLOMON_CODE_H
