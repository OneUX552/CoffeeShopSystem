#ifndef INVENTORY_H
#define INVENTORY_H

#include "models.h"
#include <stdbool.h>

void inventory_init(void);
int inventory_get_count(void);
const InventoryItem *inventory_get_items(int *out_count);
InventoryItem *inventory_find_by_id(int id);
bool inventory_add_item(const InventoryItem *item);
bool inventory_restock(int id, double quantity_added);
bool inventory_has_sufficient_stock(int id, double needed);
bool inventory_deduct(int id, double quantity_used);
int inventory_get_low_stock_count(void);
void inventory_print_stock(void);
void inventory_print_alerts(void);

// Recipe consumption helpers
bool inventory_can_prepare_item(const MenuItem *item, int quantity);
bool inventory_consume_for_item(const MenuItem *item, int quantity);

#endif // INVENTORY_H
