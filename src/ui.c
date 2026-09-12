#include "ui.h"
#include "menu.h"
#include "inventory.h"
#include "order.h"
#include "loyalty.h"
#include "payment.h"
#include "storage.h"
#include "analytics.h"
#include "auth.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int s_next_order_id = 1001;

void ui_banner(void) {
    printf(ANSI_BOLD ANSI_CYAN);
    printf("   ____       __  __            ____  _                   ____             __                 \n");
    printf("  / ___|___  / _|/ _| ___  ___ / ___|| |__   ___  _ __   / ___| _   _ ___ / /____ _ __ ___    \n");
    printf(" | |   / _ \\| |_| |_ / _ \\/ _ \\\\___ \\| '_ \\ / _ \\| '_ \\  \\___ \\| | | / __| __/ _ \\ '_ ` _ \\   \n");
    printf(" | |__| (_) |  _|  _|  __/  __/ ___) | | | | (_) | |_) |  ___) | |_| \\__ \\ ||  __/ | | | | |  \n");
    printf("  \\____\\___/|_| |_|  \\___|\\___||____/|_| |_|\\___/| .__/  |____/ \\__, |___/\\__\\___|_| |_| |_|  \n");
    printf("                                                 |_|             |___/                         \n");
    printf(ANSI_RESET);
}

void ui_view_menu(void) {
    utils_print_header("MENU CATALOG");
    menu_print_catalog(true);
    utils_pause();
}

void ui_view_inventory(void) {
    utils_print_header("STOCK INVENTORY");
    inventory_print_stock();
    inventory_print_alerts();
    utils_pause();
}

void ui_restock_inventory(void) {
    utils_print_header("RESTOCK INGREDIENTS");
    inventory_print_stock();

    int id = utils_read_int("Enter Ingredient ID to restock (0 to cancel): ", 0, 999);
    if (id == 0) return;

    InventoryItem *item = inventory_find_by_id(id);
    if (!item) {
        printf(ANSI_RED "Ingredient ID %d not found.\n" ANSI_RESET, id);
        utils_pause();
        return;
    }

    printf("Restocking: %s (Current: %.1f %s)\n", item->name, item->current_stock, item->unit);
    double qty = utils_read_double("Enter quantity to add: ", 0.01, 100000.0);

    if (inventory_restock(id, qty)) {
        storage_save_inventory(INVENTORY_FILE);
        printf(ANSI_GREEN "✓ Successfully added %.1f %s to %s! New Stock: %.1f %s\n" ANSI_RESET,
               qty, item->unit, item->name, item->current_stock, item->unit);
    } else {
        printf(ANSI_RED "Failed to restock ingredient.\n" ANSI_RESET);
    }
    utils_pause();
}

void ui_add_menu_item(void) {
    utils_print_header("ADD NEW MENU ITEM");

    MenuItem item;
    memset(&item, 0, sizeof(MenuItem));

    item.id = utils_read_int("Enter New Item ID (e.g. 12): ", 1, 9999);
    if (menu_find_by_id(item.id) != NULL) {
        printf(ANSI_RED "Error: Menu Item ID %d already exists!\n" ANSI_RESET, item.id);
        utils_pause();
        return;
    }

    utils_read_string("Enter Item Name: ", item.name, sizeof(item.name));
    printf("Select Category:\n  0: Hot Coffee\n  1: Iced Coffee\n  2: Tea\n  3: Pastry\n  4: Merch\n");
    int cat_choice = utils_read_int("Category choice (0-4): ", 0, 4);
    item.category = (ItemCategory)cat_choice;

    item.base_price = utils_read_double("Enter Base Price ($): ", 0.50, 500.0);
    utils_read_string("Enter Description: ", item.description, sizeof(item.description));
    item.available = true;
    item.recipe_count = 0;

    if (menu_add_item(&item)) {
        storage_save_menu(MENU_FILE);
        printf(ANSI_GREEN "✓ Successfully added '%s' to the menu catalog!\n" ANSI_RESET, item.name);
    } else {
        printf(ANSI_RED "Failed to add menu item.\n" ANSI_RESET);
    }
    utils_pause();
}

void ui_view_customers(void) {
    utils_print_header("LOYALTY MEMBERS DIRECTORY");
    loyalty_print_customers();
    utils_pause();
}

void ui_view_coupons(void) {
    utils_print_header("ACTIVE PROMO COUPONS");
    coupon_print_all();
    utils_pause();
}

void ui_sales_analytics(void) {
    SalesReport report = analytics_compute_report(TRANSACTIONS_FILE);
    analytics_print_report(&report);
    utils_pause();
}

void ui_cashier_pos_flow(void) {
    const User *current_user = auth_get_current_user();
    const char *cashier_name = current_user ? current_user->username : "Cashier";

    char phone[MAX_PHONE_LEN] = {0};
    utils_print_header("NEW WALK-IN POS ORDER");
    utils_read_string("Enter Customer Phone (or press ENTER for walk-in): ", phone, sizeof(phone));

    Customer *cust = NULL;
    if (strlen(phone) > 0) {
        cust = loyalty_find_customer(phone);
        if (cust) {
            printf(ANSI_GREEN "  ✓ Loyalty Member: %s (Tier: %s, Points: %d)\n" ANSI_RESET,
                   cust->name, loyalty_tier_to_string(cust->tier), cust->points);
        } else {
            char reg_choice[8];
            printf(ANSI_YELLOW "  Phone not registered in loyalty program. Register customer? (y/n): " ANSI_RESET);
            utils_read_string(NULL, reg_choice, sizeof(reg_choice));
            if (reg_choice[0] == 'y' || reg_choice[0] == 'Y') {
                char name[MAX_NAME_LEN];
                utils_read_string("Enter Customer Full Name: ", name, sizeof(name));
                loyalty_register_customer(phone, name);
                storage_save_customers(CUSTOMERS_FILE);
                cust = loyalty_find_customer(phone);
                printf(ANSI_GREEN "  ✓ Customer %s registered with 10 welcome bonus points!\n" ANSI_RESET, name);
            }
        }
    }

    Order order = order_create(s_next_order_id++, cashier_name, phone);

    // Ordering item loop
    while (1) {
        menu_print_catalog(false);
        order_print_summary(&order);

        printf("\nOrder Options:\n");
        printf("  1: Add Item to Order\n");
        printf("  2: Remove Item from Order\n");
        printf("  3: Proceed to Checkout\n");
        printf("  4: Cancel Order\n");
        int opt = utils_read_int("Select option (1-4): ", 1, 4);

        if (opt == 1) {
            int item_id = utils_read_int("Enter Menu Item ID: ", 1, 999);
            const MenuItem *item = menu_find_by_id(item_id);
            if (!item || !item->available) {
                printf(ANSI_RED "Item unavailable or does not exist.\n" ANSI_RESET);
                continue;
            }
            int qty = utils_read_int("Enter Quantity: ", 1, 50);

            // Check stock feasibility
            if (!inventory_can_prepare_item(item, qty)) {
                printf(ANSI_RED "⚠ Warning: Insufficient raw ingredients in stock to prepare %dx %s!\n" ANSI_RESET, qty, item->name);
                continue;
            }

            order_add_item(&order, item, qty);
            printf(ANSI_GREEN "✓ Added %dx %s\n" ANSI_RESET, qty, item->name);
        } else if (opt == 2) {
            if (order.item_count == 0) {
                printf(ANSI_YELLOW "Cart is currently empty.\n" ANSI_RESET);
                continue;
            }
            int item_id = utils_read_int("Enter Menu Item ID to remove: ", 1, 999);
            if (order_remove_item(&order, item_id)) {
                printf(ANSI_GREEN "✓ Item removed from order.\n" ANSI_RESET);
            } else {
                printf(ANSI_RED "Item not found in current order cart.\n" ANSI_RESET);
            }
        } else if (opt == 3) {
            if (order.item_count == 0) {
                printf(ANSI_RED "Cannot checkout with an empty cart!\n" ANSI_RESET);
                continue;
            }
            break; // Proceed to payment
        } else if (opt == 4) {
            printf(ANSI_YELLOW "Order cancelled.\n" ANSI_RESET);
            return;
        }
    }

    // Discounts & Promo Calculation
    double discount = 0.0;
    order_calculate_subtotal(&order);

    // Apply Tier Discount if member
    if (cust && cust->tier != TIER_BRONZE) {
        double tier_rate = loyalty_get_tier_discount_rate(cust->tier);
        double tier_disc = order.subtotal * tier_rate;
        discount += tier_disc;
        printf(ANSI_GREEN "✓ Loyalty Tier Discount applied (%.0f%% off): -$%.2f\n" ANSI_RESET,
               tier_rate * 100.0, tier_disc);
    }

    // Apply Coupon Code
    char promo[MAX_CODE_LEN] = {0};
    utils_read_string("Enter Promo / Coupon Code (or press ENTER to skip): ", promo, sizeof(promo));
    if (strlen(promo) > 0) {
        char err_msg[128] = {0};
        double coupon_disc = coupon_validate_and_apply(promo, order.subtotal, err_msg, sizeof(err_msg));
        if (coupon_disc > 0.0) {
            discount += coupon_disc;
            printf(ANSI_GREEN "✓ %s\n" ANSI_RESET, err_msg);
        } else {
            printf(ANSI_YELLOW "  %s\n" ANSI_RESET, err_msg);
        }
    }

    order_calculate_totals(&order, discount);

    char sub_str[16], disc_str[16], tax_str[16], tot_str[16];
    utils_format_currency(order.subtotal, sub_str, sizeof(sub_str));
    utils_format_currency(order.discount_amount, disc_str, sizeof(disc_str));
    utils_format_currency(order.tax_amount, tax_str, sizeof(tax_str));
    utils_format_currency(order.total_amount, tot_str, sizeof(tot_str));

    printf("\n" ANSI_BOLD "--- Checkout Breakdown ---" ANSI_RESET "\n");
    printf("Subtotal: %s | Discount: -%s | Tax: %s | " ANSI_BOLD ANSI_GREEN "TOTAL: %s" ANSI_RESET "\n",
           sub_str, disc_str, tax_str, tot_str);

    // Payment Selection
    printf("\nSelect Payment Method:\n");
    printf("  1: Cash\n");
    printf("  2: Credit/Debit Card\n");
    printf("  3: Mobile Pay / Apple Pay / Google Pay\n");
    int pay_method = utils_read_int("Payment method (1-3): ", 1, 3);

    if (pay_method == 1) {
        while (1) {
            double cash = utils_read_double("Enter Cash Tendered ($): ", 0.01, 10000.0);
            double change = 0.0;
            if (payment_process_cash(&order, cash, &change)) {
                printf(ANSI_GREEN "✓ Payment accepted! Change due: $%.2f\n" ANSI_RESET, change);
                break;
            } else {
                printf(ANSI_RED "Insufficient cash tendered! Minimum required: %s\n" ANSI_RESET, tot_str);
            }
        }
    } else if (pay_method == 2) {
        char card_num[24];
        utils_read_string("Enter Last 4 Digits of Card: ", card_num, sizeof(card_num));
        payment_process_card(&order, card_num);
        printf(ANSI_GREEN "✓ Card payment approved!\n" ANSI_RESET);
    } else {
        payment_process_mobile(&order, "NFC-TXN-OK");
        printf(ANSI_GREEN "✓ Mobile contactless payment verified!\n" ANSI_RESET);
    }

    // Deduct stock for all items
    for (int i = 0; i < order.item_count; i++) {
        const MenuItem *item = menu_find_by_id(order.items[i].menu_item_id);
        if (item) {
            inventory_consume_for_item(item, order.items[i].quantity);
        }
    }
    storage_save_inventory(INVENTORY_FILE);

    // Award loyalty points
    if (cust) {
        loyalty_award_points(cust, order.total_amount);
        storage_save_customers(CUSTOMERS_FILE);
        printf(ANSI_CYAN "  + Earned %d loyalty points! Current balance: %d points (Tier: %s)\n" ANSI_RESET,
               (int)order.total_amount, cust->points, loyalty_tier_to_string(cust->tier));
    }

    // Log transaction to audit file
    storage_log_transaction(&order, TRANSACTIONS_FILE);

    // Print Receipt
    payment_print_receipt(&order);
    utils_pause();
}

void ui_admin_dashboard(void) {
    while (1) {
        utils_print_header("ADMINISTRATIVE MANAGEMENT CONSOLE");
        printf("  1: View Complete Menu Catalog\n");
        printf("  2: Add New Menu Item\n");
        printf("  3: View Inventory & Stock Levels\n");
        printf("  4: Restock Raw Ingredients\n");
        printf("  5: Customer Loyalty Directory\n");
        printf("  6: Active Promo Coupon Codes\n");
        printf("  7: Daily Sales & Analytics Report\n");
        printf("  8: View Audit Transaction Log\n");
        printf("  9: Return to Main Portal\n");

        int choice = utils_read_int("Enter selection (1-9): ", 1, 9);
        switch (choice) {
            case 1: ui_view_menu(); break;
            case 2: ui_add_menu_item(); break;
            case 3: ui_view_inventory(); break;
            case 4: ui_restock_inventory(); break;
            case 5: ui_view_customers(); break;
            case 6: ui_view_coupons(); break;
            case 7: ui_sales_analytics(); break;
            case 8:
                storage_print_transaction_history(TRANSACTIONS_FILE);
                utils_pause();
                break;
            case 9: return;
        }
    }
}
