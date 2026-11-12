#include <iostream>
#include <cctype>



using namespace std;

struct CharNode {
    char ch;
    CharNode* next;
    CharNode(char c, CharNode* n = nullptr) : ch(c), next(n) {}
};

class CharList {
    CharNode* head;

public:
    CharList() : head(nullptr) {}


    CharList(const CharList& other) : head(nullptr) {
        head = copyRecursive(other.head);
    }

    CharNode* copyRecursive(CharNode* node) {
        if (!node) return nullptr;
        return new CharNode(node->ch, copyRecursive(node->next));
    }


    ~CharList() {
        clear();
    }

    void pushFront(char c) {
        head = new CharNode(c, head);
    }

    void pushBack(char c) {
        head = pushBackRecursive(head, c);
    }

    CharNode* pushBackRecursive(CharNode* node, char c) {
        if (!node) return new CharNode(c);
        node->next = pushBackRecursive(node->next, c);
        return node;
    }


    bool equalsRecursive(CharNode* a, CharNode* b) const {
        if (!a && !b) return true;
        if (!a || !b) return false;
        if (a->ch != b->ch) return false;
        return equalsRecursive(a->next, b->next);
    }

    bool equals(const CharList& other) const {
        return equalsRecursive(head, other.head);
    }

    char popFront() {
        if (!head) return 0;
        char val = head->ch;
        CharNode* temp = head;
        head = head->next;
        delete temp;
        return val;
    }

    void removeBack() {
        head = removeBackRecursive(head);
    }

    CharNode* removeBackRecursive(CharNode* node) {
        if (!node) return nullptr;
        if (!node->next) {
            delete node;
            return nullptr;
        }
        if (!node->next->next) {
            delete node->next;
            node->next = nullptr;
            return node;
        }
        node->next = removeBackRecursive(node->next);
        return node;
    }



    void appendRecursive(CharNode* node) {
        if (!node) return;
        pushBack(node->ch);
        appendRecursive(node->next);
    }

    void append(const CharList& other) {
        appendRecursive(other.head);
    }

    void clear() {
        clearRecursive(head);
        head = nullptr;
    }

    void clearRecursive(CharNode* node) {
        if (!node) return;
        clearRecursive(node->next);
        delete node;
    }

    bool isEmpty() const {
        return head == nullptr;
    }

    bool isZeroOnly() const {
        return head && !head->next && head->ch == '0';
    }

    bool isNumberRecursive(CharNode* node, bool seenMinus = false) const {
        if (!node) return true;
        if (node->ch == '-') {
            return !seenMinus && !node->next && isNumberRecursive(node->next, true);
        }
        else if (!isdigit(node->ch)) {
            return false;
        }
        return isNumberRecursive(node->next, seenMinus);
    }

    bool isNumber() const {
        if (!head) return false;
        return isNumberRecursive(head);
    }


    int toIntRecursive(CharNode* node) const {
        if (!node) return 0;
        return (node->ch - '0') + 10 * toIntRecursive(node->next);
    }

    int toInt() const {
        if (!isNumber()) return 0;

        CharNode* cur = head;
        bool negative = false;

        if (cur && cur->ch == '-') {
            negative = true;
            cur = cur->next;
        }

        int val = toIntRecursive(cur);
        return negative ? -val : val;
    }



    void fromIntRecursive(int value) {
        if (value == 0) return;
        fromIntRecursive(value / 10);
        char digit = (value % 10) + '0';
        pushFront(digit);
    }

    void fromInt(int value) {
        clear();
        if (value == 0) {
            pushFront('0');
            return;
        }

        bool negative = value < 0;
        value = abs(value);

        fromIntRecursive(value);

        if (negative) {
            pushFront('-');
        }
    }



    void printRecursive(CharNode* node) const {
        if (!node) {
            cout << '\n';
            return;
        }
        cout << node->ch;
        printRecursive(node->next);
    }

    void print() const {
        printRecursive(head);
    }


    CharNode* cloneNodes(CharNode* node) const {
        if (!node) return nullptr;
        return new CharNode(node->ch, cloneNodes(node->next));
    }

    CharList* clone() const {
        CharList* result = new CharList();
        result->head = cloneNodes(head);
        return result;
    }



    char frontChar() const {
        return head ? head->ch : 0;
    }

    char backCharRecursive(CharNode* node) const {
        if (!node) return 0;
        if (!node->next) return node->ch;
        return backCharRecursive(node->next);
    }

    char backChar() const {
        return backCharRecursive(head);
    }
};
struct StackNode {
    CharList* list;
    StackNode* next;

    StackNode(CharList* l, StackNode* n = nullptr) : list(l), next(n) {}
};

class Stack {
    StackNode* topNode;

public:
    Stack() : topNode(nullptr) {}

    ~Stack() {
        clear();
    }

    void push(CharList* list) {
        topNode = new StackNode(list, topNode);
    }

    CharList* pop() {
        if (!topNode) return nullptr;
        StackNode* temp = topNode;
        CharList* list = temp->list;
        topNode = topNode->next;
        delete temp;
        return list;
    }

    CharList* peekRecursive(StackNode* node, int index) const {
        if (!node) return nullptr;
        if (index == 0) return node->list;
        return peekRecursive(node->next, index - 1);
    }

    CharList* peek(int index = 0) const {
        return peekRecursive(topNode, index);
    }


    void swapTopTwo() {
        if (!topNode || !topNode->next) return;
        StackNode* first = topNode;
        StackNode* second = topNode->next;

        // Perform node swap
        first->next = second->next;
        second->next = first;
        topNode = second;
    }


    bool isEmpty() const {
        return topNode == nullptr;
    }

    void clear() {
        clearRecursive(topNode);
        topNode = nullptr;
    }

    void clearRecursive(StackNode* node) {
        if (!node) return;
        clearRecursive(node->next);
        delete node->list;
        delete node;
    }


    void print() const {
        printRecursive(topNode, 0);
    }

    int printRecursive(StackNode* node, int depth) const {
        if (!node) return depth;
        int currentDepth = printRecursive(node->next, depth + 1);
        cout << (currentDepth - 1) << ": ";
        node->list->print();
        return currentDepth - 1;
    }



    CharList* cloneAtRecursive(StackNode* node, int index) const {
        if (!node) return new CharList();
        if (index == 0) return node->list->clone();
        return cloneAtRecursive(node->next, index - 1);
    }

    CharList* cloneAt(int index) const {
        return cloneAtRecursive(topNode, index);
    }



};

const int MAX = 20000;

char program[MAX];
char inputBuffer[MAX];
int programLength = 0;
int inputLength = 0;
int inputPointer = 0;

Stack stack;


void run(int ip) {

    if (ip >= programLength) return;

    char instr = program[ip];
    instr = program[ip];
    if (instr == '\0') return;

    // HANDLE ALL INSTRUCTIONS FIRST

    else if (instr == '\'') {
        stack.push(new CharList());  // Always push a new empty list
    }
    else if (instr == ':') {
        CharList* top = stack.peek();
        if (top) {
            stack.push(top->clone());
        }
    }

    else if (instr == ';') {
        stack.swapTopTwo();
    }
    else if (instr == '@') {
        CharList* indexList = stack.pop();
        int index = indexList ? indexList->toInt() : 0;
        delete indexList;

        CharList* cloned = stack.cloneAt(index);
        stack.push(cloned ? cloned : new CharList());
    }




    else if (instr == '&') {
        stack.print();
    }
    else if (instr == '-') {
        CharList* top = stack.pop();
        if (top) {
            if (top->frontChar() == '-') {
                top->popFront();
            }
            else {
                CharList* temp = new CharList();
                temp->pushFront('-');
                temp->append(*top);
                delete top;
                top = temp;
            }
            stack.push(top);
        }
    }
    

    else if (instr == '+') {
        CharList* aList = stack.pop();
        CharList* bList = stack.pop();
        if (aList && bList) {
            int a = aList->toInt();
            int b = bList->toInt();
            CharList* result = new CharList();
            result->fromInt(a + b);
            stack.push(result);
        }
        delete aList;
        delete bList;
    }
    else if (instr == '.') {
        if (inputPointer < inputLength) {
            char ch = inputBuffer[inputPointer++];
            CharList* top = stack.peek();
            if (top) top->pushFront(ch);
        }
    }
    else if (instr == '>') {
        CharList* top = stack.pop();
        if (top && !top->isEmpty()) {
            cout << top->frontChar(); // Output the first character
        }
        delete top;
    }



    else if (instr == '^') {
        CharList* top = stack.peek();
        if (top && top->backChar() == '-') {
            top->removeBack();
        }
    }
    else if (instr == '[') {
        CharList* list = stack.pop();
        if (list && !list->isEmpty()) {
            int ascii = list->frontChar();
            CharList* result = new CharList();
            result->fromInt(ascii);
            stack.push(result);
        }
        else {
            stack.push(new CharList()); // push empty if list was null
        }
        delete list;
    }

    else if (instr == ']') {
        CharList* numList = stack.pop();
        int val = numList ? numList->toInt() : 0;
        delete numList;

        CharList* result = new CharList();
        result->pushFront((char)val);
        stack.push(result);
    }

    else if (instr == '$') {
        CharList* original = stack.pop();
        if (original && !original->isEmpty()) {
            char first = original->popFront();
            CharList* newList = new CharList();
            newList->pushFront(first);
            stack.push(original);
            stack.push(newList);
        }
        else if (original) {
            stack.push(original);
        }
    }






    else if (instr == '#') {
        CharList* a = stack.pop();
        CharList* top = stack.peek();
        if (a && top) {
            top->append(*a);
        }
        delete a;
    }
    else if (instr == '<') {
        CharList* aList = stack.pop();
        CharList* bList = stack.pop();
        int a = aList ? aList->toInt() : 0;
        int b = bList ? bList->toInt() : 0;
        delete aList;
        delete bList;

        CharList* result = new CharList();
        result->fromInt(b < a ? 1 : 0);
        stack.push(result);
    }

    else if (instr == '=') {
        CharList* aList = stack.pop();
        CharList* bList = stack.pop();
        int a = aList ? aList->toInt() : 0;
        int b = bList ? bList->toInt() : 0;
        delete aList;
        delete bList;

        CharList* result = new CharList();
        result->fromInt(b == a ? 1 : 0);
        stack.push(result);
    }

    else if (instr == '!') {
        CharList* list = stack.pop();
        CharList* result = new CharList();

        if (!list || list->isEmpty() || list->isZeroOnly()) {
            result->pushFront('1');
        }
        else {
            result->pushFront('0');
        }

        delete list;
        stack.push(result);
    }

    else if (instr == '~') {
        CharList* result = new CharList();
        result->fromInt(ip);
        stack.push(result);
    }

    else if (instr == '?') {
        CharList* target = stack.pop();  // instruction index
        CharList* condition = stack.pop();  // the list to evaluate
        int jumpTo = target ? target->toInt() : 0;

        bool shouldJump = condition && !condition->isEmpty() && !condition->isZeroOnly();
        delete target;
        delete condition;

        if (shouldJump) {
            run(jumpTo);
            return;  // Do not continue to ip + 1
        }
    }

    else if (instr == ',') {
        CharList* top = stack.pop();
        delete top;
    }

    else {
        CharList* top = stack.peek();
        if (top) top->pushFront(instr);
    }

    run(ip + 1);
}
int getLength(const char* str, int index = 0) {
    if (str[index] == '\0') return index;
    return getLength(str, index + 1);
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    cin.getline(program, MAX);
    cin.getline(inputBuffer, MAX);

    programLength = getLength(program);
    inputLength = getLength(inputBuffer);

    run(0);
    return 0;
}

