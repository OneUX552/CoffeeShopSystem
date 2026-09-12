#ifndef STORAGE_H
#define STORAGE_H

#include "models.h"
#include <stdbool.h>

#define DATA_DIR "data"
#define MENU_FILE "data/menu.csv"
#define INVENTORY_FILE "data/inventory.csv"
#define CUSTOMERS_FILE "data/customers.csv"
#define COUPONS_FILE "data/coupons.csv"
#define TRANSACTIONS_FILE "data/transactions.log"

bool storage_ensure_data_dir(void);

// Menu persistence
bool storage_save_menu(const char *filepath);
bool storage_load_menu(const char *filepath);

#endif // STORAGE_H

// Inventory persistence
bool storage_save_inventory(const char *filepath);
bool storage_load_inventory(const char *filepath);

// Customer and Coupon persistence
bool storage_save_customers(const char *filepath);
bool storage_load_customers(const char *filepath);
bool storage_save_coupons(const char *filepath);
bool storage_load_coupons(const char *filepath);
