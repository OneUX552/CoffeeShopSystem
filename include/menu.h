#ifndef MENU_H
#define MENU_H

#include "models.h"
#include <stdbool.h>

void menu_init(void);
int menu_get_count(void);
const MenuItem *menu_get_items(int *out_count);
const MenuItem *menu_find_by_id(int id);
bool menu_add_item(const MenuItem *item);
bool menu_update_price(int id, double new_price);
bool menu_set_availability(int id, bool available);
bool menu_delete_item(int id);
const char *menu_category_to_string(ItemCategory cat);
ItemCategory menu_string_to_category(const char *str);
void menu_print_catalog(bool show_all);

#endif // MENU_H
