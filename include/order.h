#ifndef ORDER_H
#define ORDER_H

#include "models.h"
#include <stdbool.h>

Order order_create(int order_id, const char *cashier, const char *customer_phone);
bool order_add_item(Order *order, const MenuItem *item, int quantity);
bool order_remove_item(Order *order, int menu_item_id);
void order_clear(Order *order);
void order_print_summary(const Order *order);

#endif // ORDER_H
