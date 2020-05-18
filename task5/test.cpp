#include <iostream>
#include <utility>
#include <vector>
#include <array>
#include <queue>
#include <memory>
#include <unordered_map>
#include <stack>
#include "Huffman.h"
#include <fstream>
typedef unsigned char byte;

struct MapNode {
    byte size;
    unsigned int code;

    MapNode() : size(0), code(0) {}
};

struct SymbolNode {
    byte symbol;
    size_t frequency;
    std::shared_ptr<SymbolNode> left;
    std::shared_ptr<SymbolNode> right;

    SymbolNode()
            : symbol('\0'), frequency(0),
              left(nullptr), right(nullptr) {}

    explicit SymbolNode(byte symbol)
            : symbol(symbol),
              frequency(0),
              left(nullptr), right(nullptr) {}

    SymbolNode(byte symbol, size_t frequency)
            : symbol(symbol),
              frequency(frequency),
              left(nullptr), right(nullptr) {}
};

class VectorInput : public IInputStream {
public:
    explicit VectorInput(std::vector<byte> *bytes) : bytes(bytes), pos(0) {}

    bool Read(byte &value) {
        if (pos >= bytes->size()) {
            return false;
        }
        value = (*bytes)[pos++];
        return true;
    }

    void reset() {
        pos = 0;
    }

private:
    std::vector<byte> *bytes;
    size_t pos;
};

class VectorOutput : public IOutputStream {
public:
    explicit VectorOutput(std::vector<byte> *bytes) : bytes(bytes) {}

    void Write(byte &value) {
        bytes->push_back(value);
    }

private:
    std::vector<byte> *bytes;
};

class BitInputStream {
public:
    explicit BitInputStream(IInputStream &input) : inputStream(input), bitPos(0), buff(0) {}

    bool readBit(byte &res) {
        if (bitPos == 0) {
            bool readRes = inputStream.Read(buff);
            if (!readRes) {
                return false;
            }
        }
        res = (buff >> (bitPos++)) & 1;
        if (bitPos == 8) {
            bitPos = 0;
        }
        return true;
    }

    bool readByte(byte &res) {
        res = 0;
        for (int i = 0; i < 8; i++) {
            byte bit = 0;
            bool isReadable = readBit(bit);
            if (!isReadable) return false;
            res = res | ((bit & 1) << i);
        }
        return true;
    }

private:
    IInputStream &inputStream;
    byte buff;
    byte bitPos;
};

class BitOutputStream {
public:
    explicit BitOutputStream(IOutputStream &output) : outputStream(output), bitPos(0), buff(0) {}

    void writeByte(byte b) {
        for (byte i = 0; i < 8; i++) {
            writeBit(((b >> i) & 1));
        }
    }

    void writeBits(unsigned int code, byte size) {
        for (byte i = 0; i < size; i++) {
            writeBit(((code >> i) & 1));
        }
    }

    void writeBit(byte bit) {
        if (bitPos == 8) {
            flush();
        }
        buff = buff | ((bit & 1) << (bitPos++));
    }

    void flush() {
        outputStream.Write(buff);
        buff = 0;
        bitPos = 0;
    }

private:
    IOutputStream &outputStream;
    byte buff;
    byte bitPos;

};


class CounterSymbols {
public:
    CounterSymbols() {
        symbols.fill(0);
    }

    void count(IInputStream &text) {
        byte symbol;
        while (text.Read(symbol)) {
            symbols[symbol]++;
        }
    }

    size_t operator[](byte symbol) {
        return symbols[symbol];
    }

    template<class Heap_t>
    void makeMinHeap(Heap_t &heap) {
        for (int i = 0; i < 256; i++) {
            if (symbols[i] > 0) {
                heap.push(std::make_shared<SymbolNode>(i, symbols[i]));
            }
        }
    }

    size_t frequencyAt(byte symbol) {
        return symbols[symbol];
    }

private:
    std::array<size_t, 256> symbols;
};

class HuffmanTree {
public:
    HuffmanTree() : root(nullptr), lastNode(nullptr) {}

    explicit HuffmanTree(IInputStream &text) : root(nullptr), lastNode(nullptr) {
        counter.count(text);


        auto comp = [](std::shared_ptr<SymbolNode> &l, std::shared_ptr<SymbolNode> &r) {
            return l->frequency > r->frequency;
        };
        std::priority_queue<std::shared_ptr<SymbolNode>,
                std::vector<std::shared_ptr<SymbolNode>>,
                decltype(comp)> minHeap(comp);

        counter.makeMinHeap(minHeap);

        while (minHeap.size() > 1) {
            auto first = minHeap.top();
            minHeap.pop();
            auto second = minHeap.top();
            minHeap.pop();
            auto combo = std::make_shared<SymbolNode>('\0', first->frequency + second->frequency);
            combo->left = first;
            combo->right = second;
            minHeap.push(combo);
        }

        root = minHeap.top();
        makeCodeMap();
    }


    void treeWrite(BitOutputStream &ss) {
        _treeWrite(root, ss);
    }

    void treeRead(BitInputStream &ss) {
        root = _treeRead(ss);
    }

    byte lastUsedBits() {
        size_t bitsOfMessage = 0;
        for (auto pair: codeMap) {
            bitsOfMessage += pair.second.size * counter.frequencyAt(pair.first);
        }
        size_t bitsOfTree = codeMap.size() * 10 - 1;
        return ((bitsOfMessage + bitsOfTree) + 4) % 8; // + 4 потому что флаг занимает 4 бита
    }

    std::pair<bool, char> getByBits(byte bit) {
        if (!lastNode) {
            if (!root) {
                return {false, '\0'};
            }
            if (!root->left && !root->right) {
                return {true, root->symbol};
            }
            lastNode = root;
        }
        if (bit == 0) {
            lastNode = lastNode->left;
        } else {
            lastNode = lastNode->right;
        }

        if (!lastNode->left && !lastNode->right) {
            byte symbol = lastNode->symbol;
            lastNode = nullptr;
            return {true, symbol};
        } else {
            return {false, '\0'};
        }
    }

    std::unordered_map<byte, MapNode> codeMap;
private:

    void makeCodeMap() {
        enum NodeType {
            rootWas, leftDone, rightDone
        };
        std::stack<std::shared_ptr<SymbolNode>> history;
        std::stack<byte> description;
        MapNode currMapNode;
        history.push(root);
        description.push(rootWas);
        while (!history.empty()) {
            switch (description.top()) {
                case rootWas:
                    if (history.top()->left) {
                        currMapNode.size++;
                        history.push(history.top()->left);
                        description.push(rootWas);
                    } else {
                        if (!history.top()->right) {
                            codeMap.insert({history.top()->symbol, currMapNode});
                            currMapNode.code = (currMapNode.code) ^ 1 << currMapNode.size--;
                        }
                        description.top() = leftDone;
                    }
                    break;
                case leftDone:
                    currMapNode.code = (currMapNode.code) | 1 << currMapNode.size++;
                    if (history.top()->right) {
                        history.push(history.top()->right);
                        description.push(rootWas);
                    } else {
                        description.top() = rightDone;
                    }
                    break;
                case rightDone:
                    currMapNode.code = (currMapNode.code) ^ 1 << currMapNode.size--;
                    history.pop();
                    description.pop();
                    if (description.empty()) {
                        break;
                    }
                    if (description.top() == rootWas) {
                        description.top() = leftDone;
                    } else {
                        description.top() = rightDone;
                    }
                    break;
            }
        }
    }

    void _treeWrite(std::shared_ptr<SymbolNode> &node, BitOutputStream &ss) {
        if (!node->left && !node->right) {
            ss.writeBit(0);
            ss.writeByte(node->symbol);
        } else {
            ss.writeBit(1);
            _treeWrite(node->left, ss);
            _treeWrite(node->right, ss);
        }
    }

    std::shared_ptr<SymbolNode> _treeRead(BitInputStream &ss) {
        auto node = std::make_shared<SymbolNode>();
        byte currentBit;
        ss.readBit(currentBit);
        if (currentBit == 0) {
            byte symbol;
            ss.readByte(symbol);
            node->symbol = symbol;
        } else {
            node->left = _treeRead(ss);
            node->right = _treeRead(ss);
        }
        return node;
    }

    std::shared_ptr<SymbolNode> root;
    std::shared_ptr<SymbolNode> lastNode;
    CounterSymbols counter;
};

void copyText(IInputStream &original, std::vector<byte> &text) {
    byte symbol;
    while (original.Read(symbol)) {
        text.push_back(symbol);
    }
}

enum EncodingType {
    encodedByOneSymbol = 8,
    notEncoded = 9
};

void Encode(IInputStream &original, IOutputStream &compressed) {
    std::vector<byte> text;

    copyText(original, text);
    VectorInput originalStream(&text); // копируем и создаем свой поток для того, чтобы была возможность
    //вернуться к началу потока
    BitOutputStream output(compressed);
    byte _byte; //вспомогательная переменная

    //если в тексте меньше 8 символов, то кодировать не стоит и записывам все как есть
    if (text.size() < 8) {
        _byte = notEncoded;
        output.writeBits(_byte, 4);
        output.writeBits(0, 4);
        while (originalStream.Read(_byte)) {
            output.writeByte(_byte);
        }
        output.flush();
        return;
    }

    HuffmanTree tree(originalStream); // создаем префиксное дерево и хеш таблицу для символов

    originalStream.reset(); // возвращаемся на начало потока

    //если сообщение состоит из одного символа
    //записываем 4 бита
    //4 байта
    if (tree.codeMap.size() == 1) {
        _byte = encodedByOneSymbol;
        output.writeBits(_byte, 4);
        for (auto pair : tree.codeMap) { //выполнится один раз так как в таблице только один элемент
            byte symbol;
            output.writeByte(pair.first);
        }
        std::cout << text.size() << std::endl;
        output.writeBits(text.size(), 16);
        output.flush();
        return;
    }

    output.writeBits(tree.lastUsedBits(), 4); //указываем количество полезных бит в последнем байте
    tree.treeWrite(output);

    //записываем закодированный текст
    while (originalStream.Read(_byte)) {
        auto node = tree.codeMap[_byte];
        output.writeBits(node.code, node.size);
    }
    output.flush(); //сбрасываем буфер потока
}

void Decode(IInputStream &compressed, IOutputStream &original) {
    BitInputStream input(compressed);
    byte lastUsedBits = 0;
    byte _bit;
    for (int i = 0; i < 4; i++) {
        input.readBit(_bit);
        lastUsedBits |= ((_bit & 1) << i);
    }
    //если данные не закодированы
    if (lastUsedBits == notEncoded) {
        for (int i = 0; i < 4; i++) {
            input.readBit(_bit);
        }
        byte symbol;
        while (input.readByte(symbol)) {
            original.Write(symbol);
        }
        return;
    }

    //если данные состоят из одного символа
    if (lastUsedBits == encodedByOneSymbol) {
        size_t size = 0;
        byte symbol;
        byte bit;
        input.readByte(symbol);
        for (int i = 0; i <16; i++) {
            input.readBit(bit);
            size |= ((bit & 1) << i);
        }
        for (size_t i = 0; i < size; i++) {
            original.Write(symbol);
        }
        return;
    }

    HuffmanTree tree;
    tree.treeRead(input);

    byte bit;
    BitOutputStream output(original);
    std::queue<byte> buffer;
    while (input.readBit(bit)) {
        if (buffer.size() < 16) {
            buffer.push(bit);
        } else {
            for (int i = 0; i < 8; i++) {
                byte buffBit = buffer.front();
                buffer.pop();
                auto res = tree.getByBits(buffBit);
                if (res.first) {
                    output.writeByte(res.second);
                }
            }
            buffer.push(bit);
        }
    }
    lastUsedBits = lastUsedBits ? lastUsedBits : 8;
    byte needReed = buffer.size() - (8 - lastUsedBits);
    for (int i = 0; i < needReed; i++) {
        bit = buffer.front();
        buffer.pop();
        auto res = tree.getByBits(bit);
        if (res.first) {
            output.writeByte(res.second);
        }
    }
    output.flush();
}

//
//
//bool check(std::vector<byte> vectorL, std::vector<byte> vectorR) {
//    bool result = true;
//    if (vectorL.size() != vectorR.size()) {
//        return false;
//    }
//    for (int i = 0; i < vectorL.size(); i++) {
//        if (vectorL[i] != vectorR[i]) {
//            return false;
//        }
//    }
//    return true;
//}
//
//int main() {
//    std::ifstream file;
//    std::vector<byte> vector;
//    VectorOutput ovector(&vector);
//    BitOutputStream bitfstream(ovector);
//    file.open("../task5/testfile");
//    std::ofstream outFile;
//    outFile.open("/home/mark_bykhovets/testout", std::ofstream::out | std::ofstream::binary);
//    char ch = '\0';
//    while (file.get(ch)) {
//        bitfstream.writeByte(ch);
//    }
//    bitfstream.flush();
//    VectorInput ivector(&vector);
//    std::vector<byte> output_vector;
//    VectorOutput otvector(&output_vector);
//
//    Encode(ivector, otvector);
//
//    for (auto & _byte: output_vector) {
//        outFile << _byte;
//    }
//
//    std::cout << "original: " << vector.size() << std::endl;
//    std::cout << "encoded: " << output_vector.size() << std::endl;
//    outFile.close();
//
//    auto k = (float) output_vector.size() / vector.size();
//    std::cout << k << std::endl;
//    VectorInput decodeInput(&output_vector);
//    std::vector<byte> decoded;
//    VectorOutput odecoded(&decoded);
//
//    Decode(decodeInput, odecoded);
//
//    byte symbol;
//    VectorInput result(&decoded);
//    while (result.Read(symbol)) {
//        std::cout << symbol;
//    }
//    std::cout << std::endl;
//    if (check(vector, decoded)) {
//        std::cout << "PASSED" << std::endl;
//        return 0;
//    } else {
//        std::cout << "FAIL" << std::endl;
//        return 1;
//    }
//    return 0;
//}
//
