#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_FILES 100
#define DISK_SIZE 1000

typedef struct {
    char file_title[50];
    int start_sector;
    int file_size;
} FileDescriptor;

FileDescriptor file_table[MAX_FILES];
int storage_disk[DISK_SIZE] = {0};
int total_unused_sectors = 0;

void initialize_disk() {
    for (int i = 0; i < DISK_SIZE; i++) {
        storage_disk[i] = 0;
    }
}

int find_empty_sectors(int size) {
    int start_sector = -1;
    int sector_count = DISK_SIZE + 1;

    int empty_sectors = 0;
    for (int i = 0; i < DISK_SIZE; i++) {
        if (storage_disk[i] == 0) {
            empty_sectors++;
            if (empty_sectors >= size && empty_sectors < sector_count) {
                start_sector = i - empty_sectors + 1;
                sector_count = empty_sectors;
            }
        } else {
            empty_sectors = 0;
        }
    }

    return start_sector;
}

int find_file_descriptor(const char* title) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (strcmp(file_table[i].file_title, title) == 0) {
            return i;
        }
    }
    return -1; // File not found
}

void create_new_file(char* title, int size) {
    int descriptor_index = find_file_descriptor(title);
    if (descriptor_index != -1) {
        printf("Error: File with the same title already exists\n");
        return;
    }

    for (int i = 0; i < MAX_FILES; i++) {
        if (file_table[i].file_size == 0) {
            strcpy(file_table[i].file_title, title);
            file_table[i].file_size = size;

            int start_sector = find_empty_sectors(size);

            if (start_sector != -1) {
                file_table[i].start_sector = start_sector;
                for (int j = start_sector; j < start_sector + size; j++) {
                    storage_disk[j] = 1;
                }
                printf("File %s created\n", title);
            } else {
                total_unused_sectors += size;
                printf("Error: Not enough space for file %s\n", title);
                file_table[i].file_size = 0;
            }

            return;
        }
    }

    printf("Error: Maximum number of files reached\n");
}

void remove_file(char* title) {
    int descriptor_index = find_file_descriptor(title);
    if (descriptor_index != -1) {
        for (int j = file_table[descriptor_index].start_sector; j < file_table[descriptor_index].start_sector + file_table[descriptor_index].file_size; j++) {
            storage_disk[j] = 0;
        }
        file_table[descriptor_index].file_size = 0;
        printf("File %s removed\n", title);
    } else {
        printf("Error: File not found\n");
    }
}

void rename_existing_file(char* old_title, char* new_title) {
    int descriptor_index = find_file_descriptor(old_title);
    if (descriptor_index != -1) {
        if (find_file_descriptor(new_title) == -1) {
            strcpy(file_table[descriptor_index].file_title, new_title);
            printf("File %s renamed to %s\n", old_title, new_title);
        } else {
            printf("Error: A file with the new title already exists\n");
        }
    } else {
        printf("Error: File not found\n");
    }
}

void show_disk_status() {
    int used_sectors = 0;
    for (int i = 0; i < DISK_SIZE; i++) {
        if (storage_disk[i] == 1) {
            used_sectors++;
        }
    }

    int free_sectors = DISK_SIZE - used_sectors;

    printf("Disk Status:\n");
    printf("Total sectors: %d\n", DISK_SIZE);
    printf("Used sectors: %d\n", used_sectors);
    printf("Free sectors: %d\n", free_sectors);
    printf("Total unused sectors: %d\n", total_unused_sectors);
    if (used_sectors + total_unused_sectors > 0) {
        printf("Fragmentation: %.2f%%\n", (float)total_unused_sectors / (used_sectors + total_unused_sectors) * 100);
    } else {
        printf("Fragmentation: 0.00%%\n");
    }
}

int main() {
    char choice;
    char title[50];
    char new_title[50]; // Added for renaming
    int size;

    initialize_disk();

    while (1) {
        printf("\nOptions:\n");
        printf("1. Create a file\n");
        printf("2. Remove a file\n");
        printf("3. Rename a file\n"); // New option
        printf("4. Display disk status\n");
        printf("5. Exit\n"); // Adjust option numbers
        printf("Enter your choice: ");
        scanf(" %c", &choice);

        switch (choice) {
            case '1':
                printf("Enter the title of the file: ");
                scanf("%s", title);
                printf("Enter the size of the file: ");
                scanf("%d", &size);
                create_new_file(title, size);
                break;
            case '2':
                printf("Enter the title of the file to remove: ");
                scanf("%s", title);
                remove_file(title);
                break;
            case '3':
                printf("Enter the title of the file to rename: ");
                scanf("%s", title);
                printf("Enter the new title for the file: ");
                scanf("%s", new_title);
                rename_existing_file(title, new_title);
                break;
            case '4':
                show_disk_status();
                break;
            case '5':
                exit(0);
            default:
                printf("Invalid choice, please try again.\n");
        }
    }

    return 0;
}
