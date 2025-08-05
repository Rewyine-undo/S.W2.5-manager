#include <random>

// d����: 1d, 2d, 1d+6 �Ȃǂ��v�Z����֐�
int RollDice(int times, int add = 0) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(1, 6);

    int result = add;
    for (int i = 0; i < times; ++i) {
        result += dis(gen);
    }
    return result;
}