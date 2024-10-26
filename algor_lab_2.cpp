#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <stdexcept>

class BinaryNumber {
public:
    std::vector<bool> bits; 
    int size;  
 
    BinaryNumber(int size_temp) : size(size_temp), bits(size_temp, false) {}

    // перевод числа из десятичной в двоичную
    BinaryNumber(int decimal, int size_temp) : size(size_temp), bits(size_temp, false) {
        bool negative_num = decimal < 0;
        decimal = std::abs(decimal);
        for (int i = 0; i < size_temp; ++i) {
            bits[i] = decimal % 2;
            decimal /= 2;
        }
        if (negative_num) {
            add_two();
        }
    }

    // дополнение до двух для отрицательных чисел
    void add_two() {
        bool reloc = true;
        for (int i = 0; i < size; ++i) {
            bits[i] = !bits[i];  // инверсия битов
        }
        for (int i = 0; i < size && reloc; ++i) {
            reloc = bits[i];
            bits[i] = !bits[i];  // добавление единицы
        }
    }
    // перевод из двоичной в десятичную
    int toDecimal() const {
        int result_dec = 0;
        bool negative_num = bits[size - 1];  // проверяем последний бит (если = 1, то число -)
        std::vector<bool> temp(bits);

        if (negative_num) {
            bool reloc = true;
            for (int i = 0; i < size; ++i) {
                temp[i] = !temp[i];
            }
            for (int i = 0; i < size && reloc; ++i) {
                reloc = temp[i];
                temp[i] = !temp[i];
            }
        }

        for (int i = 0; i < size; ++i) {
            if (temp[i]) result_dec += (1 << i);  // суммируем степени двойки для каждого бита 
        }

        return negative_num ? -result_dec : result_dec;
    }


  
    BinaryNumber add(const BinaryNumber& other) const {
        BinaryNumber result(0, size);
        bool reloc = false;
        for (int i = 0; i < size; ++i) {
            bool sum = bits[i] ^ other.bits[i] ^ reloc;
            reloc = (bits[i] && other.bits[i]) || (bits[i] && reloc) || (other.bits[i] && reloc); 
            result.bits[i] = sum;
        }
        if (reloc) {
            std::cout << "Overflow in add!" << std::endl;
        }
        return result;
    }

    BinaryNumber subtract(const BinaryNumber& other) const {
        BinaryNumber negative_number(other.size);
        std::copy(other.bits.begin(), other.bits.end(), negative_number.bits.begin());
        negative_number.add_two();  // делаем из положительного other отрицательный negative_number
        return this->add(negative_number);  // результат вычитания other из this 
    }

    BinaryNumber multiply(const BinaryNumber& other) const {
        BinaryNumber result(0, size);
        for (int i = 0; i < size; ++i) {
            if (other.bits[i]) {
                BinaryNumber temp(*this); //копируем
                std::rotate(temp.bits.begin(), temp.bits.end() - i, temp.bits.end()); // сдвигаем temp на i битов влево
                std::fill(temp.bits.begin(), temp.bits.begin() + i, false);
                result = result.add(temp);  // складываем полученные промежуточные результаты
            }
        }
        return result;
    }

    void print() const {
        for (int i = size - 1; i >= 0; --i) {
            std::cout << bits[i];
        }
        std::cout << " (Binary)  " << std::endl;
        std::cout << toDecimal() << " (Decimal)" << std::endl;
    }
};



template<typename T>
class ListNode {
public:
    T value;
    ListNode* next_ptr;

    ListNode(T val) : value(val), next_ptr(nullptr) {}
};

//односвязный список
template<typename T>
class LinkedList {
public:
    ListNode<T>* head;

    LinkedList() : head(nullptr) {}


    void push_front(T value) {
        ListNode<T>* new_node = new ListNode<T>(value); 
        new_node->next_ptr = head; 
        head = new_node;
    }

 
    T pop_front() {
        if (!head) throw std::underflow_error("List is empty");
        ListNode<T>* temp = head; 
        T value_ret = head->value;
        head = head->next_ptr;
        delete temp;
        return value_ret;
    }

    bool empty() const { 
        return head == nullptr;
    }
};

//стек на основе односвязного списка
template<typename T>
class Stack {
public:
    LinkedList<T> list; 

    void push(T value) {
        list.push_front(value);
    }

    T pop() {
        return list.pop_front();
    }

    bool empty() const {
        return list.empty();
    }
};

class BinaryCalculator {
public:
    void run() {
        std::string expression;
        int size = 0;

        std::cout << "Enter the size (8, 16, 32): ";
        std::cin >> size;
        std::cin.ignore(); // очищаем буфер ввода
        std::cout << "Enter postfix expression: ";
        std::getline(std::cin, expression);

        try {
            BinaryNumber result = evaluate_postfix(expression, size);
            std::cout << "Result: " << std::endl;
            result.print();
        }
        catch (const std::exception& e) { 
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }

private:
    BinaryNumber evaluate_postfix(const std::string& expression, int size) {
        Stack<BinaryNumber> stack;
        std::istringstream stream(expression);
        std::string token;
        
        while (stream >> token) { 
            if (token == "+" || token == "-" || token == "*") {
                if (stack.empty()) throw std::invalid_argument("Invalid expression");

                BinaryNumber b = stack.pop();
                if (stack.empty()) throw std::invalid_argument("Invalid expression");

                BinaryNumber a = stack.pop();

                if (token == "+") {
                    stack.push(a.add(b));
                }
                else if (token == "-") {
                    stack.push(a.subtract(b));
                }
                else if (token == "*") {
                    stack.push(a.multiply(b));
                }
            }
            else {
                int num = std::stoi(token);
                stack.push(BinaryNumber(num, size)); // Преобразуем число в двоичное и помещаем в стек
            }
        }


        BinaryNumber result = stack.pop();

        if (!stack.empty()) throw std::invalid_argument("Invalid expression");

        return result;
    }
};

int main() {
    BinaryCalculator calc;
    calc.run();
    return 0;
}