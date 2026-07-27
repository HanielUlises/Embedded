#include <iostream>

struct ListNode {
    int val;
    ListNode *next;
    ListNode(int x) : val(x), next(nullptr) {}
};

ListNode* mergeTwoLists(ListNode* l1, ListNode* l2) {
    // TODO: implement this function as per the instructions
    ListNode *head = nullptr;
    if(l1 -> val < l2 -> val) {
        head = l1;
        l1 = l1 -> next;
    } else {
        head = l2;
        l2 = l2 -> next;
    }
    
    ListNode *tail = head;

    while(l1 && l2) {
        if(l1 -> val > l2 -> val) {
            tail -> next = l1;
            l1 = l1 -> next;
        } else {
            tail -> next = l2;
            l2 = l2 -> next;
        }

        tail = tail -> next;
    }

    tail->next = l1 ? l1 : l2;

    return head;
}

// Helper function to print the list (for testing purposes)
void printList(ListNode* node) {
    while (node != nullptr) {
        std::cout << node->val << " -> ";
        node = node->next;
    }
}

int main() {
    // Example usage:
    // List 1: 1 -> 3 -> 5
    ListNode* l1 = new ListNode(1);
    l1->next = new ListNode(3);
    l1->next->next = new ListNode(5);

    // List 2: 2 -> 4 -> 6
    ListNode* l2 = new ListNode(2);
    l2->next = new ListNode(4);
    l2->next->next = new ListNode(6);

    ListNode* mergedList = mergeTwoLists(l1, l2);

    // Expected outcome : 1 -> 2 -> 3 -> 4 -> 5 -> 6
    printList(mergedList);

    return 0;
}   