#include "storage.h"
#include "menu.h"
#include "inventory.h"
#include "loyalty.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

bool storage_ensure_data_dir(void) {
    #if defined(_WIN32)
        mkdir(DATA_DIR);
    #else
        mkdir(DATA_DIR, 0755);
    #endif
    return true;
}

bool storage_save_menu(const char *filepath) {
    FILE *fp = fopen(filepath, "w");
    if (!fp) return false;

    int count = 0;
    const MenuItem *items = menu_get_items(&count);

    // CSV Header
    fprintf(fp, "id,name,category,price,description,available,recipe_count\n");
    for (int i = 0; i < count; i++) {
        const MenuItem *it = &items[i];
        fprintf(fp, "%d,%s,%d,%.2f,%s,%d,%d\n",
                it->id, it->name, (int)it->category, it->base_price,
                it->description, it->available ? 1 : 0, it->recipe_count);
    }

    fclose(fp);
    return true;
}

bool storage_load_menu(const char *filepath) {
    FILE *fp = fopen(filepath, "r");
    if (!fp) return false;

    char line[512];
    // Skip header
    if (!fgets(line, sizeof(line), fp)) {
        fclose(fp);
        return false;
    }

    while (fgets(line, sizeof(line), fp)) {
        utils_trim(line);
        if (strlen(line) == 0) continue;

        MenuItem item;
        memset(&item, 0, sizeof(MenuItem));
        int cat_int = 0, avail_int = 1, recipe_cnt = 0;

        // Parse line: id,name,category,price,description,available,recipe_count
        char *token = strtok(line, ",");
        if (!token) continue;
        item.id = atoi(token);

        token = strtok(NULL, ",");
        if (!token) continue;
        strncpy(item.name, token, sizeof(item.name) - 1);

        token = strtok(NULL, ",");
        if (!token) continue;
        cat_int = atoi(token);
        item.category = (ItemCategory)cat_int;

        token = strtok(NULL, ",");
        if (!token) continue;
        item.base_price = atof(token);

        token = strtok(NULL, ",");
        if (!token) continue;
        strncpy(item.description, token, sizeof(item.description) - 1);

        token = strtok(NULL, ",");
        if (token) avail_int = atoi(token);
        item.available = (avail_int != 0);

        token = strtok(NULL, ",");
        if (token) recipe_cnt = atoi(token);
        item.recipe_count = recipe_cnt;

        menu_add_item(&item);
    }

    fclose(fp);
    return true;
}

bool storage_save_inventory(const char *filepath) {
    FILE *fp = fopen(filepath, "w");
    if (!fp) return false;

    int count = 0;
    const InventoryItem *items = inventory_get_items(&count);

    fprintf(fp, "id,name,unit,current_stock,min_threshold,unit_cost\n");
    for (int i = 0; i < count; i++) {
        const InventoryItem *it = &items[i];
        fprintf(fp, "%d,%s,%s,%.2f,%.2f,%.2f\n",
                it->id, it->name, it->unit, it->current_stock,
                it->min_threshold, it->unit_cost);
    }

    fclose(fp);
    return true;
}

bool storage_load_inventory(const char *filepath) {
    FILE *fp = fopen(filepath, "r");
    if (!fp) return false;

    char line[512];
    if (!fgets(line, sizeof(line), fp)) {
        fclose(fp);
        return false;
    }

    while (fgets(line, sizeof(line), fp)) {
        utils_trim(line);
        if (strlen(line) == 0) continue;

        InventoryItem item;
        memset(&item, 0, sizeof(InventoryItem));

        char *token = strtok(line, ",");
        if (!token) continue;
        item.id = atoi(token);

        token = strtok(NULL, ",");
        if (!token) continue;
        strncpy(item.name, token, sizeof(item.name) - 1);

        token = strtok(NULL, ",");
        if (!token) continue;
        strncpy(item.unit, token, sizeof(item.unit) - 1);

        token = strtok(NULL, ",");
        if (!token) continue;
        item.current_stock = atof(token);

        token = strtok(NULL, ",");
        if (!token) continue;
        item.min_threshold = atof(token);

        token = strtok(NULL, ",");
        if (!token) continue;
        item.unit_cost = atof(token);

        inventory_add_item(&item);
    }

    fclose(fp);
    return true;
}

bool storage_save_customers(const char *filepath) {
    FILE *fp = fopen(filepath, "w");
    if (!fp) return false;

    int count = 0;
    const Customer *customers = loyalty_get_customers(&count);

    fprintf(fp, "phone,name,points,tier,total_spent,visit_count\n");
    for (int i = 0; i < count; i++) {
        const Customer *c = &customers[i];
        fprintf(fp, "%s,%s,%d,%d,%.2f,%d\n",
                c->phone, c->name, c->points, (int)c->tier,
                c->total_spent, c->visit_count);
    }

    fclose(fp);
    return true;
}

bool storage_load_customers(const char *filepath) {
    FILE *fp = fopen(filepath, "r");
    if (!fp) return false;

    char line[512];
    if (!fgets(line, sizeof(line), fp)) {
        fclose(fp);
        return false;
    }

    while (fgets(line, sizeof(line), fp)) {
        utils_trim(line);
        if (strlen(line) == 0) continue;

        char phone[MAX_PHONE_LEN] = {0};
        char name[MAX_NAME_LEN] = {0};
        int points = 0, tier_int = 0, visits = 0;
        double total_spent = 0.0;

        char *token = strtok(line, ",");
        if (!token) continue;
        strncpy(phone, token, sizeof(phone) - 1);

        token = strtok(NULL, ",");
        if (!token) continue;
        strncpy(name, token, sizeof(name) - 1);

        token = strtok(NULL, ",");
        if (token) points = atoi(token);

        token = strtok(NULL, ",");
        if (token) tier_int = atoi(token);

        token = strtok(NULL, ",");
        if (token) total_spent = atof(token);

        token = strtok(NULL, ",");
        if (token) visits = atoi(token);

        loyalty_register_customer(phone, name);
        Customer *c = loyalty_find_customer(phone);
        if (c) {
            c->points = points;
            c->tier = (LoyaltyTier)tier_int;
            c->total_spent = total_spent;
            c->visit_count = visits;
        }
    }

    fclose(fp);
    return true;
}

bool storage_save_coupons(const char *filepath) {
    FILE *fp = fopen(filepath, "w");
    if (!fp) return false;

    int count = 0;
    const Coupon *coupons = coupon_get_all(&count);

    fprintf(fp, "code,discount_percent,min_order_value,active\n");
    for (int i = 0; i < count; i++) {
        const Coupon *c = &coupons[i];
        fprintf(fp, "%s,%.2f,%.2f,%d\n",
                c->code, c->discount_percent, c->min_order_value, c->active ? 1 : 0);
    }

    fclose(fp);
    return true;
}

bool storage_load_coupons(const char *filepath) {
    FILE *fp = fopen(filepath, "r");
    if (!fp) return false;

    char line[512];
    if (!fgets(line, sizeof(line), fp)) {
        fclose(fp);
        return false;
    }

    while (fgets(line, sizeof(line), fp)) {
        utils_trim(line);
        if (strlen(line) == 0) continue;

        Coupon c;
        memset(&c, 0, sizeof(Coupon));

        char *token = strtok(line, ",");
        if (!token) continue;
        strncpy(c.code, token, sizeof(c.code) - 1);

        token = strtok(NULL, ",");
        if (!token) continue;
        c.discount_percent = atof(token);

        token = strtok(NULL, ",");
        if (!token) continue;
        c.min_order_value = atof(token);

        token = strtok(NULL, ",");
        c.active = token ? (atoi(token) != 0) : true;

        coupon_add(&c);
    }

    fclose(fp);
    return true;
}
#include "payment.h"

bool storage_log_transaction(const Order *order, const char *filepath) {
    if (!order) return false;
    storage_ensure_data_dir();

    FILE *fp = fopen(filepath, "a");
    if (!fp) return false;

    // Log format: timestamp | order_id | cashier | phone | total | method | items
    fprintf(fp, "%s|%d|%s|%s|%.2f|%s|",
            order->timestamp,
            order->order_id,
            order->cashier_username,
            strlen(order->customer_phone) > 0 ? order->customer_phone : "Walk-in",
            order->total_amount,
            payment_method_to_string(order->payment_method));

    for (int i = 0; i < order->item_count; i++) {
        fprintf(fp, "%s (x%d)%s",
                order->items[i].item_name,
                order->items[i].quantity,
                (i == order->item_count - 1) ? "" : "; ");
    }
    fprintf(fp, "\n");

    fclose(fp);
    return true;
}

void storage_print_transaction_history(const char *filepath) {
    FILE *fp = fopen(filepath, "r");
    if (!fp) {
        printf("  [No transaction records found]\n");
        return;
    }

    printf("\n" ANSI_BOLD ANSI_CYAN "%-20s %-8s %-12s %-14s %-10s %-12s %s" ANSI_RESET "\n",
           "Timestamp", "Order#", "Cashier", "Customer", "Total", "Method", "Items");
    printf("-------------------------------------------------------------------------------------------------------\n");

    char line[1024];
    while (fgets(line, sizeof(line), fp)) {
        utils_trim(line);
        if (strlen(line) == 0) continue;

        char ts[32] = {0}, cashier[32] = {0}, phone[32] = {0}, method[32] = {0};
        char items[512] = {0};
        int order_id = 0;
        double total = 0.0;

        char *t = strtok(line, "|");
        if (t) strncpy(ts, t, sizeof(ts) - 1);
        t = strtok(NULL, "|");
        if (t) order_id = atoi(t);
        t = strtok(NULL, "|");
        if (t) strncpy(cashier, t, sizeof(cashier) - 1);
        t = strtok(NULL, "|");
        if (t) strncpy(phone, t, sizeof(phone) - 1);
        t = strtok(NULL, "|");
        if (t) total = atof(t);
        t = strtok(NULL, "|");
        if (t) strncpy(method, t, sizeof(method) - 1);
        t = strtok(NULL, "|");
        if (t) strncpy(items, t, sizeof(items) - 1);

        char total_buf[16];
        utils_format_currency(total, total_buf, sizeof(total_buf));

        printf("%-20s %-8d %-12s %-14s %-10s %-12s %s\n",
               ts, order_id, cashier, phone, total_buf, method, items);
    }

    printf("-------------------------------------------------------------------------------------------------------\n");
    fclose(fp);
}
