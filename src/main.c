#include "models.h"
#include "utils.h"
#include "menu.h"
#include "inventory.h"
#include "loyalty.h"
#include "storage.h"
#include "auth.h"
#include "ui.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void system_initialize(void) {
    menu_init();
    inventory_init();
    loyalty_init();
    coupon_init();
    auth_init();

    storage_ensure_data_dir();

    // Attempt loading persistent records
    storage_load_menu(MENU_FILE);
    storage_load_inventory(INVENTORY_FILE);
    storage_load_customers(CUSTOMERS_FILE);
    storage_load_coupons(COUPONS_FILE);

    // Seed defaults if fresh installation
    storage_seed_defaults_if_empty();
}

static void show_login_screen(void) {
    ui_banner();
    printf("\n" ANSI_BOLD "Staff Login Portal" ANSI_RESET "\n");
    printf(ANSI_WHITE "(Default demo accounts: 'admin' / 'admin123' or 'barista' / 'coffee123')" ANSI_RESET "\n\n");

    char username[MAX_NAME_LEN];
    char password[MAX_NAME_LEN];

    while (1) {
        utils_read_string("Username: ", username, sizeof(username));
        if (strcmp(username, "exit") == 0 || strcmp(username, "quit") == 0) {
            printf("\nExiting system. Goodbye!\n");
            exit(0);
        }

        utils_read_password("Password: ", password, sizeof(password));

        const User *u = auth_login(username, password);
        if (u) {
            printf(ANSI_GREEN "✓ Welcome, %s (%s)!\n" ANSI_RESET, u->display_name,
                   u->role == ROLE_ADMIN ? "Administrator" : "Cashier / Barista");
            break;
        } else {
            printf(ANSI_RED "Invalid credentials. Please try again (or type 'exit' to quit).\n" ANSI_RESET);
        }
    }
}

int main(void) {
    system_initialize();
    show_login_screen();

    while (1) {
        const User *curr = auth_get_current_user();
        if (!curr) {
            show_login_screen();
            curr = auth_get_current_user();
        }

        utils_print_header("MAIN PORTAL");
        printf("Current Staff: " ANSI_BOLD ANSI_GREEN "%s" ANSI_RESET " (Role: %s)\n\n",
               curr->display_name, curr->role == ROLE_ADMIN ? "Admin" : "Cashier");

        printf("  1: Cashier POS Register (New Order)\n");
        printf("  2: View Menu Catalog\n");
        printf("  3: View Inventory Levels\n");

        if (curr->role == ROLE_ADMIN) {
            printf("  4: Administrator Management Console\n");
            printf("  5: Switch Staff Account / Logout\n");
            printf("  6: Exit Application\n");
        } else {
            printf("  4: Switch Staff Account / Logout\n");
            printf("  5: Exit Application\n");
        }

        int max_opt = (curr->role == ROLE_ADMIN) ? 6 : 5;
        int choice = utils_read_int("Enter selection: ", 1, max_opt);

        if (choice == 1) {
            ui_cashier_pos_flow();
        } else if (choice == 2) {
            ui_view_menu();
        } else if (choice == 3) {
            ui_view_inventory();
        } else if (choice == 4 && curr->role == ROLE_ADMIN) {
            ui_admin_dashboard();
        } else if ((choice == 4 && curr->role != ROLE_ADMIN) || (choice == 5 && curr->role == ROLE_ADMIN)) {
            auth_logout();
            printf(ANSI_YELLOW "Logged out successfully.\n" ANSI_RESET);
            show_login_screen();
        } else {
            printf(ANSI_GREEN "\nThank you for using Artisan Coffee Shop System. Shutting down.\n" ANSI_RESET);
            break;
        }
    }

    return 0;
}
