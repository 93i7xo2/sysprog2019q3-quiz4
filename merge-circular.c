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
    struct __list *prev;
    struct __list *next;
} list;

void print(list *head);

list *sort(list *start) {
    /* If list is empty or consists of only one node. */
    if (!start || start == start->next)
        return start;

    /* Divide list into two parts. */
    list *left = start;
    list *right = left->next;
    /* For right list. */
    right->prev = left->prev;
    left->prev->next = right;
    /* For left list. */
    left->prev = left;
    left->next = left;

    left = sort(left);
    right = sort(right);

    for (list *merge = NULL; left || right;) {
        /* Use 'merge' to record the last element in list. */
        if (!right || (left && left->data < right->data)) {
            /* A head of a remaining part. */
            list *_left = NULL;
            if (left->next != left){
                /*
                 * If 'left' consists of more than one node,
                 * make the remaining part as a new circular doubly-linked list.
                 * Otherwise, set '_left' as NULL.
                 */
                left->next->prev = left->prev;
                left->prev->next = left->next;
                /* Store the new head. */
                _left = left->next;
            }
            /* Insert a node at the end of 'merge'. */
            if (!merge) {
                start = merge = left;
                left->prev = left->next = left;
            } else {
                left->prev = merge;
                left->next = merge->next;
                merge->next->prev= left;
                merge->next = left;
                merge = merge->next;
            }
            left = _left;
        } else {
            list *_right = NULL;
            if (right->next != right){
                right->next->prev = right->prev;
                right->prev->next = right->next;
                _right = right->next;
            }
            if (!merge) {
                start = merge = right;
                right->prev = right->next = right;
            } else {
                right->prev = merge;
                right->next = merge->next;
                merge->next->prev= right;
                merge->next = right;
                merge = merge->next;
            }
            right = _right;
        }
    }
    return start;
}

list *insert(list *head, int data) {
    list *ele = (list *) malloc(sizeof(list));
    ele->data = data;
    ele->prev = ele;
    ele->next = ele;
    if (!head) return ele;

    /* Insert a new node at the beginning. */
    ele->prev = head->prev;
    head->prev->next = ele;
    head->prev = ele;
    ele->next = head;
    return ele;
}

/* remove 1st element */
list *delete(list **head) {
    if (!*head) return *head;

    list *old_head;
    /* For single node. */
    if ((*head)->next == *head) {
        old_head = *head;
        *head = NULL;
        return old_head;
    }

    /* For multiple node. */
    list *new_head = (*head)->next;
    new_head->prev = (*head)->prev;
    (*head)->prev->next = new_head;
    old_head = (*head)->next = (*head)->prev = *head;
    *head = new_head;
    return old_head;
}

void print(list *head) {
    if (!head) return;

    list *tmp = head;
    do {
        printf(" %d", head->data);
        head = head->next;
    } while (head != tmp);
    printf("\n");
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
    l = sort(l);
    t2 = tvgetf();

    printf("%.10f", t2 - t1); // sec

    list *temp;
    while ((temp = delete(&l))) free(temp);
    free(testdata);

    return 0;
}