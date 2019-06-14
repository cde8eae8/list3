//
// Created by nikita on 07.06.19.
//

#include "huffman.h"


namespace HuffmanTree {
    namespace {
        struct Node {
            Node() : c(0), frequence(0), left(nullptr), right(nullptr) {}
            Node(Node *l, Node *r) : c(0), frequence(l->frequence + r->frequence),
                                     left(l), right(r) {}
            Node(unsigned char ch, size_t freq) : c(ch), frequence(freq),
                                                  left(nullptr), right(nullptr) {}

            ~Node() {
                if (!isLeaf()) {
                    delete left;
                    delete right;
                }
            }

            Node* to0() {
                return left;
            }

            Node* to1() {
                return right;
            }

            Node* create0() {
                left = new Node;
                return left;
            }

            Node* create1() {
                right = new Node;
                return right;
            }

            // TODO: change it
            bool isLeaf() {
                return left == right && left;
            }

            void print(size_t level = 0) {
                if(level > 10)
                    return;
                if (!isLeaf() && left)
                    left->print(level + 1);
                else {
                    std::cout << "->";
                    for (size_t i = 0; i < level; ++i) {
                        std::cout << "\t";
                    }
                    std::cout << "l " << left << std::endl;
                }
                std::cout << "->";
                for (size_t i = 0; i < level; ++i) {
                   std::cout << "\t";
                }
                std::cout << frequence << "(" << c << ")" << std::endl;
                if (!isLeaf() && right)
                    right->print(level + 1);
                else {
                    std::cout << "->";
                    for (size_t i = 0; i < level; ++i) {
                        std::cout << "\t";
                    }
                    std::cout << "r " << right << std::endl;
                }
            }
            unsigned char c;
            size_t frequence;
            Node *left, *right;
            friend bool operator<(Node const& lhs, Node const& rhs) {
                return lhs.frequence < rhs.frequence;
            }
        };

        struct NodeComparator {
            bool operator()(Node const *lhs, Node const *rhs) {
                return lhs->frequence > rhs->frequence;
            }
        };

        const unsigned char UP = 0;
        const unsigned char DOWN = 1;

        void setCodes(std::vector<Code>& data, Node *node, unsigned char* currentCode,
                size_t bit_length, bitwriter& tree_writer, std::vector<unsigned char>& used);

        bitarray setCodes(std::vector<Code>& data, Node *node, std::vector<unsigned char>& used) {
            assert(node);
            bitwriter bw;
            if (!node->left && !node->right) {
                bits::set_bit(data[node->c].code[0], 7);
                data[node->c].code_bit_length = 1;
                bw.push_back(DOWN);
                bw.push_back(UP);
                used.push_back(node->c);
                return bw.reset();
            }
            const size_t MAX_CODE_LENGTH = 8;
            unsigned char code[MAX_CODE_LENGTH]{};
            setCodes(data, node, code, 0, bw, used);
            return bw.reset();
        }

        void setCodes(std::vector<Code>& data, Node *node, unsigned char* currentCode,
                size_t bit_length, bitwriter& tree_writer, std::vector<unsigned char>& used) {
            assert(node);
            assert((!node->right && !node->left) || (node->right && node->left && node->right != node->left));
            assert(currentCode);
            tree_writer.push_back(DOWN);

            if (node->right && node->left) {
                bits::clear_bit(currentCode[bit_length / 8], 7 - bit_length % 8);
                setCodes(data, node->to0(), currentCode, bit_length + 1, tree_writer, used);
                bits::set_bit(currentCode[bit_length / 8], 7 - bit_length % 8);
                setCodes(data, node->to1(), currentCode, bit_length + 1, tree_writer, used);
            } else {
                std::copy(currentCode, currentCode + bits::bit_size_to_byte(bit_length), data[node->c].code.begin());
                data[node->c].code_bit_length = bit_length;
                used.push_back(node->c);
            }
            tree_writer.push_back(UP);
        }
    }

    bitarray buildTree(std::vector<Code>& data, std::vector<unsigned char>& used) {
        std::priority_queue<Node*, std::vector<Node*>, NodeComparator> tree;
        for (const auto &item : data) {
            if (item.frequence != 0)
                tree.push(new Node(item.ch, item.frequence));
        }
        while (tree.size() != 1) {
            Node* first = tree.top();
            tree.pop();
            Node* second = tree.top();
            tree.pop();
            Node* parent = new Node(first, second);
            tree.push(parent);
        }
        auto tree_bits = setCodes(data, tree.top(), used);
        delete tree.top();
        return tree_bits;
    }

    void restoreTree(Node* root, Node* node, bitreader& br, size_t& pos, std::vector<unsigned char> const& used_chars);
    Node* restoreTree(bitarray tree, std::vector<unsigned char> const& used_chars);

    void restoreTree(Node* root, Node* node, bitreader& br, size_t& pos, std::vector<unsigned char> const& used_chars) {
        if (br.eob())
            throw std::runtime_error("tree is corrupted");
        if (br.get() == UP) {
            node->c = used_chars[pos++];
            node->left = root;
            node->right = root;
            return;
        }
        restoreTree(root, node->create0(), br, pos, used_chars);
        if (br.get() == DOWN) {
            restoreTree(root, node->create1(), br, pos, used_chars);
        } else {
            std::cout << "error: only one child" << std::endl;
            //throw std::runtime_error("tree is corrupted");
        }
        if (br.get() == UP) {
            return;
        } else {
            std::cout << "error: too many children" << std::endl;
            //throw std::runtime_error("tree is corrupted");
        }
    }

    Node* restoreTree(bitarray tree, std::vector<unsigned char> const& used_chars) {
        bitreader br(tree);
        if (br.eob())
            return nullptr;
        if (br.get() != DOWN)
            throw std::runtime_error("tree is corrupted");
        Node *root = new Node;
        size_t pos = 0;
        restoreTree(root, root, br, pos, used_chars);
        return root;
    }

}

std::pair<std::vector<Code>, HuffmanData> HuffmanCoder::getCodes(unsigned char const *data, size_t length) {
    std::vector<Code> codes(256, Code(0));
    for (size_t j = 0; j < 256; ++j) {
        codes[j].ch = j;
    }
    for (size_t i = 0; i < length; ++i) {
        ++codes[data[i]].frequence;
    }

    if (length != 0) {
        std::vector<unsigned char> used;
        auto tree = HuffmanTree::buildTree(codes, used);
        tree = tree;
        char* used_chars = static_cast<char*>(operator new(used.size()));
        std::copy(used.begin(), used.end(), used_chars);
        return {codes, HuffmanData(tree, used_chars, used.size())};
    } else {
        return {codes, HuffmanData({}, {}, 0)};
    }
}

std::vector<unsigned char> HuffmanCoder::decode(HuffmanData const &data, char *dest, size_t length) {
    std::vector<unsigned char> used_chars(data.used_chars, data.used_chars + data.n_used_chars);
    HuffmanTree::Node* root = HuffmanTree::restoreTree(data.tree, used_chars);
    HuffmanTree::Node* node = root;
    bitreader br(data.code);
    std::vector<unsigned char> result;
    while(!br.eob()) {
        if(br.get() == 1) {
            node = node->to1();
        } else {
            node = node->to0();
        }
        if (node->isLeaf()) {
            result.push_back(node->c);
            node = root;
        }
    }
    delete root;
    return result;
}
