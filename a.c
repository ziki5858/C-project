#include "headeMethods.h"

int main() {
    const char *filename = "exampleCheck";
    struct Node *head = processAssemblyText(filename);

    struct Node *current = head;
    while (current != NULL) {
        printf("Label: %s\n", current->data->label);

        switch (current->data->type_line) {
            case DIRECTIVE:
                printf("Type: Directive\n");
                printf("Directive Type: %d\n", current->data->choice.dir.directive_type);
                break;
            case INSTRUCTION:
                printf("Type: Instruction\n");
                printf("Operation Type: %d\n", current->data->choice.inst.op_type);
                break;
            case DEFINE:
                printf("Type: Define\n");
                printf("Value: %d\n", current->data->choice.def.value);
                break;
            case ERROR:
                printf("Type: Error\n");
                printf("Error Message: %s\n", current->data->choice.error);
                break;
            default:
                printf("Unknown Type\n");
                break;
        }

        printf("\n");

        current = current->next;
    }
    return 0;
}