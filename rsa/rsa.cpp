#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <memory>
#include <chrono>
#include <thread>
#include <iomanip>

/*

Implementation of a dynamic integer, where the least significant byte has the index of 0

*/
class Number {
private:
    int m_size;
    uint8_t* m_data;
public:

    Number() : m_size(0), m_data(nullptr) {}

    ~Number() {
        delete[] m_data;
    }

    Number(const uint64_t n)
        : m_size(4)
    {
        m_data = new uint8_t[4];
        m_data[0] = ((n & 0x000000FF));
        m_data[1] = ((n & 0x0000FF00) >> 8);
        m_data[2] = ((n & 0x00FF0000) >> 16);
        m_data[3] = ((n & 0xFF000000) >> 24);
    }

    Number(const int _size, uint8_t* _data)
        : m_size(_size), m_data(_data)
    { }

    // copy
    Number(const Number& other)
        : m_size(other.m_size)
    {
        m_data = new uint8_t[m_size];
        std::copy(m_data, m_data + m_size, other.m_data);
    }

    // move
    Number(Number&& other) noexcept
        : m_size(other.m_size)
    {
        m_data = other.m_data;
    }


    int GetSize() const { return m_size; }

    std::string ToString() const {
        std::stringstream outputStream;
        for (int i = m_size - 1; i >= 0; i--)
        {
            outputStream << std::hex << std::setfill('0') << std::setw(2) << (uint16_t)m_data[i];
        }
        return outputStream.str();
    }

    bool operator==(const Number& right) const {
        int maxSize = std::max(m_size, right.GetSize());
        for (int i = 0; i < maxSize; i++) {
            if (m_data[i] != right[i]) return false;
        }
        return true;
    }

    bool operator!=(const Number& right) const {
        int maxSize = std::max(m_size, right.GetSize());
        for (int i = 0; i < maxSize; i++) {
            if (m_data[i] != right[i]) return true;
        }
        return false;
    }

    uint8_t& operator[] (const int i) const {
        return m_data[i];
    }

    // move
    Number& operator= (const Number& other) {
        delete[] m_data;
        m_data = other.m_data;
        m_size = other.m_size;
        return *this;
    }

    Number operator+ (const Number& other) const {

        const int resultSize = std::max(m_size, other.m_size) + 1;
        uint8_t* resultData = new uint8_t[resultSize];

        uint8_t carry = 0;
        for (int i = 0; i < resultSize; i++) {

            uint8_t a = i < m_size ? m_data[i] : 0;
            uint8_t b = i < other.m_size ? other.m_data[i] : 0;

            uint16_t sum = a + b + carry;

            carry = sum / 256;
            sum %= 256;

            resultData[i] = (uint8_t)sum;
        }

        return Number(resultSize, resultData);
    }

    Number& operator+= (const Number& other) {
        Number sum = *this + other;
        delete[] m_data;
        m_data = sum.m_data;
        m_size = sum.m_size;
        return *this;
    }

    Number& operator <<= (const int n) {

        for (int j = 0; j < n; j++) {

            if (m_size % 8 == 7)
                m_size++;

            uint8_t* result = new uint8_t[m_size];

            uint8_t carry = 0;

            for (size_t i = 0; i < m_size * 8; i++)
            {
                uint8_t nextCarry = (m_data[i / 8] & 0x80) >> 7;
                result[i / 8] = m_data[i / 8] << 1;
                result[i / 8] |= carry;
                carry = nextCarry;
            }

            delete[] m_data;
            m_data = result;

            if (carry == 1)
                std::cout << "[ERROR] Carried out a 1 when shifting!" << std::endl;
        }

        return *this;
    }

    Number operator* (const Number& other) const {

        const int resultSize = m_size + other.m_size;
        uint8_t* resultData = new uint8_t[resultSize];

        Number result(1);

        for (size_t i = 0; i < m_size; i++)
        {
            /*uint8_t currentByte = (*this)[i / 8];
            uint8_t currentBit = (currentByte >> (i % 8)) & 0x01;*/
            result <<= 1;
            //result += other;
        }

        return Number(resultSize, resultData);
    }

};

std::ostream& operator<<(std::ostream& out, const Number& right) {
    return out << right.ToString();
}

int main(int argc, char** argv) {

    std::cout << "Hello, RSA!" << std::endl;

    Number n = 1;
    Number m = 1;
    std::cout << n * m << std::endl;

    return 0;
}


