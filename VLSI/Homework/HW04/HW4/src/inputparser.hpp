#pragma once
#include "BStarTree.hpp"

struct HardBlock {
    std::string name;
    int64_t width;
    int64_t height;
    int64_t x=0;
    int64_t y=0;
    bool rotate=false;
    int64_t W() const { return rotate ? height : width; }
    int64_t H() const { return rotate ? width : height; }
};

struct SymPair {
    std::string block1;
    std::string block2;
};

struct SymSelf {
    std::string block;
};

struct SymGroup {
    std::string name;
    std::vector<SymPair> pairs;
    std::vector<SymSelf> selfs;
};

class InputParser {
private:
    std::unordered_map<std::string, HardBlock> hardBlocks;
    std::vector<SymGroup> symGroups;
    std::vector<std::string> freeBlocks;

    void parseHardBlocks(std::ifstream& file, int64_t numBlocks) {
        std::string line;
        for (int64_t i = 0; i < numBlocks; ) {
            if (!std::getline(file, line)) {
                throw std::runtime_error("Unexpected end of file while reading HardBlocks");
            }
            if (line.empty() || line.substr(0, 2) == "//") continue;

            std::istringstream iss(line);
            std::string token;
            iss >> token;
            if (token == "HardBlock") {
                HardBlock hb;
                if (!(iss >> hb.name >> hb.width >> hb.height)) {
                    throw std::runtime_error("Invalid HardBlock format: " + line);
                }
                hardBlocks[hb.name] = hb;
                freeBlocks.emplace_back(hb.name);
                ++i;
            }
        }
    }

    void parseSymGroups(std::ifstream& file, int64_t numGroups) {
        std::string line;
        std::unordered_set<std::string> usedBlocks;
        for (int64_t i = 0; i < numGroups; ) {
            if (!std::getline(file, line)) {
                throw std::runtime_error("Unexpected end of file while reading SymGroups");
            }
            if (line.empty() || line.substr(0, 2) == "//") continue;
            std::istringstream iss(line);
            std::string token;
            iss >> token;
            if (token == "SymGroup") {
                SymGroup sg;
                int64_t numItems;
                if (!(iss >> sg.name >> numItems)) {
                    throw std::runtime_error("Invalid SymGroup format: " + line);
                }

                for (int64_t j = 0; j < numItems; ) {
                    if (!std::getline(file, line)) {
                        throw std::runtime_error("Unexpected end of file in SymGroup items");
                    }
                    if (line.empty() || line.substr(0, 2) == "//") continue;

                    std::istringstream itemIss(line);
                    std::string itemToken;
                    itemIss >> itemToken;
                    if (itemToken == "SymPair") {
                        SymPair sp;
                        if (!(itemIss >> sp.block1 >> sp.block2)) {
                            throw std::runtime_error("Invalid SymPair format: " + line);
                        }
                        sg.pairs.push_back(sp);
                        usedBlocks.insert(sp.block1);
                        usedBlocks.insert(sp.block2);
                        ++j;
                    } else if (itemToken == "SymSelf") {
                        SymSelf ss;
                        if (!(itemIss >> ss.block)) {
                            throw std::runtime_error("Invalid SymSelf format: " + line);
                        }
                        sg.selfs.push_back(ss);
                        usedBlocks.insert(ss.block);
                        ++j;
                    }
                }
                symGroups.push_back(sg);
                ++i;
            }
        }
        // Then remove those blocks from allBlocks
        freeBlocks.erase(
            std::remove_if(
                freeBlocks.begin(), 
                freeBlocks.end(),
                [&usedBlocks](const std::string& block) {
                    return usedBlocks.find(block) != usedBlocks.end();
                }
            ),
            freeBlocks.end()
        );
    }

public:
    void parseInput(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open file: " + filename);
        }

        std::string line;
        while (std::getline(file, line)) {
            if (line.empty() || line.substr(0, 2) == "//") continue;

            std::istringstream iss(line);
            std::string token;
            iss >> token;

            if (token == "NumHardBlocks") {
                int64_t numBlocks;
                if (!(iss >> numBlocks)) {
                    throw std::runtime_error("Invalid NumHardBlocks format");
                }
                parseHardBlocks(file, numBlocks);
            } else if (token == "NumSymGroups") {
                int64_t numGroups;
                if (!(iss >> numGroups)) {
                    throw std::runtime_error("Invalid NumSymGroups format");
                }
                parseSymGroups(file, numGroups);
            }
        }
    }

    const std::unordered_map<std::string, HardBlock>& getHardBlocks() const {
        return hardBlocks;
    }

    const std::vector<SymGroup>& getSymGroups() const {
        return symGroups;
    }

    void printInputSummary() const {
        std::cout << "=== Hard Blocks ===" << std::endl;
        for (const auto& pair : hardBlocks) {
            const HardBlock& hb = pair.second;
            std::cout << hb.name << ": " << hb.width << " x " << hb.height << std::endl;
        }

        std::cout << "\n=== Symmetry Groups ===" << std::endl;
        for (const auto& sg : symGroups) {
            std::cout << "Group " << sg.name << ":" << std::endl;
            for (const auto& sp : sg.pairs) {
                std::cout << "  Pair: " << sp.block1 << " <-> " << sp.block2 << std::endl;
            }
            for (const auto& ss : sg.selfs) {
                std::cout << "  Self: " << ss.block << std::endl;
            }
        }
    }

    std::vector<std::string>& getfreeNodes() {
        return freeBlocks;
    }
};