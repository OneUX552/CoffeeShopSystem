#include "inventory.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>

static InventoryItem s_inventory[MAX_INVENTORY_ITEMS];
static int s_inventory_count = 0;

void inventory_init(void) {
    s_inventory_count = 0;
    memset(s_inventory, 0, sizeof(s_inventory));
}

int inventory_get_count(void) {
    return s_inventory_count;
}

const InventoryItem *inventory_get_items(int *out_count) {
    if (out_count) {
        *out_count = s_inventory_count;
    }
    return s_inventory;
}

InventoryItem *inventory_find_by_id(int id) {
    for (int i = 0; i < s_inventory_count; i++) {
        if (s_inventory[i].id == id) {
            return &s_inventory[i];
        }
    }
    return NULL;
}

bool inventory_add_item(const InventoryItem *item) {
    if (!item || s_inventory_count >= MAX_INVENTORY_ITEMS) {
        return false;
    }
    if (inventory_find_by_id(item->id) != NULL) {
        return false;
    }
    s_inventory[s_inventory_count] = *item;
    s_inventory_count++;
    return true;
}

bool inventory_restock(int id, double quantity_added) {
    if (quantity_added <= 0.0) return false;
    InventoryItem *item = inventory_find_by_id(id);
    if (!item) return false;
    item->current_stock += quantity_added;
    return true;
}

bool inventory_has_sufficient_stock(int id, double needed) {
    const InventoryItem *item = inventory_find_by_id(id);
    if (!item) return false;
    return item->current_stock >= needed;
}

bool inventory_deduct(int id, double quantity_used) {
    if (quantity_used < 0.0) return false;
    InventoryItem *item = inventory_find_by_id(id);
    if (!item || item->current_stock < quantity_used) {
        return false;
    }
    item->current_stock -= quantity_used;
    return true;
}

int inventory_get_low_stock_count(void) {
    int count = 0;
    for (int i = 0; i < s_inventory_count; i++) {
        if (s_inventory[i].current_stock <= s_inventory[i].min_threshold) {
            count++;
        }
    }
    return count;
}

void inventory_print_stock(void) {
    printf("\n" ANSI_BOLD ANSI_CYAN "%-4s %-24s %-12s %-12s %-10s %-10s" ANSI_RESET "\n", 
           "ID", "Ingredient Name", "Stock", "Unit", "Min Level", "Status");
    printf("----------------------------------------------------------------------------\n");
    for (int i = 0; i < s_inventory_count; i++) {
        const InventoryItem *item = &s_inventory[i];
        bool is_low = item->current_stock <= item->min_threshold;
        const char *status_str = is_low ? 
            (ANSI_BOLD ANSI_RED "LOW STOCK" ANSI_RESET) : 
            (ANSI_GREEN "OK" ANSI_RESET);

        printf("%-4d %-24s %-12.1f %-12s %-10.1f %-10s\n",
               item->id,
               item->name,
               item->current_stock,
               item->unit,
               item->min_threshold,
               status_str);
    }
    printf("----------------------------------------------------------------------------\n");
}

void inventory_print_alerts(void) {
    int count = inventory_get_low_stock_count();
    if (count == 0) {
        printf(ANSI_GREEN "  ✓ All inventory stock levels are healthy.\n" ANSI_RESET);
        return;
    }
    printf(ANSI_BOLD ANSI_RED "  ⚠ INVENTORY ALERT: %d item(s) below reorder threshold:\n" ANSI_RESET, count);
    for (int i = 0; i < s_inventory_count; i++) {
        const InventoryItem *item = &s_inventory[i];
        if (item->current_stock <= item->min_threshold) {
            printf(ANSI_YELLOW "    • %s (ID %d): %.1f %s remaining (Threshold: %.1f %s)\n" ANSI_RESET,
                   item->name, item->id, item->current_stock, item->unit, item->min_threshold, item->unit);
        }
    }
}
