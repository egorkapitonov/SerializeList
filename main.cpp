#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdint>
#include <unordered_map>

using namespace std;

struct ListNode { 
    ListNode* prev = nullptr; 
    ListNode* next = nullptr;
    ListNode* rand = nullptr; 
    std::string data; 
};


ListNode* LoadList(const string& filename) {

    vector<string> all_data;                        // хранит все data
    vector<int> all_rand_index;                     // хранит все rand
    vector<ListNode*> nodes;                        // хранит все узлы

    ifstream file(filename);                        // читает файл
    string line;

    if (!file) {                                    // проверка на открытие файла
        cout << "File not open"s << endl;
        return nullptr;
    }

    // Чтение файла по строчно
    while (getline(file, line)) {
        size_t pos = line.find(';');                // определяю позицию у знака ;
        string str = line.substr(0, pos);           // data до знака ;
        string str_index = line.substr(pos + 1);    // индекс rand

        int i = stoi(str_index);                    // преобразем в int

        all_data.push_back(str);                    // сохраняем каждую data в вектор
        all_rand_index.push_back(i);                // сохраняем каждый rand_index в вектор
    }

    // Создание узлов списка
    for (string str : all_data) {                   // проходим по нашим data
        ListNode* node = new ListNode;              // создаю узел в куче
        node->data = str;                           // присваиваю узлу data
        nodes.push_back(node);                      // сохраняю указатель
    }

    // Присваивание prev и next узлам
    for (size_t i = 0; i < nodes.size(); i++) {

        // Через тернарный оператор проверяю является ли элемент первый и последний
        nodes[i]->prev = (i == 0 ? nullptr : nodes[i - 1]);
        nodes[i]->next = (i == nodes.size() - 1 ? nullptr : nodes[i + 1]);
    }

    // Присваивание rand узлам
    // Проверка rand на отрицательное значение
    for (size_t i = 0; i < all_rand_index.size(); i++) {
        if (all_rand_index[i] >= 0) {
            nodes[i]->rand = nodes[all_rand_index[i]];
        }
        else {
            nodes[i]->rand = nullptr;
        }
    }

    // Если список пустой
    if (nodes.empty()) {
        return nullptr;
    }
    // Возвращение голову списка
    return nodes[0];
}
// Сериализация списка в бинарный файл
void SerializeList(ListNode* head, const string& filename) {

    // Если список пустой, ничего нет для записи
    if (!head) {
        // Создаём бинарный файл
        std::ofstream out(filename, std::ios::binary);
        if (out) {
            uint32_t zero = 0;                       // Количество узлов = 0
            out.write(reinterpret_cast<const char*>(&zero), sizeof(zero)); // Записываем 4 байта
        }
        return; // Пустой список обработан
    }
    // Создаем вектор узлов, что бы находить индексы
    vector<ListNode*> nodes;

    // Создаём карту указатель индекс для быстрого поиска rand
    unordered_map<ListNode*, int32_t> ptr_to_idx;

    ListNode* current = head;                       // голова узла
    int32_t idx = 0;                                // Индекс первого узла

    while (current) {
        nodes.push_back(current);                   // добавляю указатель на узел в вектор
        ptr_to_idx[current] = idx++;                // Сохраняем индекс узла по указателю
        current = current->next;                    // следующий узел
    }

    // Открываем файл для записил
    ofstream out (filename, ios::binary); 
    if (!out) {
        cerr << "Cannot open file " << filename << " for writing\n";
        return;
    }
    
    // Записываем количество узлов
    uint32_t count = static_cast<uint32_t>(nodes.size());
    out.write(reinterpret_cast<const char*>(&count), sizeof(count));

    // Проходим по всем узлам и записываем данные
    for (ListNode* node : nodes) {

        // Запишем длинну строки data
        uint32_t len = static_cast<uint32_t>(node->data.size());
        out.write(reinterpret_cast<const char*>(&len), sizeof(len));

        // Запишем символы строки
        if (len > 0) {
            out.write(node->data.data(), len);
        }

        // Определяем индекс узла, на который указывает rand
        int32_t rand_index = -1;                            // По умолчанию -1, если rand == nullptr
        if (node-> rand) {
            auto it = ptr_to_idx.find(node->rand);          // Ищем узел в карте
            if (it != ptr_to_idx.end()) {
                rand_index = it->second;                    // Индекс найденного узла
            }
        }
        // Записываем rand_index
        out.write(reinterpret_cast<const char*>(&rand_index), sizeof(rand_index));
    }
}
// Освобождаем память
void DeleteList(ListNode* head) {
    
    ListNode* current = head;
    while (current != nullptr) {
        ListNode* next = current->next;
        delete current;
        current = next;
    }
}
int main()
{
    ListNode* head = LoadList("inlet.in");

    // Проверка
    if (!head) {
        cout << "List is empty or file error" << endl;
        return 1;
    }
    SerializeList(head, "outlet.out");
    DeleteList(head);
}