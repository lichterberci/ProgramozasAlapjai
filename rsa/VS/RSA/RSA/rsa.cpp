#include <iostream>
#include <string>
#include <vector>
#include <sstream>

/*

Implementation of a dynamic integer, where the least significant byte has the index of 0

*/
class Number {
protected:
    int m_size;
    std::vector<uint8_t> m_data;
public:
    Number(const uint64_t n)
        : m_size(4)
    {
        m_data = std::vector<uint8_t>(4);
        m_data.push_back((n && 0x000000FF));
        m_data.push_back((n && 0x0000FF00) >> 8);
        m_data.push_back((n && 0x00FF0000) >> 16);
        m_data.push_back((n && 0xFF000000) >> 24);
    }

    Number(const int _size, const std::vector<uint8_t>& _data)
        : m_size(m_size) {
        m_data.resize(_size);
        m_data = _data;
    }

    const int GetSize() const { return m_size; }

    void PrintHex() {
        std::cout << std::hex;
        for (uint8_t& i : m_data)
            std::cout << i;
        std::cout << std::dec << std::endl;
    }

    std::string ToString() {
        std::stringstream outputStream;
        outputStream << std::hex;
        for (uint8_t& i : m_data)
            outputStream << (uint16_t)i;
        return outputStream.str();
    }

    bool operator==(const Number& right) {
        int maxSize = std::max(m_size, right.GetSize());
        for (int i = 0; i < maxSize; i++) {
            if (m_data[i] != right[i]) return false;
        }
        return true;
    }

    bool operator!=(const Number& right) {
        int maxSize = std::max(m_size, right.GetSize());
        for (int i = 0; i < maxSize; i++) {
            if (m_data[i] != right[i]) return true;
        }
        return false;
    }

    const uint8_t& operator[] (int i) const {
        // assert(i >= 0 && i <= m_size);
        return m_data[i];
    }

    const Number& operator= (const Number& right) const {
        return Number(right.m_size, right.m_data);
    }

    const Number& operator+ (const Number& right) const {

        int resultSize = std::max(right.m_size, m_size) + 1;
        std::vector<uint8_t> resultData;

        uint8_t carry = 0;
        for (int i = 0; i < resultSize; i++) {
            uint8_t a = i < m_size ? m_data[i] : 0;
            uint8_t b = i < right.m_size ? right.m_data[i] : 0;
            uint16_t sum = a + b + carry;
            carry = sum / 256;
            sum %= 256;
            resultData.push_back((uint8_t)sum);
        }

        return Number(resultSize, resultData);
    }
};


int main(int argc, char** argv) {

    std::cout << "Hello, RSA!" << std::endl;

    Number n(4);
    Number m(5);
    std::cout << (uint16_t)n[0] << (uint16_t)n[1] << (uint16_t)n[2] << (uint16_t)n[3] << std::endl;
    Number r = n + m;
    std::cout << r.ToString() << std::endl;

    return 0;
}


