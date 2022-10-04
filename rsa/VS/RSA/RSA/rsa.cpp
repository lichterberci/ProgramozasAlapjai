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

    Number (const uint64_t n)
    : m_size(4)
    { 
        m_data = std::vector<uint8_t>(4);

        m_data[0] = ((n & 0x000000FF));
        m_data[1] = ((n & 0x0000FF00) >> 8);
        m_data[2] = ((n & 0x00FF0000) >> 16);
        m_data[3] = ((n & 0xFF000000) >> 24);        
    }

    Number(const int _size, const std::vector<uint8_t>& _data)
        : m_size(m_size) {
        m_data.resize(_size);
        m_data = _data;
    }

    const int GetSize() const { return m_size; }

    const std::string ToString() const {
        std::stringstream outputStream;
        for (int i = m_size - 1; i >= 0; i--)
        {
            outputStream << std::hex << (uint16_t)m_data[i];
        }        
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
        return m_data[i];
    }

    // copy
    Number& operator= (const Number& right) {
        m_data = std::vector<uint8_t>(right.m_data);
        m_size = right.m_size;
        return *this;
    }

    friend Number operator+ (Number& left, const Number& right) {
        
        int resultSize = std::max(right.m_size, left.m_size) + 1;
        std::vector<uint8_t> resultData;

        uint8_t carry = 0;
        for (int i = 0; i < resultSize; i++) {

            uint8_t a = i < left.m_size ? left.m_data[i] : 0;
            uint8_t b = i < right.m_size ? right.m_data[i] : 0;

            uint16_t sum = a + b + carry;

            carry = sum / 256;
            sum %= 256;

            resultData.push_back((uint8_t)sum);
        }        

        return Number (resultSize, resultData);
    }
};

std::ostream& operator<<(std::ostream& out, const Number& right) {
    return out << right.ToString();
}

int main(int argc, char** argv) {

    std::cout << "Hello, RSA!" << std::endl;

    Number n(256);
    Number m(256);
    std::cout << "n: " << n << std::endl;
    std::cout << "m: " << m << std::endl;
    std::cout << "n + m: " << n + m << std::endl;
    Number r = n + m;
    std::cout << r.GetSize() << std::endl;
    std::cout << r << std::endl;
    return 0;
}


