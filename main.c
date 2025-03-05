#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ORDERS 50

typedef struct { // struct to keep track of user orders stored in orders[]
    //char user[128];
    char item[256];
    char restaurant[50];
} Order;

Order orders[MAX_ORDERS];
int orderCount = 0;

void displayMenu(const char *filename) {
    FILE *file = fopen(filename, "r");

    if (file == NULL) {
        perror("Error opening file");
        printf("Error opening file %s \n", filename);
        return;
    }
    char line[256]; //buffer to put the text from file to display
    printf("\nMenu:\n");
    while (fgets(line, sizeof(line), file)) {
        printf("%s", line);
        printf("\n");
    }
    fclose(file);

}
void customizePizza(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        printf("Error opening file %s \n", filename);
        return;
    }



}

void placeOrder(const char *restaurantName, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error: Could not open menu file %s\n", filename);
        return;
    }

    char menuItems[MAX_ORDERS][256];
    int itemCount = 0;
    while (fgets(menuItems[itemCount], sizeof(menuItems[itemCount]), file)) {
        itemCount++;
    }
    fclose(file);

    int choice;
    do {
        printf("%s Menu:\n", restaurantName);
        for (int i = 0; i < itemCount; i++) {
            printf(" %s", menuItems[i]);
        }
        printf("%d. Back to Restaurant Menu\n", itemCount + 1);
        printf("Enter item number to order: ");
        scanf("%d", &choice);

        if (choice > 0 && choice <= itemCount) {
            if (orderCount < MAX_ORDERS) {
                strcpy(orders[orderCount].item, menuItems[choice - 1]);
                strcpy(orders[orderCount].restaurant, restaurantName);
                orderCount++;
                printf("\nItem added to order!\n");
            } else {
                printf("\nOrder list is full!\n");
            }
        }
    } while (choice != itemCount + 1);
}


void menuRestaurant(const char *restaurantName, const char *filename) {
    int choice;
    do {
        printf("%s MENU:\n", restaurantName);
        printf("1. View Menu items\n");
        printf("2. Order from this Restaurant\n");
        printf("3. Back to Main Menu\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                printf("\nDisplaying menu items...\n");
            displayMenu(filename);
            break;
            case 2:
                //printf("\nCustomize a pizza...\n");
                    placeOrder(restaurantName, filename);
            break;
            case 3:
                printf("\nReturning to main menu...\n");
            break;
            default:
                printf("Invalid choice, try again\n");
        }
        printf("\nPress anything to continue:");
        getchar();
        getchar(); //wait for user input
    } while (choice != 3);
}

void viewOrderHistory(){
    printf("\nOrder History:\n");
    if (orderCount == 0) { //check if previous orders exist
        printf("\nEmpty order list!\n");
        printf("Start a new order.\n");
    } else {
        for (int i = 0; i < orderCount; i++) {
            printf("%d. %s (From: %s)\n", i + 1, orders[i].item, orders[i].restaurant);
            //shows order items and restaurant name
        }
    }
    printf("\nPress anything to continue:");
    getchar();
    getchar();

}

int main(void) {
    //system("cls"); //clearing terminal
    //system("color 0F");

    int choice;
    do {
        printf("\nWelcome to the Food Ordering App!\n");
        printf("================================\n");
        printf("1. Restaurant 1\n");
        printf("2. Restaurant 2\n");
        printf("3. Restaurant 3\n");
        printf("4. View order history\n");
        printf("5. Exit\n");
        printf("================================\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        switch (choice) {
            case 1: menuRestaurant("Restaurant 1", "restaurant1.txt");
            break;
            case 2: menuRestaurant("Restaurant 2", "restaurant2.txt");
            break;
            case 3: menuRestaurant("Restaurant 3", "restaurant3.txt");
            break;
            case 4: viewOrderHistory();
            break;
            default:printf("Exiting...\n");
            getchar();
            getchar();
        }
    }while (choice != 5);

    return 0;
}