#include "menu.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>

static MenuItem s_menu_items[MAX_MENU_ITEMS];
static int s_menu_count = 0;

void menu_init(void) {
    s_menu_count = 0;
    memset(s_menu_items, 0, sizeof(s_menu_items));
}

int menu_get_count(void) {
    return s_menu_count;
}

const MenuItem *menu_get_items(int *out_count) {
    if (out_count) {
        *out_count = s_menu_count;
    }
    return s_menu_items;
}

const MenuItem *menu_find_by_id(int id) {
    for (int i = 0; i < s_menu_count; i++) {
        if (s_menu_items[i].id == id) {
            return &s_menu_items[i];
        }
    }
    return NULL;
}

bool menu_add_item(const MenuItem *item) {
    if (!item || s_menu_count >= MAX_MENU_ITEMS) {
        return false;
    }
    // Check if ID already exists
    if (menu_find_by_id(item->id) != NULL) {
        return false;
    }
    s_menu_items[s_menu_count] = *item;
    s_menu_count++;
    return true;
}

bool menu_update_price(int id, double new_price) {
    if (new_price < 0.0) return false;
    for (int i = 0; i < s_menu_count; i++) {
        if (s_menu_items[i].id == id) {
            s_menu_items[i].base_price = new_price;
            return true;
        }
    }
    return false;
}

bool menu_set_availability(int id, bool available) {
    for (int i = 0; i < s_menu_count; i++) {
        if (s_menu_items[i].id == id) {
            s_menu_items[i].available = available;
            return true;
        }
    }
    return false;
}

bool menu_delete_item(int id) {
    for (int i = 0; i < s_menu_count; i++) {
        if (s_menu_items[i].id == id) {
            for (int j = i; j < s_menu_count - 1; j++) {
                s_menu_items[j] = s_menu_items[j + 1];
            }
            s_menu_count--;
            return true;
        }
    }
    return false;
}

const char *menu_category_to_string(ItemCategory cat) {
    switch (cat) {
        case CAT_HOT_COFFEE:  return "Hot Coffee";
        case CAT_ICED_COFFEE: return "Iced Coffee";
        case CAT_TEA:         return "Tea & Infusion";
        case CAT_PASTRY:      return "Pastry & Bakery";
        case CAT_MERCH:       return "Merchandise";
        default:              return "Unknown";
    }
}

ItemCategory menu_string_to_category(const char *str) {
    if (!str) return CAT_HOT_COFFEE;
    if (utils_strcasecmp(str, "Hot Coffee") == 0) return CAT_HOT_COFFEE;
    if (utils_strcasecmp(str, "Iced Coffee") == 0) return CAT_ICED_COFFEE;
    if (utils_strcasecmp(str, "Tea & Infusion") == 0 || utils_strcasecmp(str, "Tea") == 0) return CAT_TEA;
    if (utils_strcasecmp(str, "Pastry & Bakery") == 0 || utils_strcasecmp(str, "Pastry") == 0) return CAT_PASTRY;
    if (utils_strcasecmp(str, "Merchandise") == 0) return CAT_MERCH;
    return CAT_HOT_COFFEE;
}

void menu_print_catalog(bool show_all) {
    printf("\n" ANSI_BOLD ANSI_CYAN "%-4s %-22s %-16s %-10s %-10s" ANSI_RESET "\n", 
           "ID", "Name", "Category", "Price", "Status");
    printf("-----------------------------------------------------------------\n");
    for (int i = 0; i < s_menu_count; i++) {
        const MenuItem *item = &s_menu_items[i];
        if (!show_all && !item->available) continue;
        
        char price_buf[16];
        utils_format_currency(item->base_price, price_buf, sizeof(price_buf));
        
        const char *status_str = item->available ? 
            (ANSI_GREEN "In Stock" ANSI_RESET) : 
            (ANSI_RED "Sold Out" ANSI_RESET);
            
        printf("%-4d %-22s %-16s %-10s %-10s\n",
               item->id,
               item->name,
               menu_category_to_string(item->category),
               price_buf,
               status_str);
    }
    printf("-----------------------------------------------------------------\n");
}
