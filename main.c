#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define COLOR_RESET "\033[0m"
#define COLOR_GREEN "\033[32m"
#define COLOR_YELLOW "\033[33m"
#define COLOR_RED "\033[31m"
#define COLOR_BLUE "\033[34m"
#define COLOR_CYAN "\033[36m"
#define COLOR_MAGENTA "\033[35m"
#define COLOR_WHITE "\033[37m"
#define COLOR_BOLD "\033[1m"
#define COLOR_UNDERLINE "\033[4m"
#define COLOR_BOLD_YELLOW "\033[1;33m"

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
        printf(COLOR_RED "Error opening file: %s\n" COLOR_RESET, filename);
        return;
    }
    char line[256]; //buffer to put the text from file to display
    int itemNumber = 1;

    printf(COLOR_YELLOW "==== Menu ====\n" COLOR_RESET);
    while (fgets(line, sizeof(line), file)) {
        char *dollarSign = strchr(line, '$'); // Find the dollar sign in the line
        if (dollarSign) {
            *dollarSign = '\0'; // Temporarily terminate the string before the dollar sign
            printf("%d. %s" COLOR_GREEN "$" COLOR_RESET "%s", itemNumber++, line, dollarSign + 1);
        } else {
            printf("%d. %s", itemNumber++, line); // If no dollar sign, print normally
        }
    }
    fclose(file);
}

void customizePizza(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf(COLOR_RED "Error opening file %s" COLOR_RESET "\n", filename);
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
        printf(COLOR_YELLOW "\n==== Customize Your Pizza ====\n" COLOR_RESET);
        for (int i = 0; i < toppingCount; i++) {
            char *d = strchr(toppings[i], '$');
            if (d) {
                int len = d - toppings[i];
                printf("%d. %.*s" COLOR_GREEN "$" COLOR_RESET "%s \n",
                       i+1,
                       len,                // print text before '$'
                       toppings[i],
                       d+1);              // print price after '$'
            } else {
                printf("%d. " COLOR_CYAN "%s" COLOR_RESET "\n", i+1, toppings[i]);
            }
        }
        printf("--------------------------");
        printf("\n%d. Back to Restaurant Menu\n", toppingCount + 1);
        printf("\nEnter topping number to add to your pizza or exit: ");
        scanf("%d", &choice);

        if (choice > 0 && choice <= toppingCount) {
            clearScreen();
            int found = 0;

            // Check if the pizza already exists in the orders
            for (int i = 0; i < orderCount; i++) {
                if (strcmp(orders[i].item, toppings[choice - 1]) == 0 &&
                    strcmp(orders[i].restaurant, "Restaurant 1") == 0) {
                    orders[i].quantity++;
                    found = 1;
                    printf("Increased quantity of " COLOR_BOLD_YELLOW "%s" COLOR_RESET " to " COLOR_GREEN "%d\n" COLOR_RESET,
                           orders[i].item, orders[i].quantity);
                    break;
                }
            }

            // If not found, add a new order
            if (!found) {
                if (orderCount < MAX_ORDERS) {
                    strcpy(orders[orderCount].item, toppings[choice - 1]);
                    strcpy(orders[orderCount].restaurant, "Restaurant 1");
                    orders[orderCount].quantity = 1;
                    orderCount++;
                    printf("Pizza with " COLOR_BOLD_YELLOW "%s" COLOR_RESET " added to order successfully!\n", toppings[choice - 1]);
                } else {
                    printf(COLOR_RED "Order limit reached. Cannot add more items." COLOR_RESET "\n");
                }
            }

            printf("\nPress anything to continue!\n");
            getchar(); getchar();
        }
    } while (choice != toppingCount + 1);
}

void saveOrder(DeliveryInfo info) {
    FILE *file = fopen("order_history.csv", "a");
    if (file == NULL) {
        printf(COLOR_RED "Error opening file: %p\n" COLOR_RESET, file);
        return;
    }

    // Write customer info
    fprintf(file, "\"%s\",\"%s\",\"%s\",\"", info.name, info.address, info.phone);

    double totalOrder = 0;

    // Write all ordered items with their total price
    for (int i = 0; i < orderCount; i++) {
        const char *dollarSign = strchr(orders[i].item, '$');
        if (dollarSign) {
            int price = atoi(dollarSign + 1);
            double totalItem = price * orders[i].quantity;
            totalOrder += totalItem;

            fprintf(file, "%s from %s (x%d) - $%.2f",
                    orders[i].item,
                    orders[i].restaurant,
                    orders[i].quantity,
                    totalItem);

            if (i < orderCount - 1) {
                fprintf(file, "; ");
            }
        }
    }

    // Write total order price
    fprintf(file, "\",\"$%.2f\"\n", totalOrder);
    fclose(file);
}

void processOrder() { //saves order to file
    DeliveryInfo info;
    clearScreen();
    printf(COLOR_YELLOW "==== Order Placement ====\n" COLOR_RESET);

    printf(COLOR_CYAN "Enter your name " COLOR_RESET "(example: " COLOR_GREEN "anto" COLOR_RESET "): ");
    getchar();
    fgets(info.name, sizeof(info.name), stdin);
    info.name[strcspn(info.name, "\n")] = 0;

    printf(COLOR_CYAN "Enter delivery address " COLOR_RESET "(example: " COLOR_GREEN "street 1" COLOR_RESET "): ");
    fgets(info.address, sizeof(info.address), stdin);
    info.address[strcspn(info.address, "\n")] = 0;

    printf(COLOR_CYAN "Enter phone number " COLOR_RESET "(example: " COLOR_GREEN "zzz-zzz-zzz" COLOR_RESET "): ");
    fgets(info.phone, sizeof(info.phone), stdin);
    info.phone[strcspn(info.phone, "\n")] = 0;

    // Save the order to the file
    saveOrder(info);

    printf(COLOR_GREEN "\nOrder placed successfully!\n" COLOR_RESET);

    // Clear the current orders
    orderCount = 0;
    memset(orders, 0, sizeof(orders)); // Reset the orders array

    printf("Press anything to return to the main menu...");
    getchar();
}

void placeOrder(const char *restaurantName, const char *filename) { //remembers order
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf(COLOR_RED "Error: Could not open menu file %s\n" COLOR_RESET, filename);
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
            char *d = strchr(menuItems[i], '$');
            if (d) {
                int len = d - menuItems[i];
                printf("%d. %.*s" COLOR_GREEN "$" COLOR_RESET "%s\n",
                       i+1,
                       len,                // print text before '$'
                       menuItems[i],
                       d+1);              // print the price
            } else {
                printf("%d. %s\n", i+1, menuItems[i]);
            }
        }


        printf("--------------------------\n");
        printf("%d. Back to Restaurant Menu\n", itemCount + 1);
        printf("\nEnter item number to order or exit: ");
        scanf("%d", &choice);

        if (choice > 0 && choice <= itemCount) {
            clearScreen();
            int found = 0;
            //check if item is already in list
            for (int i = 0; i < orderCount; i++) {
                if (strcmp(orders[i].item, menuItems[choice-1]) == 0 && strcmp(orders[i].restaurant, restaurantName) == 0) {
                    orders[i].quantity++;
                    found = 1;
                    char itemName[256];
                    char price[32];
                    strcpy(itemName, orders[i].item);
                    char *dollarPos = strchr(itemName, '$');
                    if (dollarPos) {
                        *dollarPos = '\0';
                        printf("\t Increased quantity of %s" COLOR_GREEN "$" COLOR_RESET "%s to " COLOR_BLUE "%d" COLOR_RESET "\n",
                               itemName, dollarPos + 1, orders[i].quantity);
                    } else {
                        printf("\t Increased quantity of %s to " COLOR_BLUE "%d" COLOR_RESET "\n",
                               orders[i].item, orders[i].quantity);
                    }
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

                    char itemName[256];
                    strcpy(itemName, menuItems[choice-1]);
                    char *dollarPos = strchr(itemName, '$');
                    if (dollarPos) {
                        *dollarPos = '\0';
                        printf("Added %s" COLOR_GREEN "$" COLOR_RESET "%s to order successfully!\n",
                               itemName, dollarPos + 1);
                    } else {
                        printf("\n Item added to order successfully!!!\n");
                    }
                    printf("\t Press anything to continue\n");
                    printf("\n");
                    getchar();
                } else {
                    printf(COLOR_RED "Item not found\n" COLOR_RESET);
                }
            }
        }


    } while (choice != itemCount + 1 && choice <= 3 && choice > 0);
}
double calculateItemTotal(const char *item) {
    const char *dollarSign = strchr(item, '$');
    const char *multiplier = strstr(item, "(x");
    double total = 0;

    if (dollarSign && multiplier) {
        int price = atoi(dollarSign + 1); // Extract price after '$'
        int quantity = atoi(multiplier + 2); // Extract quantity after '(x'
        total = price * quantity;
    }

    return total;
}

void previousOrders() {
    FILE *file = fopen("order_history.csv", "r");
    if (file == NULL) {
        printf(COLOR_RED "No order history found.\n" COLOR_RESET);
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
            printf(COLOR_CYAN "Customer: " COLOR_RESET "%s\n", token);
            token = strtok(NULL, "\",\"");
        }
        if (token) {
            printf(COLOR_CYAN "Address: " COLOR_RESET "%s\n", token);
            token = strtok(NULL, "\",\"");
        }
        if (token) {
            printf(COLOR_CYAN "Phone: " COLOR_RESET "%s\n", token);
            token = strtok(NULL, "\",\"");
        }
        if (token) {
            printf(COLOR_CYAN "Items: " COLOR_RESET);
            char *item = strtok(token, ";");
            double orderTotal = 0;

            while (item) {
                char *dollarSign = strchr(item, '$');
                char *multiplier = strstr(item, "(x");
                char *restaurant = strstr(item, "from ");
                double itemTotal = 0;

                if (dollarSign && multiplier) {
                    int price = atoi(dollarSign + 1); // Extract price after '$'
                    int quantity = atoi(multiplier + 2); // Extract quantity after '(x'
                    itemTotal = price * quantity;
                    orderTotal += itemTotal;

                    // Truncate and display the item with the unit price and total
                    *dollarSign = '\0';
                    *multiplier = '\0';
                    if (restaurant) {
                        *restaurant = '\0';
                        printf("%s " COLOR_GREEN "$%d" COLOR_RESET " PER UNIT ==> PRICE FOR x%d UNIT/S = " COLOR_GREEN "$%.2f" COLOR_RESET" FROM %s; ",
                               item, price, quantity, itemTotal, restaurant + 5);
                    } else {
                        printf("%s " COLOR_GREEN "$%d" COLOR_RESET " PER UNIT ==> " COLOR_GREEN "$%.2f" COLOR_RESET " (x%d); ",
                               item, price, itemTotal, quantity);
                    }
                }
                item = strtok(NULL, ";");
            }
            printf("\n" COLOR_BOLD "Order Total: " COLOR_GREEN "$%.2f\n" COLOR_RESET, orderTotal);
        }
        printf("-----------------\n");
    }

    if (orderNum == 0) {
        printf(COLOR_RED "No orders found.\n" COLOR_RESET);
    }

    fclose(file);
}

void viewOrderHistory(){
    int choice;
    do {
        clearScreen();
        printf(COLOR_YELLOW"\n===== Order History: =====\n" COLOR_RESET);
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
                printf(COLOR_YELLOW"==== Current order view ====\n" COLOR_RESET);
                if (orderCount == 0) {
                    printf(COLOR_RED"No current order!" COLOR_RESET);
                } else {
                    for (int i = 0; i < orderCount; i++) {
                        char itemName[256];
                        strcpy(itemName, orders[i].item);
                        char *dollarPos = strchr(itemName, '$');
                        if (dollarPos) {
                            *dollarPos = '\0';
                            printf("%s" COLOR_GREEN "$" "%s" COLOR_RESET " from "
                                   COLOR_MAGENTA "%s" COLOR_RESET ", x" COLOR_GREEN "%d" COLOR_RESET "\n",
                                   itemName,
                                   dollarPos + 1,
                                   orders[i].restaurant,
                                   orders[i].quantity);
                        } else {
                            printf(COLOR_CYAN "%s" COLOR_RESET " from " COLOR_MAGENTA "%s" COLOR_RESET ", x"
                                   COLOR_GREEN "%d" COLOR_RESET "\n",
                                   orders[i].item,
                                   orders[i].restaurant,
                                   orders[i].quantity);
                        }
                    }
                    printf("\nWant to place this order? (1=" COLOR_GREEN "yes" COLOR_RESET " / 2=" COLOR_RED "no" COLOR_RESET ") : \n");
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
                printf(COLOR_BOLD_YELLOW"\n===== Previous Orders =====\n" COLOR_RESET);
                previousOrders();
                printf("\nPress anything to continue: ");
                getchar();
                break;

            case 3:
                clearScreen();
                printf("Returning to Main Menu..\n");
                break;
            default:
                printf(COLOR_RED "Invalid Choice\n" COLOR_RESET);
                printf("Press anything to continue..");
                getchar();
        }

    } while (choice !=3);
}


void menuRestaurant(const char *restaurantName, const char *filename) {
    int choice;
    do {
        clearScreen();
        printf(COLOR_YELLOW"==== Menu of: %s ==== \n" COLOR_RESET, restaurantName);
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
                //printf(COLOR_YELLOW"==== %s Menu Items ====\n" COLOR_RESET, restaurantName);
                displayMenu(filename);
                //printf("\nDisplaying menu items...\n");
                printf("\n");
                printf("Press anything to return:");
                getchar();
                break;

            case 2:
                clearScreen();
                printf(COLOR_YELLOW "==== Place an order at %s ====\n" COLOR_RESET, restaurantName);
                placeOrder(restaurantName, filename);
                break;
            case 3:
                if(strcmp(restaurantName, "Restaurant 1") == 0) {
                    clearScreen();
                    printf(COLOR_YELLOW"==== Customize a Pizza ====\n" COLOR_RESET);
                    customizePizza("pizza_toppings.txt");
                } else {
                    clearScreen();
                    printf("\nReturning to main menu....\n");
                }
                break;
            case 4:
                if (strcmp(restaurantName, "Restaurant 1") == 0) {
                    clearScreen();
                    printf("\nReturning to Main Menu...\n");
                } else {
                    clearScreen();
                    printf(COLOR_RED"Invalid choice. Try Again\n" COLOR_RESET);
                    getchar();
                }
                break;
            default:
                clearScreen();
                printf(COLOR_RED "Invalid choice. Try Again\n" COLOR_RESET);
                getchar();
        }
    } while ((strcmp(restaurantName, "Restaurant 1") == 0 && choice != 4) ||
         (strcmp(restaurantName, "Restaurant 1") != 0 && choice != 3));
}

int main(void) {
    int choice;
    do {
        clearScreen();
        printf(COLOR_YELLOW"\nWelcome to the Food Ordering App!\n" COLOR_RESET);
        printf("================================\n");
        for (int i = 0; i < 3; i++) {
            printf("%d. Restaurant" COLOR_BOLD_YELLOW" %d\n" COLOR_RESET, i + 1, i + 1);
        }
        printf("4. View order history\n");
        printf("5. Exit\n");
        printf("================================\n");
        printf("Enter your choice(1 trough 5): ");
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