#pragma once

#include <cassert>
#include <cstddef>
#include <iterator>
#include <string>
#include <utility>
#include <algorithm>
#include <new>

template <typename Type>
class SingleLinkedList {
    // Узел списка
    struct Node {
        Node() = default;
        Node(const Type& val, Node* next) : value(val), next_node(next) {}
        Type value = {};
        Node* next_node = nullptr;
    };

    // Шаблон класса Базовый Итератор.
    // Определяет поведение итератора на элементы односвязного списка
    // ValueType - совпадает с Type (для Iterator) либо с const Type (для ConstIterator)
    template <typename ValueType>
    class BasicIterator {
        // Класс списка объявляется дружественным, чтобы из методов списка
        // был доступ к приватной области итератора
        friend class SingleLinkedList;

        // Конвертирующий конструктор итератора из указателя на узел списка
        explicit BasicIterator(SingleLinkedList<Type>::Node* node) : node_(node) {}
        explicit BasicIterator(const SingleLinkedList<Type>::Node* node) : node_(const_cast<SingleLinkedList<Type>::Node*>(node)) {}

    public:
        // Объявленные ниже типы сообщают стандартной библиотеке о свойствах этого итератора

        // Категория итератора - forward iterator
        // (итератор, который поддерживает операции инкремента и многократное разыменование)
        using iterator_category = std::forward_iterator_tag;
        // Тип элементов, по которым перемещается итератор
        using value_type = Type;
        // Тип, используемый для хранения смещения между итераторами
        using difference_type = std::ptrdiff_t;
        // Тип указателя на итерируемое значение
        using pointer = ValueType*;
        // Тип ссылки на итерируемое значение
        using reference = ValueType&;
        // Тип указателя на итерируемое значение
        using const_pointer = const ValueType*;
        // Тип ссылки на итерируемое значение
        using const_reference = const ValueType&;


        BasicIterator() = default;

        // Конвертирующий конструктор/конструктор копирования
        // При ValueType, совпадающем с Type, играет роль копирующего конструктора
        // При ValueType, совпадающем с const Type, играет роль конвертирующего конструктора
        BasicIterator(const BasicIterator<Type>& other) noexcept : node_(other.node_) {}

        // Чтобы компилятор не выдавал предупреждение об отсутствии оператора = при наличии
        // пользовательского конструктора копирования, явно объявим оператор = и
        // попросим компилятор сгенерировать его за нас.
        BasicIterator& operator=(const BasicIterator& rhs) = default;

        // Оператор сравнения итераторов (в роли второго аргумента выступает константный итератор)
        // Два итератора равны, если они ссылаются на один и тот же элемент списка, либо на end()
        [[nodiscard]]
        bool operator==(const BasicIterator<const Type>& rhs) const noexcept {
            return node_ == rhs.node_;
        }

        // Оператор, проверки итераторов на неравенство
        // Противоположен !=
        [[nodiscard]]
        bool operator!=(const BasicIterator<const Type>& rhs) const noexcept {
            return !(*this == rhs);
        }

        // Оператор сравнения итераторов (в роли второго аргумента итератор)
        // Два итератора равны, если они ссылаются на один и тот же элемент списка, либо на end()
        [[nodiscard]]
        bool operator==(const BasicIterator<Type>& rhs) const noexcept {
            return node_ == rhs.node_;
        }

        // Оператор, проверки итераторов на неравенство
        // Противоположен !=
        [[nodiscard]]
        bool operator!=(const BasicIterator<Type>& rhs) const noexcept {
            return !(*this == rhs);
        }

        // Оператор прединкремента. После его вызова итератор указывает на следующий элемент списка
        // Возвращает ссылку на самого себя
        // Инкремент итератора, не указывающего на существующий элемент списка, приводит к неопределённому поведению
        BasicIterator& operator++() noexcept {
            if (node_ != nullptr) {
                node_ = node_->next_node;
            }
            return *this;
        }

        // Оператор постинкремента. После его вызова итератор указывает на следующий элемент списка.
        // Возвращает прежнее значение итератора
        // Инкремент итератора, не указывающего на существующий элемент списка,
        // приводит к неопределённому поведению
        BasicIterator operator++(int) noexcept {
            BasicIterator temp(*this);
            if (node_ != nullptr) {
                node_ = node_->next_node;
            }
            return temp;
        }

        // Операция разыменования. Возвращает ссылку на текущий элемент
        // Вызов этого оператора, у итератора, не указывающего на существующий элемент списка,
        // приводит к неопределённому поведению
        [[nodiscard]]
        reference operator*() const noexcept {
            return node_->value;
        }

        // Операция доступа к члену класса. Возвращает указатель на текущий элемент списка.
        // Вызов этого оператора, у итератора, не указывающего на существующий элемент списка,
        // приводит к неопределённому поведению
        [[nodiscard]]
        pointer operator->() const noexcept {
            return &node_->value;
        }

    private:
        Node* node_ = nullptr;
    };

public:
    using value_type = Type;
    using reference = value_type&;
    using const_reference = const value_type&;

    // Итератор, допускающий изменение элементов списка
    using Iterator = BasicIterator<Type>;
    // Константный итератор, предоставляющий доступ для чтения к элементам списка
    using ConstIterator = BasicIterator<const Type>;

    // Возвращает итератор, ссылающийся на первый элемент
    // Если список пустой, возвращённый итератор будет равен end()
    [[nodiscard]]
    Iterator begin() noexcept {
        return Iterator(head_->next_node);
    }

    // Возвращает итератор, указывающий на позицию, следующую за последним элементом односвязного списка
    // Разыменовывать этот итератор нельзя - попытка разыменования приведёт к неопределённому поведению
    [[nodiscard]]
    Iterator end() noexcept {
        return Iterator(tail_);
    }

    // Возвращает константный итератор, ссылающийся на первый элемент
    // Если список пустой, возвращённый итератор будет равен end()
    // Результат вызова эквивалентен вызову метода cbegin()
    [[nodiscard]]
    ConstIterator begin() const noexcept {
        return ConstIterator(head_->next_node);
    }

    // Возвращает константный итератор, указывающий на позицию, следующую за последним элементом односвязного списка
    // Разыменовывать этот итератор нельзя - попытка разыменования приведёт к неопределённому поведению
    // Результат вызова эквивалентен вызову метода cend()
    [[nodiscard]]
    ConstIterator end() const noexcept {
        return ConstIterator(tail_);
    }

    // Возвращает константный итератор, ссылающийся на первый элемент
    // Если список пустой, возвращённый итератор будет равен cend()
    [[nodiscard]]
    ConstIterator cbegin() const noexcept {
        return ConstIterator(head_->next_node);
    }

    // Возвращает константный итератор, указывающий на позицию, следующую за последним элементом односвязного списка
    // Разыменовывать этот итератор нельзя - попытка разыменования приведёт к неопределённому поведению
    [[nodiscard]]
    ConstIterator cend() const noexcept {
        return ConstIterator(tail_);
    }

    SingleLinkedList() : tail_(new Node({}, nullptr)), head_(new Node({}, tail_)), size_(0) {}

    SingleLinkedList(std::initializer_list<Type> values) : tail_(new Node({}, nullptr)), head_(new Node({}, tail_)), size_(0) {
        Node* newHead = new Node({}, nullptr);
        size_t count = 0;
        Node* newEnd = Copy(values.begin(), values.end(), newHead, count);
        newEnd->next_node = tail_;
        head_ = newHead;
        size_ = count;
    }

    SingleLinkedList(const SingleLinkedList& other) : tail_(new Node({}, nullptr)), head_(new Node({}, tail_)), size_(0) {
        Node* newHead = new Node({}, nullptr);
        size_t count = 0;
        Node* newEnd = Copy(other.begin(), other.end(), newHead, count);
        newEnd->next_node = tail_;
        head_ = newHead;
        size_ = count;
    }

    SingleLinkedList& operator=(const SingleLinkedList& rhs) {
        Node* newHead = new Node({}, nullptr);
        size_t count = 0;
        Node* newEnd = Copy(rhs.begin(), rhs.end(), newHead, count);
        Clear();
        newEnd->next_node = tail_;
        head_ = newHead;
        size_ = rhs.size_;
        return *this;
    }

    // Обменивает содержимое списков за время O(1)
    void swap(SingleLinkedList& other) noexcept {
        Node* tempTail = tail_;
        Node* tempHead = head_;
        size_t tempSize = size_;
        tail_ = other.tail_;
        head_ = other.head_;
        size_ = other.size_;
        other.tail_ = tempTail;
        other.head_ = tempHead;
        other.size_ = tempSize;
    }

    ~SingleLinkedList() {
        Clear();
        delete tail_;
        delete head_;
    }

    // Возвращает количество элементов в списке за время O(1)
    [[nodiscard]]
    size_t GetSize() const noexcept {
        // Заглушка. Реализуйте метод самостоятельно
        return size_;
    }

    // Сообщает, пустой ли список за время O(1)
    [[nodiscard]]
    bool IsEmpty() const noexcept {
        return size_ == 0;
    }

    // Вставляет элемент value в начало списка за время O(1)
    void PushFront(const Type& value) {
        head_->next_node = new Node(value, head_->next_node);
        ++size_;
    }

    // Вставляет элемент value в начало списка за время O(1)
    void PushBack(const Type& value) {
        tail_->next_node = new Node({}, nullptr);
        tail_->value = value;
        tail_ = tail_->next_node;
        ++size_;
    }

    // Очищает список за время O(N)
    void Clear() noexcept {
        Node* next = head_->next_node;
        while (next != tail_) {
            head_->next_node = next->next_node;
            delete next;
            next = head_->next_node;
        }
        size_ = 0;
    }

    // Возвращает итератор, указывающий на позицию перед первым элементом односвязного списка.
    // Разыменовывать этот итератор нельзя - попытка разыменования приведёт к неопределённому поведению
    [[nodiscard]]
    Iterator before_begin() noexcept {
        return Iterator(head_);
    }

    // Возвращает константный итератор, указывающий на позицию перед первым элементом односвязного списка.
    // Разыменовывать этот итератор нельзя - попытка разыменования приведёт к неопределённому поведению
    [[nodiscard]]
    ConstIterator cbefore_begin() const noexcept {
        return ConstIterator(head_);
    }

    // Возвращает константный итератор, указывающий на позицию перед первым элементом односвязного списка.
    // Разыменовывать этот итератор нельзя - попытка разыменования приведёт к неопределённому поведению
    [[nodiscard]]
    ConstIterator before_begin() const noexcept {
        return ConstIterator(head_);
    }

    // Вставляет элемент value после элемента, на который указывает pos.
    // Возвращает итератор на вставленный элемент
    // Если при создании элемента будет выброшено исключение, список останется в прежнем состоянии
    Iterator InsertAfter(ConstIterator pos, const Type& value) {
        Node* newNode = new Node(value, pos.node_->next_node);
        pos.node_->next_node = newNode;
        ++size_;
        return Iterator(newNode);
    }

    void PopFront() noexcept {
        if (size_ == 0 || head_->next_node == tail_) {
            return;
        }
        Node* temp = head_->next_node->next_node;
        delete head_->next_node;
        head_->next_node = temp;
        --size_;
    }

    // Удаляет элемент, следующий за pos.
    // Возвращает итератор на элемент, следующий за удалённым
    Iterator EraseAfter(ConstIterator pos) noexcept {
        Node* temp = pos.node_->next_node->next_node;
        delete pos.node_->next_node;
        pos.node_->next_node = temp;
        --size_;
        return Iterator(temp);
    }

private:

    template <typename ContainerIterator>
    Node* Copy(ContainerIterator from_begin, ContainerIterator from_end, Node* to_head, size_t& count) {
        auto it = from_begin;
        Node* current = to_head;
        try {
            while (it != from_end) {
                current->next_node = new Node(*it, nullptr);
                ++it;
                ++count;
                current = current->next_node;
            }
        }
        catch(const std::bad_alloc&) {
            Node* slider = to_head;
            while (slider != current) {
                Node* temp = slider;
                slider = slider->next_node;
                delete temp;
                --count;
            }
            throw;
        }
        return current;
    }

    // Фиктивный узел, используется для вставки "перед первым элементом"
    Node* tail_;
    Node* head_;
    size_t size_;
};

template <typename Type>
void swap(SingleLinkedList<Type>& lhs, SingleLinkedList<Type>& rhs) noexcept {
    lhs.swap(rhs);
}

template <typename Type>
bool operator==(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    if (lhs.GetSize() != rhs.GetSize()) {
        return false;
    }
    for (auto it1 = lhs.begin(), it2 = rhs.begin(); it1 != lhs.end();) {
        if (*it1 != *it2) {
            return false;
        }
        ++it1;
        ++it2;
    }
    return true;
}

template <typename Type>
bool operator!=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return !(lhs == rhs);
}

template <typename Type>
bool operator<(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end(), [](auto const& p1, auto const& p2) { return p1 < p2; });
}

template <typename Type>
bool operator<=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return !(lhs > rhs);
}

template <typename Type>
bool operator>(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end(), [](auto const& p1, auto const& p2) { return p1 > p2; });
}

template <typename Type>
bool operator>=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return !(lhs < rhs);
}