#pragma once

#include <iostream>

template <typename T>
class RBTree {
	const bool red = true;
	const bool black = false;

    struct Node {
    	T value; // значение элемента
    	bool color; // бит цвета

    	Node *left; // указатель на левое поддерево
    	Node *right; // указатель на правое поддерево

    	Node *parent; // родитель
    };

    Node *root; // корень дерева

    Node *Sibling(Node *node) const; // получение брата узла

    void RotateLeft(Node *node); // левый поворот
    void RotateRight(Node *node); // правый поворот

    void FixInsertion(Node *node); // корректировка узла для удовлетворения всех свойств красно-чёрного дерева после вставки
    void FixRemoving(Node *node); // корректировка узла для удовлетворения всех свойств красно-чёрного дерева после удаления

    Node *FindNode(const T& value) const; // поиск узла с заданным значением
    void RemoveNode(Node *node); // удаление узла node

    void Copy(Node *node, Node* &newNode); // копирование дерева
    void Clear(Node* &node); // очистка дерева (освобождение памяти)

    int GetSize(Node *node) const; // количество элементов в дереве
    int GetHeight(Node *node) const; // высота дерева

    void Print(std::ostream &os, Node *node, int level) const; // вывод дерева в виде повёрнутого дерева в поток

public:
    RBTree(); // конструктор по умолчанию
    RBTree(const RBTree& tree); // конструктор копирования

    RBTree& operator=(const RBTree& tree); // оператор присваивания

    void Insert(const T& value); // вставка элемента в дерево
    bool Remove(const T& value); // удаление элемента из дерева
    void Clear(); // удаление всех элементов из дерева

    bool Find(const T& value) const; // поиск элемента в дереве

    int GetSize() const; // количество элементов в дереве
    int GetHeight() const; // высота дерева

    ~RBTree(); // деструктор (освобождение памяти)

    template <typename T1>
    friend std::ostream& operator<<(std::ostream &os, const RBTree<T1>& tree); // оператор вывода в поток
};

// получение брата узла
template <typename T>
typename RBTree<T>::Node* RBTree<T>::Sibling(Node *node) const {
	if (node->parent == nullptr) 
		return nullptr; 

	Node *parent = node->parent;

	return node == parent->left ? parent->right : parent->left; 
}

// левый поворот
template <typename T>
void RBTree<T>::RotateLeft(Node *node) {
	Node *right = node->right;
	node->right = right->left;

	if (node->right != nullptr)
		node->right->parent = node;

	right->parent = node->parent;

	if (node->parent == nullptr) {
		root = right;
	}
	else if (node == node->parent->left) {
		node->parent->left = right;
	}
	else {
		node->parent->right = right;
	}

	right->left = node;
	node->parent = right;
}

// правый поворот
template <typename T>
void RBTree<T>::RotateRight(Node *node) {
	Node *left = node->left;
	node->left = left->right;

	if (node->left)
		node->left->parent = node;

	left->parent = node->parent;

	if (node->parent == nullptr) {
		root = left;
	}
	else if (node == node->parent->left) {
		node->parent->left = left;
	}
	else {
		node->parent->right = left;
	}

	left->right = node;
	node->parent = left;
}

// корректировка узла для удовлетворения всех свойств красно-чёрного дерева после вставки
template <typename T>
void RBTree<T>::FixInsertion(Node *node) {
	while (node != root && node->color != black && node->parent->color == red) {
		Node *parent = node->parent; // получаем родителя
		Node *grandparent = node->parent->parent; // получаем дедушку
		Node *uncle = parent == grandparent->left ? grandparent->right : grandparent->left; // получаем дядю

		if (uncle && uncle->color == red) {
			grandparent->color = red;
			parent->color = black;
			uncle->color = black;

			node = grandparent;
		}
		else {
			if (parent == grandparent->left) {						
				if (node == parent->right) {
					RotateLeft(parent);
					node = parent;
					parent = node->parent;
				}

				RotateRight(grandparent);
			}
			else {
				if (node == parent->left) {
					RotateRight(parent);
					node = parent;
					parent = node->parent;
				}

				RotateLeft(grandparent);
			}
			
			std::swap(parent->color, grandparent->color);

			node = parent;
		}		
	}

	root->color = black;
}

// корректировка узла для удовлетворения всех свойств красно-чёрного дерева после удаления
template <typename T>
void RBTree<T>::FixRemoving(Node *node) {
	if (node == root) // если корень
		return; 

	Node *sibling = Sibling(node);
	Node *parent = node->parent;

	if (sibling == nullptr) { // нет брата
		FixRemoving(parent); 
	}
	else { 
		if (sibling->color == red) {  // брат красный
			parent->color = red; 
			sibling->color = black; 

			if (sibling == sibling->parent->left) { 
				RotateRight(parent); 
			}
			else {
				RotateLeft(parent); 
			}

			FixRemoving(node); 
		} else { // брат чёрный black 
			if ((sibling->left && sibling->left->color == red) || (sibling->right && sibling->right->color == red)) { // по крайней мере 1 красный ребенок
				if (sibling->left && sibling->left->color == red) { 
					if (sibling == sibling->parent->left) {
						sibling->left->color = sibling->color; 
						sibling->color = parent->color; 
						
						RotateRight(parent); 
					}
					else { 
						sibling->left->color = parent->color; 
						
						RotateRight(sibling); 
						RotateLeft(parent); 
					} 
				}
				else { 
					if (sibling == sibling->parent->left) {
						sibling->right->color = parent->color; 
						
						RotateLeft(sibling); 
						RotateRight(parent); 
					}
					else {
						sibling->right->color = sibling->color; 
						sibling->color = parent->color; 
						
						RotateLeft(parent); 
					} 
				} 
			
				parent->color = black; 
			}
			else { // 2 чёрных ребёнка
				sibling->color = red; 
				
				if (parent->color == black) {
					FixRemoving(parent); 
				}
				else {
					parent->color = black; 
				}
			} 
		} 
	} 
}

// поиск узла с заданным значением
template <typename T>
typename RBTree<T>::Node* RBTree<T>::FindNode(const T& value) const {
	Node *node = root; // начинаим искать с корня

	// пока не дойдём до листа
	while (node) {
		// если нашли нужное значение
		if (node->value == value)
			return node; // возвращаем узел

		// иначе если значение меньше текушего узла
		if (value < node->value) {
			node = node->left; // идём в левое поддерево
		}
		else {
			node = node->right; // иначе в правое поддерево
		}
	}

	return nullptr;
}

// удаление узла node
template <typename T>
void RBTree<T>::RemoveNode(Node *node) {
	Node *u = nullptr;

	// если два ребёнка
	if (node->left && node->right) {
		Node *tmp = node->right; 

		while (tmp->left) 
			tmp = tmp->left; 

		u = tmp; 
	}
	else {
		if (node->left) { // если только один ребёнок
			u = node->left; 
		}
		else if (node->right) {
			u = node->right; 
		}
	}

	bool uvBlack = (u == nullptr || u->color == black) && (node->color == black); // являются ли чёрными узел и u
	Node *parent = node->parent; 

	// u пустой узел, если node - лист
	if (u == nullptr) { 
		if (node == root) { 
			root = nullptr; // node - корень, обнуляем корень
		}
		else { 
			if (uvBlack) { // если узлы node и u чёрные 
				FixRemoving(node); 
			}
			else { // один из узлов - красный
				Node *sibling = Sibling(node);
				
				if (sibling) {
					sibling->color = red;
				}
			} 

			// удаляем узел из дерева 
			if (node == node->parent->left) { 
				parent->left = nullptr; 
			} else { 
				parent->right = nullptr; 
			} 
		} 

		delete node; 
		return; 
	} 

	// если узла один ребёнок
	if (node->left == nullptr || node->right == nullptr) { 
		if (node == root) { // node - корень, присваиваем узлу его значение и удаляем u
			node->value = u->value; 
			node->left = nullptr;
			node->right = nullptr; 
			
			delete u; 
		} else { 
			// отсоединяем узел от дерева и перемещаем его вверх 
			if (node == node->parent->left) { 
				parent->left = u; 
			} else { 
				parent->right = u; 
			} 

			delete node; 
			
			u->parent = parent; 
			
			if (uvBlack) {
				FixRemoving(u);
			} else {
				u->color = black; 
			} 
		} 

		return; 
	} 

	std::swap(u->value, node->value);
	RemoveNode(u);
}

// копирование дерева
template <typename T>
void RBTree<T>::Copy(Node *node, Node* &newNode) {
	if (node == nullptr)
		return;

	newNode = new Node;
	
	newNode->value = node->value;
	newNode->color = node->color;
	newNode->left = nullptr;
	newNode->right = nullptr;

	Copy(node->left, newNode->left); // копируем левое поддерево
	Copy(node->right, newNode->right); // копируем правое поддерево
}

// удаление элементов из дерева и освобождение памяти
template <typename T>
void RBTree<T>::Clear(Node* &node) {
	if (node == nullptr)
		return;

	Clear(node->left); // удаляем левое поддерево
	Clear(node->right); // удаляем правоеп поддерево

	delete node; // удаляем элемент
	node = nullptr; // обнуляем значение элемента
}

// количество элементов в дереве
template <typename T>
int RBTree<T>::GetSize(Node *node) const {
	if (node == nullptr)
		return 0; // лист не считается за элемент

	int leftSize = GetSize(node->left); // находим число элементов в левом поддереве
	int rightSize = GetSize(node->right); // находим число элементов в правом поддереве

	return 1 + leftSize + rightSize; // итоговое число элементов - сумма числа элементов в поддеревьях + 1
}

// высота дерева
template <typename T> 
int RBTree<T>::GetHeight(Node *node) const {
	if (node == nullptr)
		return 0; // высота листа равна 0

	int leftHeight = GetHeight(node->left); // получаем высоту левого поддеревп
	int rightHeight = GetHeight(node->right); //  получаем высоту правого поддерева

	return 1 + (leftHeight > rightHeight ? leftHeight : rightHeight); // высота узла на 1 больше, чем максимальная из высот поддеревьев
}

// вывод дерева в виде повёрнутого дерева в поток
template <typename T>
void RBTree<T>::Print(std::ostream &os, Node *node, int level) const {
	if (node == nullptr)
		return;

	Print(os, node->right, level + 1);

	for (int i = 0; i < level; i++)
		os << "    ";

	os << node->value << std::endl;

	Print(os, node->left, level + 1);
}

// конструктор по умолчанию
template <typename T>
RBTree<T>::RBTree() {
	root = nullptr; // изначально дерева нет
}

// конструктор копирования
template <typename T>
RBTree<T>::RBTree(const RBTree& tree) {
	Copy(tree.root, root);
}

// оператор присваивания
template <typename T>
RBTree<T>& RBTree<T>::operator=(const RBTree& tree) {
    if (this == &tree)
        return *this;

    Clear(root); // очищаем текущее дерево
    Copy(tree.root, root); // копируем второе дерево

    return *this;
}

template <typename T>
void RBTree<T>::Insert(const T& value) {
	Node *current = root; // текущий элемент
	Node *parent = nullptr; // родитель

	// идём до онца дерева
	while (current != nullptr) {
		parent = current; // обновляем родителя

		// переходим на левое или правое поддерево в зависимости от значения
		if (value < current->value) {
			current = current->left;
		}
		else {
			current = current->right;
		}
	}

	// создаём новый красный элемент
	Node *node = new Node;

	node->value = value;
	node->parent = parent;
	node->left = nullptr;
	node->right = nullptr;
	node->color = red;

	// если был родитель, то вставляем либо в левое, либо в правое поддерево
	if (parent) {
		if (value < parent->value) {
			parent->left = node;
		}
		else {
			parent->right = node;
		}
	}
	else {
		root = node; // иначе мы в корне, корень и есть созданный элемент
	}

	FixInsertion(node); // проверяем и корректируем выполнимость свойств красно-чёрного дерева
}

template <typename T>
bool RBTree<T>::Remove(const T& value) {
	Node *node = FindNode(value);

	// если нет нужного элемента
	if (node == nullptr)
		return false; // удаления не было

	RemoveNode(node);

	return true; // удаление было
}

// удаление всех элементов из дерева
template <typename T>
void RBTree<T>::Clear() {
	Clear(root); // вызываем рекурсивную функцию очистки от дерева
}

// поиск элемента в дереве
template <typename T>
bool RBTree<T>::Find(const T& value) const {
	return FindNode(value) != nullptr;
}

// количество элементов в дереве
template <typename T>
int RBTree<T>::GetSize() const {
	return GetSize(root); // вызываем рекурсивную версию получения размера
}

// высота дерева
template <typename T> 
int RBTree<T>::GetHeight() const {
	return GetHeight(root); // вызываем рекурсивную версию получения высоты
}

// деструктор (освобождения памяти)
template <typename T>
RBTree<T>::~RBTree() {
	Clear(root);
}

// оператор вывода в поток
template <typename T>
std::ostream& operator<<(std::ostream &os, const RBTree<T>& tree) {
    tree.Print(os, tree.root, 0);

    return os;
}