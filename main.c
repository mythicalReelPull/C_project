#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}


#define MAX_ORDERS 50
typedef struct { // struct to keep track of user orders stored in orders[]
    char item[256];
    char restaurant[50];
    int quantity; //bool
} Order;

Order orders[MAX_ORDERS];
int orderCount = 0;



typedef struct { //user order info struct
    char name[256];
    char address[256];
    char phone[20];
} DeliveryInfo;

void displayMenu(const char *filename) {
    FILE *file = fopen(filename, "r");

    if (file == NULL) {
        perror("Error opening file");
        printf("Error opening file %s \n", filename);
        return;
    }
    char line[256]; //buffer to put the text from file to display

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

    char line[256];
    char toppings[MAX_ORDERS][256];
    int toppingCount = 0;

    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = 0;
        strcpy(toppings[toppingCount], line);
        toppingCount++;
    }
    fclose(file);

    int choice;
    do {
        clearScreen();
        printf("==== Customize Your Pizza ====\n");
        for (int i = 0; i< toppingCount; i++) {
            printf("%d. %s\n", i+1,toppings[i]);
        }
        printf("%d. Back to Restaurant Menu\n", toppingCount + 1);
        printf("Enter topping number to add to your pizza or exit: ");
        scanf("%d", &choice);

        if (choice> 0 && choice <= toppingCount) {
            clearScreen();
            if (orderCount < MAX_ORDERS) {
                char pizzaOrder[256];
                snprintf(pizzaOrder, sizeof(pizzaOrder), "%s", toppings[choice-1]);
                strcpy(orders[orderCount].item, pizzaOrder);
                strcpy(orders[orderCount].restaurant, "Restaurant 1");
                orders[orderCount].quantity = 1;
                orderCount++;
                if (orders[orderCount].quantity > 0)
                printf("Pizza with %s added to order successfully!\n", toppings[choice-1]);
                printf("Press anything to continue\n");
                getchar(); getchar();
            } else {
                printf("Order limit reached. Cannot add more items.\n");
                printf("\nPress anything to continue\n");
                getchar(); getchar();
            }
        }
    } while (choice != toppingCount + 1);

}

void saveOrder(DeliveryInfo info) { //saves info to csv file
    FILE *file = fopen("order_history.csv", "a");
    if (file == NULL) {
        printf("Error opening file: %p \n", file);
        return;
    }
    //write customer info
    fprintf(file, "\"%s\",\"%s\",\"%s\",\"", info.name, info.address, info.phone);

    //write all ordered items
    for (int i = 0; i < orderCount; i++) {
        fprintf(file, "%s from %s (x%d)",
                orders[i].item,
                orders[i].restaurant,
                orders[i].quantity);
        if (i < orderCount - 1) {
            fprintf(file, "; ");
        }
    }
    fprintf(file, "\"\n");
    fclose(file);
}

void processOrder() { //reads info to register user
    DeliveryInfo info;
    clearScreen();
    printf("==== Order Placement ====\n");
    printf("Enter your name: ");
    getchar();
    fgets(info.name, sizeof(info.name), stdin);
    info.name[strcspn(info.name, "\n")] = 0;

    printf("Enter delivery address: ");
    fgets(info.address, sizeof(info.address), stdin);
    info.address[strcspn(info.address, "\n")] = 0;

    printf("Enter phone number: ");
    fgets(info.phone, sizeof(info.phone), stdin);
    info.phone[strcspn(info.phone, "\n")] = 0;

    saveOrder(info);
    printf("\nOrder placed successfully!\n");

    // Clear current orders after saving
    //orderCount = 0;

    printf("Press anything to continue...");
    getchar();
}

void placeOrder(const char *restaurantName, const char *filename) { //remembers order
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error: Could not open menu file %s\n", filename);
        return;
    }

    char menuItems[MAX_ORDERS][256];
    int itemCount = 0;
    while (fgets(menuItems[itemCount], sizeof(menuItems[itemCount]), file)) {
        menuItems[itemCount][strcspn(menuItems[itemCount], "\n")] = 0;
        itemCount++;
    }
    fclose(file);

    int choice;
    do {

        for (int i = 0; i < itemCount; i++) {
            printf("%d. %s\n",i+1, menuItems[i]);
        }
        printf("--------------------------\n");
        printf("%d.Back to Restaurant Menu\n", itemCount + 1);
        printf("Enter item number to order or exit: ");
        scanf("%d", &choice);

        if (choice > 0 && choice <= itemCount) {
            clearScreen();
            int found = 0;
            //check if item is already in list
            for (int i = 0; i < orderCount; i++) {
               if (strcmp(orders[i].item, menuItems[choice-1]) == 0 && strcmp(orders[i].restaurant, restaurantName) == 0) {
                   orders[i].quantity ++;
                   found = 1;
                   printf("Increased quantity of '%s' to %d \n", orders[i].item, orders[i].quantity);
                   printf("\n");
                   getchar();
                   break;
               }
            }
            if (!found) {
                if (orderCount < MAX_ORDERS) {
                    strcpy(orders[orderCount].item, menuItems[choice-1]);
                    strcpy(orders[orderCount].restaurant, restaurantName);
                    orders[orderCount].quantity = 1;
                    orderCount++;
                    printf("Item added to order successfully!!!\n");
                    //printf(orders[orderCount].item);
                    printf("\n");
                    printf("Press anything to continue\n");
                    getchar();

                }else{
                    printf("Item not found\n");
                }

            }
        }


    } while (choice != itemCount + 1 && choice <= 3 && choice > 0);
}

void previousOrders() {
    FILE *file = fopen("order_history.csv", "r");
    if (file == NULL) {
        printf("No order history found.\n");
        return;
    }

    char line[1024];
    int orderNum = 0;

    // Skip header line
    fgets(line, sizeof(line), file);

    while (fgets(line, sizeof(line), file)) {
        orderNum++;
        char *token = strtok(line, "\",\"");
        printf("\nOrder %d:\n", orderNum);
        printf("-----------------\n");

        if (token) {
            printf("Customer: %s\n", token);
            token = strtok(NULL, "\",\"");
        }
        if (token) {
            printf("Address: %s\n", token);
            token = strtok(NULL, "\",\"");
        }
        if (token) {
            printf("Phone: %s\n", token);
            token = strtok(NULL, "\",\"");
        }
        if (token) {
            printf("Items: %s\n", token);
        }
        printf("-----------------\n");
    }

    if (orderNum == 0) {
        printf("No orders found.\n");
    }

    fclose(file);
}

void viewOrderHistory(){
    int choice;
    do {
        clearScreen();
        printf("\n===== Order History: =====\n");
        if (orderCount != 0) {
            printf("1. View current order\n");
        }else {
            printf("You haven't placed any new orders. Start a new order\n");
        }
        printf("2. View previous order history\n");
        printf("3. Return to main menu\n");
        printf("\nEnter your choice: \n");
        scanf("%d", &choice);
        getchar();

        switch (choice) {
            case 1:
                clearScreen();
                printf("==== Current order view ====\n");
                if (orderCount == 0) {
                    printf("No current order!");
                } else {
                    for (int i = 0; i < orderCount; i++) {
                        printf("%s from %s, x%d\n",
                        orders[i].item,
                        orders[i].restaurant,
                        orders[i].quantity);
                    }
                    printf("\nWant to place this order? 1(yes) / 2(no) \n");
                    int placeChoice;
                    scanf("%d", &placeChoice);
                    if (placeChoice == 1) {
                        processOrder();
                        break;
                    }

                }
                printf("\nPress anything to continue: ");
                getchar();
                break;

            case 2:
                clearScreen();
                printf("\n===== Previous Orders =====\n");
                previousOrders();
                printf("\nPress anything to continue: ");
                getchar();
                break;

            case 3:
                clearScreen();
                printf("Returning to main menu..\n");
                break;
            default:
                printf("Invalid choice\n");
                printf("Press anything to continue..");
                getchar();
        }

    } while (choice !=3);
}


void menuRestaurant(const char *restaurantName, const char *filename) {
    int choice;
    do {
        clearScreen();
        printf("==== Menu of: %s ==== \n", restaurantName);
        printf("1. View Menu items\n");
        printf("2. Order from this Restaurant\n");
        if (strcmp(restaurantName, "Restaurant 1") == 0) { //added an optional view for choosing to customize a pizza
            printf("3. Customize a Pizza\n");
            printf("4. Back to Main Menu\n");
        }else {
            printf("3. Back to Main Menu\n");
        }
        printf("Enter your choice: ");
        scanf("%d", &choice);
        getchar();

        switch (choice) {
            case 1:
                clearScreen();
                printf("==== %s Menu Items ====\n", restaurantName);
                displayMenu(filename);
                //printf("\nDisplaying menu items...\n");
                printf("\n");
                printf("Press anything to continue:");
                getchar();
                break;

            case 2:
                clearScreen();
                printf("==== Place an order at %s ====\n", restaurantName);
                placeOrder(restaurantName, filename);
                break;
            case 3:
                if(strcmp(restaurantName, "Restaurant 1") == 0) {
                    clearScreen();
                    printf("==== Customize a Pizza ====\n");
                    customizePizza("pizza_toppings.txt");
                } else {
                    clearScreen();
                    printf("\nReturning to main menu....\n");
                }
                break;
            case 4:
                if (strcmp(restaurantName, "Restaurant 1") == 0) {
                    clearScreen();
                    printf("\nReturning to main menu...\n");
                } else {
                    clearScreen();
                    printf("Invalid choice, try again\n");
                    getchar();
                }
                break;
            default:
                clearScreen();
                printf("Invalid choice, try again\n");
                getchar();
        }
    } while ((strcmp(restaurantName, "Restaurant 1") == 0 && choice != 4) ||
         (strcmp(restaurantName, "Restaurant 1") != 0 && choice != 3));
}

int main(void) {
    int choice;
    do {
        clearScreen();
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
            default:printf("Exiting... Bye!:)\n");
            getchar();
        }
    }while (choice != 5);

    return 0;
}