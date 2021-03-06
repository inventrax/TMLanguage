#include "ParseTree.h"
#include <iostream>
#include <sstream>
#include <ctype.h>
#include <stdio.h>

// PARSETREE.CPP
//--------------------------------------------------------------------------

std::string charToStr(char c) {
	std::stringstream ssm;
	std::string str;

	ssm << c;
	ssm >> str;

	return str;
}

//--------------------------------------------------------------------------

bool isCapital(std::string s) {
	if (s.length() > 1) {
		return false;
	}

	std::stringstream ssm;
	char c;

	ssm << s.c_str();
	ssm >> c;

	return isupper(c);
}

//--------------------------------------------------------------------------

bool containsCapitals(std::string s) {
	for (int i = 0; i < s.length(); ++i) {
		if (isCapital(charToStr(s[i])))
			return true;
	}

	return false;
}

//--------------------------------------------------------------------------

std::string removePrevious(std::string nextDerivation, std::string type) {
	std::string newStr = nextDerivation;

	// Delete all things that come before the type
	while (newStr.find(type + "(") != std::string::npos) {
		newStr.erase(newStr.begin(), newStr.begin() + newStr.find(type + "(") + 3);
	}

	// Delete everything after the type
	newStr.erase(newStr.begin() + newStr.find(")") - 1, newStr.end());

	return newStr;
}

//--------------------------------------------------------------------------

std::string parseList(std::string list) {
	std::string newStr = "";

	while(list.find("\"") != std::string::npos) {
		// Erase everything before and including the first " (aka the opening ")
		list.erase(list.begin(), list.begin() + list.find("\"") + 1);

		// Add the next character to the newStr
		newStr += list[0];

		// Erase everything before the next " (aka the closing ")
		list.erase(list.begin(), list.begin() + list.find("\"") + 1);
	}

	return newStr;
}

//--------------------------------------------------------------------------

std::string parseNodeName(std::string nextDerivation, std::string type) {
	std::cout << "PARSETREE: Parsing string: " << nextDerivation << "\nPARSETREE: of Type: " << type << "\n";

	if (type == "c" || type == "d") {
		std::cout << "PARSETREE: type is c/d/z\n";
		return removePrevious(nextDerivation, type);
	}
	else if (type == "l") {
		std::cout << "PARSETREE: type is l\n";
		std::string list = removePrevious(nextDerivation, type);
		return parseList(list);
	}
	else {
		return type;
	}
}

//--------------------------------------------------------------------------

ParseTreeNode::ParseTreeNode(std::string v, std::string n, ParseTree* t, int p) {
	value = v;

	std::cout << "PARSETREE: Adding Node: value: " << v << ", name: " << n << "\n"; 

	leafPosition = p;

	if (v == "l") {
		for (int i = 0; i < n.size(); ++i) {
			name.push_back(charToStr(n[i]));
		}
	}
	else {
		name.push_back(n);
	}

	isLeaf = true;
	tree = t;
}

//--------------------------------------------------------------------------

ParseTreeNode::ParseTreeNode(std::string v, std::string n, ParseTree* t) {
	value = v;

	std::cout << "PARSETREE: Adding Node: value: " << v << ", name: " << n << "\n"; 

	if (v == "l") {
		for (int i = 0; i < n.size(); ++i) {
			name.push_back(charToStr(n[i]));
		}
	}
	else {
		name.push_back(n);
	}

	isLeaf = true;
	tree = t;
}

//--------------------------------------------------------------------------

void ParseTreeNode::addChildren(std::string s, std::string nextDerivation, ParseTree* t) {
	std::string current;

	int p = leafPosition;

	// Update leafPositions for nodes behind this one
	for (int i = p + 1; i < tree->leafNodes.size(); ++i) {
		tree->leafNodes[i]->leafPosition += s.length() - 1;
	}

	for (int i = 0; i < s.length(); ++i) {
		current = s[i];

		// Parse name for the new node
		std::string nodeName = parseNodeName(nextDerivation, current);

		std::cout << "PARSETREE: nodeName parsed: " << nodeName << "\n";

		ParseTreeNode* newNode = new ParseTreeNode(current, nodeName, t, p);
		children.push_back(newNode);
		tree->leafNodes.insert(tree->leafNodes.begin() + p, newNode);

		p++;
	}

	isLeaf = false;
	leafPosition = -1;

	// Remove leafNodes that are no longer leaves
	tree->checkLeafNodes(s);
}

//--------------------------------------------------------------------------

ParseTree::ParseTree(std::vector<std::string>& d, std::vector<std::string>& r) {
	// Create root
	root = new ParseTreeNode(d.front(), d.front(), this, 0);
	leafNodes.push_back(root);

	ParseTreeNode* currentNode = root;

	// Add children for every derivation
	for (int i = 0; i < d.size(); ++i) {
		if (! capitalLeaves()) {
			break;
		}

		currentNode = this->findLeftMostCapital();

		std::cout << "PARSETREE: Current Derivation: " << d[i + 1] << "\n";

		std::cout << "PARSETREE: Current Node: " << currentNode->value << "\n";

		std::cout << "PARSETREE: Adding children: " << r[i] << "\n";

		// Add the new children for the step of the derivation
		currentNode->addChildren(r[i], d[i + 1], this);

		std::cout << "-------------------------------------------\n";
	}

	std::cout << "PARSETREE: Parse Tree Complete!" << "\n";
}

ParseTreeNode* ParseTree::findLeftMostCapital() {
	ParseTreeNode* currentNode;

	for (int i = 0; i < leafNodes.size(); ++i) {
		currentNode = leafNodes[i];

		if (currentNode->isLeaf && isCapital(currentNode->value)) {
			break;
		}
	}

	return currentNode;
}

void ParseTree::checkLeafNodes(std::string s) {
	// Remove nodes that are no longer leaves
	for (int i = 0; i < leafNodes.size(); ++i) {
		if (! leafNodes[i]->isLeaf) {
			leafNodes.erase(leafNodes.begin() + i);
			i--;
		}
	}
}

bool ParseTree::capitalLeaves() {
	for (int i = 0; i < leafNodes.size(); ++i) {
		if (isCapital(leafNodes[i]->value))
			return true;
	}

	return false;
}