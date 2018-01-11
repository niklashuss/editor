#include <stdio.h>
#include <stdlib.h>

static const char* file_name = "../large_file.bin";

#define CHUNK_SIZE 4096

//#define log printf
#define log 

struct Node {
    Node* next;
    Node* prev;
    int size;
    void* data;
};

void* alloc_chunk() {
    void* ptr = calloc(1, CHUNK_SIZE);
    log("Allocating chunk: %p\n", ptr);
    return ptr;
}

Node* create_node() {
    Node* node = (Node*)calloc(1, sizeof(Node));
    log("Creating node: %p\n", node);
    return node;
}

Node* alloc_all_chunks(long int size) {
    log("Allocating all chunks base on size = %li\n", size);
    Node* root = create_node();
    Node* prev = root;
    for (int i = 0; i < size; i += CHUNK_SIZE) {
        Node* node = create_node();
        node->data = alloc_chunk();
        node->prev = prev;
        prev->next = node;
        prev = node; 
    }
    return root;
}

void dealloc_all_chunks(Node* node) {
    log("Deleting all chunks\n");
    while(node != nullptr) {
        if (node->data != nullptr) {
            log("Node data freed: %p\n", node->data);
            free(node->data);
        } else {
            log("Node %p has no data\n", node);
        }
        node->prev = nullptr;
        Node* next = node->next;
        log("Node freed: %p\n", node);
        free(node);
        node = next;
    }
}

long int file_size(FILE* file) {
    fseek(file, 0, SEEK_END);
    long int size = ftell(file);
    fseek(file, 0, SEEK_SET);
    return size;
}

Node* load_file(const char* file_name) {
  printf("Loading file '%s' \n", file_name);
    FILE* file = fopen(file_name, "r");
    if (file == nullptr) {
        printf("Error: Unable to open file '%s'\n", file_name);
        return nullptr;
    }
   
    long int size = file_size(file);
    Node* root = alloc_all_chunks(size);
    Node* node = root->next;
    long int left = size;
    while (node != nullptr) {
        long int tmp = left <= CHUNK_SIZE ? left : CHUNK_SIZE;
        log("Read size = %li\n", tmp);
        fread(node->data, tmp, 1, file);
        node->size = tmp;
        left -= CHUNK_SIZE;
        node = node->next;
    }
    fclose(file);
    return root;
}

void write_file(Node* node, const char* file_name) {
  printf("Save file '%s' \n", file_name, get_file_size(file_name));
  FILE* file = fopen(file_name, "w");
    if (file == nullptr) {
        printf("Error: Failed to write to file '%s'\n", file_name);
    }
    while (node != nullptr) {
        if (node->data != nullptr) {
            fwrite(node->data, node->size, 1, file);
        }
        node = node->next;
    }
    fclose(file);
}

int main(int argumentCount, char* arguments[]) {
    Node* root = load_file(file_name);
    write_file(root, "../new_file.txt");
    dealloc_all_chunks(root);
    return 0;
}
