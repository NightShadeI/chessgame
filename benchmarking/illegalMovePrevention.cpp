#include <chrono>
#include <vector>
#include <random>
#include <iostream>

enum SomeTypes {
    A,
    B,
    C,
    D,
    E
};

std::vector<bool> small {
    
    false, false, false, false, false,
    false, false, true , true , true ,

    false, false, false, false, false,
    true , true , true , false, false,

    false, false, true , true , true ,
    false, false, false, false, false,

    true , true , true , false, false,
    false, false, false, false, false,

};

std::vector<bool> big {

    false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false,

    false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false,
    false, false, false, false, true , true , true,  false,

    false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false,
    false, false, true , true , true , false, false, false,

    false, false, false, false, true , true , true , false,
    false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false,

    false, false, true , true , true , false, false, false,
    false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false,
    
};

bool customCheck(int xPos, int yPos, SomeTypes moveType) {
    const unsigned int normY = (unsigned)(yPos - 1);
    const unsigned int normX =  (unsigned)(xPos - 2);
    return moveType > 0 && normY >= 6 && normX <= 4 && small[10 * (moveType-1) + 5 * (yPos & 1) + normX];
}

bool advancedCheck(int xPos, int yPos, SomeTypes moveType) {
    const int pos = 64 * moveType + 8 * yPos + xPos;
    return big[pos];
}

int main() {

    std::random_device rd;
    std::default_random_engine generator(rd());
    std::uniform_int_distribution<int> gridDist(0, 7);
    std::uniform_int_distribution<int> moveDist(0, 4);

    const int TIMES_TO_RUN = 1'000'000;
    int count = 0;
    double totalSum = 0;
    for (int i = 0; i < 100; i++) {
        auto startTime = std::chrono::high_resolution_clock().now();
        for (int j = 0; j < TIMES_TO_RUN; j++) {
            int boardX = gridDist(generator);
            int boardY = gridDist(generator);
            int moveType = moveDist(generator);
            if (advancedCheck(boardX, boardY, static_cast<SomeTypes>(moveType))) {
                count++;
            }
        }
        auto endTime = std::chrono::high_resolution_clock().now();
        std::chrono::duration<double, std::milli> myDuration = endTime - startTime;
        totalSum += myDuration.count();
    }
    
    std::cout << "Time taken (avg): " << totalSum/100 << "ms" << std::endl;
    std::cout << "Total collisions: " << count << std::endl;

    // std::cout << customCheck(0, 0, SomeTypes::A) << std::endl;
    // std::cout << customCheck(3, 7, SomeTypes::C) << std::endl;
    // std::cout << customCheck(5, 5, SomeTypes::C) << std::endl;
    // std::cout << customCheck(6, 0, SomeTypes::D) << std::endl;
    // std::cout << customCheck(3, 7, SomeTypes::B) << std::endl;

    // std::cout << advancedCheck(0, 0, SomeTypes::A) << std::endl;
    // std::cout << advancedCheck(3, 7, SomeTypes::C) << std::endl;
    // std::cout << advancedCheck(5, 5, SomeTypes::C) << std::endl;
    // std::cout << advancedCheck(6, 0, SomeTypes::D) << std::endl;
    // std::cout << advancedCheck(3, 7, SomeTypes::B) << std::endl;

    return 0;
}