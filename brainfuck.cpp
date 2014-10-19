// Brainfuck interpreter by Gian Sass

#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
using namespace std;

unsigned int ptr;
char *data;
char *program;
FILE *file;
size_t memSize = 2048; // default memory size

char *deref(unsigned int ptr)
{
    if(ptr < 0 || ptr > memSize) {
        cout << "Memory access violation" << endl;
        exit(1);
    }
    return &data[ptr];
}

int main(int argc, char **argv) {
    if(argc < 2) {
        cout << "Usage: " << argv[0] << " <file> <opt:memory>" << endl;
        return 0;
    }
    if(argc == 3) {
        memSize = atoi(argv[2]);
    }
    // Open the file
    file = fopen(argv[1], "r");
    if(!file) {
        perror("Failed to open file");
        return -1;
    }
    // Get size of the file
    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Read file contents in temp buffer
    char *temp = new char[size];
    fread(temp, 1, size, file);

    // Close the file
    fclose(file);

    // Allocate new space for program
    program = new char[size];

    // Copy only valid characters to program
    // Index of temp buffer, index of program buffer
    size_t b1 = 0, b2 = 0;
    size_t psize = 0;
    while(b1 < size) {
        char c = temp[b1];
        if(c == '>' || c == '<' || c == '+' || c == '-' || c =='.' || c == ',' || c == '[' || c ==']') {
            program[b2++] = temp[b1];
            psize++;
        }
        b1++;
    }

    // We don't need this anymore
    delete temp;

    // Allocate memory
    data = new char[memSize];
    memset(data, 0, memSize);

    ptr = 0;

    // The amount of "while" blocks we were in
    int nestedLoops = 0;
    // Where the loop offsets are stored
    size_t loops[256];

    size_t i = 0;

    while(i < psize) {
        switch(program[i]) {
        case '>':
            ptr++;
            break;

        case '<':
            ptr--;
            break;

        case '+':
            (*deref(ptr))++; 
            break;

        case '-':
            (*deref(ptr))--; 
            break;

        case '.':
            cout << data[ptr];
            break;

        case ',':
            data[ptr] = (char)getchar();
            break;

        case '[':
            // If 0, skip everything to the ] for this loop
            if(data[ptr] == 0) {
                size_t tempNested = 1;
                while(tempNested != 0) {
                    i++;
                    char c = program[i];
                    if(c == '[') {
                        tempNested++;
                    } else if(c == ']') {
                        tempNested--;
                    }
                }
                i++;
                continue;
            }
            // Add new loop to next instruction
            loops[++nestedLoops] = i + 1;
            break;

        case ']':
            if(data[ptr] != 0) {
                // Go back to beginning of loop
                i = loops[nestedLoops];
                continue;
            } else {
                // The loop ended
                nestedLoops--;
            }
            break;
        }
        i++;
    }
}
