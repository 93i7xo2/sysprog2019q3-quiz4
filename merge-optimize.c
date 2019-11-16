#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

/* timing helper function */
static double tvgetf(void) {
    struct timespec ts;
    double sec;

    clock_gettime(CLOCK_REALTIME, &ts);
    sec = ts.tv_nsec;
    sec /= 1e9;
    sec += ts.tv_sec;

    return sec;
}

typedef struct __list {
    int data;
    struct __list *next;
} list;

/*
 * Refer to https://www.geeksforgeeks.org/insertion-sort-for-singly-linked-list/
 */
void sortedInsert(list **head_ref, list *new_node) {
    list *current;
    if (*head_ref == NULL || (*head_ref)->data >= new_node->data) {
        new_node->next = *head_ref;
        *head_ref = new_node;
    } else {
        /* Insert the node at appropriate position*/
        current = *head_ref;
        while (current->next!=NULL &&
               current->next->data < new_node->data) {
            current = current->next;
        }
        new_node->next = current->next;
        current->next = new_node;
    }
}


list *sort(list *start, unsigned size) {
    if (!start || !start->next)
        return start;

    /* If list size is smaller than L1 cache size (32k), do insertion sort. */
    if (!(sizeof(list) * size >> 15)) {
        list *current = start;
        list *sorted = NULL; // To store head of sorted list.
        while (current != NULL) {
            list *next = current->next;
            sortedInsert(&sorted, current);
            current = next;
        }
        return sorted;
    }

    /* Otherwise, do merge sort. */
    list *left = start;
    list *tmp = start;

    for (int i = 1; i < size >> 1; ++i) {
        tmp = tmp->next;
    }
    list *right = tmp->next;
    tmp->next = NULL;

    left = sort(left, size >> 1);
    right = sort(right, (size + 1) >> 1);

    for (list *merge = NULL; left || right;) {
        if (!right || (left && left->data < right->data)) {
            if (!merge) {
                start = merge = left;
            } else {
                merge->next = left;
                merge = merge->next;
            }
            left = left->next;
        } else {
            if (!merge) {
                start = merge = right;
            } else {
                merge->next = right;
                merge = merge->next;
            }
            right = right->next;
        }
    }
    return start;
}

list *insert(list *head, int data) {
    list *ele = (list *) malloc(sizeof(list));
    ele->data = data;
    ele->next = NULL;
    if (!head) return ele;

    list *tail = head;
    while ((tail->next)) tail = tail->next;
    tail->next = ele;
    return head;
}

/* remove 1st element */
list *delete(list **head) {
    if (!*head) return *head;

    list* old_head = *head;
    *head = (*head)->next;
    return old_head;
}

void print(list *head) {
    if (!head) return;

    while (head) {
        printf(" %d", head->data);
        if ((head = head->next)) printf("->");
        else printf("\n");
    }
}

int main(int argc, char **argv) {
    int32_t *testdata, count;
    if (argc == 3) {
        count = atoi(argv[2]);
        testdata = (int32_t *) malloc(sizeof(int32_t) * count);
        FILE *fp = fopen(argv[1], "rb");
        if (!fp) {
            abort();
        }
        if (fread(testdata, count * sizeof(int32_t), 1, fp) != 1) {
            abort();
        }
        fclose(fp);
    } else {
        printf("Insufficient arguments.\n");
        exit(1);
    }

    // insert data
    list *l = NULL;
    for (int i = 0; i < count; ++i) {
        l = insert(l, testdata[i]);
    }

    // traversaling the list
    // print(l);

    double t1, t2;
    t1 = tvgetf();
    l = sort(l, count);
    t2 = tvgetf();
    printf("%.10f", t2 - t1); // sec

    list *temp;
    while ((temp = delete(&l))) free(temp);
    free(testdata);

    return 0;
}