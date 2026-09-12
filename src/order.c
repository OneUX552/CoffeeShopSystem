#include "order.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>

Order order_create(int order_id, const char *cashier, const char *customer_phone) {
    Order ord;
    memset(&ord, 0, sizeof(Order));
    ord.order_id = order_id;
    if (cashier) {
        strncpy(ord.cashier_username, cashier, MAX_NAME_LEN - 1);
    }
    if (customer_phone) {
        strncpy(ord.customer_phone, customer_phone, MAX_PHONE_LEN - 1);
    }
    ord.status = ORDER_PENDING;
    utils_get_timestamp(ord.timestamp, sizeof(ord.timestamp));
    return ord;
}

bool order_add_item(Order *order, const MenuItem *item, int quantity) {
    if (!order || !item || quantity <= 0) {
        return false;
    }

    // Check if item already exists in current order, if so increment quantity
    for (int i = 0; i < order->item_count; i++) {
        if (order->items[i].menu_item_id == item->id) {
            order->items[i].quantity += quantity;
            order->items[i].line_total = order->items[i].quantity * order->items[i].unit_price;
            return true;
        }
    }

    if (order->item_count >= MAX_ITEMS_IN_ORDER) {
        return false;
    }

    OrderLineItem *line = &order->items[order->item_count];
    line->menu_item_id = item->id;
    strncpy(line->item_name, item->name, MAX_NAME_LEN - 1);
    line->unit_price = item->base_price;
    line->quantity = quantity;
    line->line_total = (double)quantity * item->base_price;

    order->item_count++;
    return true;
}

bool order_remove_item(Order *order, int menu_item_id) {
    if (!order) return false;
    for (int i = 0; i < order->item_count; i++) {
        if (order->items[i].menu_item_id == menu_item_id) {
            for (int j = i; j < order->item_count - 1; j++) {
                order->items[j] = order->items[j + 1];
            }
            order->item_count--;
            return true;
        }
    }
    return false;
}

void order_clear(Order *order) {
    if (!order) return;
    order->item_count = 0;
    order->subtotal = 0.0;
    order->discount_amount = 0.0;
    order->tax_amount = 0.0;
    order->total_amount = 0.0;
    order->cash_tendered = 0.0;
    order->change_due = 0.0;
    order->status = ORDER_PENDING;
}

void order_print_summary(const Order *order) {
    if (!order) return;
    printf("\n" ANSI_BOLD "--- Current Order Cart (ID #%d) ---" ANSI_RESET "\n", order->order_id);
    if (order->item_count == 0) {
        printf("  [Cart is empty]\n");
        return;
    }

    printf("%-4s %-24s %-6s %-10s %-10s\n", "#", "Item", "Qty", "Price", "Total");
    printf("-----------------------------------------------------------\n");
    for (int i = 0; i < order->item_count; i++) {
        const OrderLineItem *line = &order->items[i];
        char u_price[16], l_total[16];
        utils_format_currency(line->unit_price, u_price, sizeof(u_price));
        utils_format_currency(line->line_total, l_total, sizeof(l_total));

        printf("%-4d %-24s %-6d %-10s %-10s\n",
               i + 1,
               line->item_name,
               line->quantity,
               u_price,
               l_total);
    }
    printf("-----------------------------------------------------------\n");
}

double order_calculate_subtotal(Order *order) {
    if (!order) return 0.0;
    double subtotal = 0.0;
    for (int i = 0; i < order->item_count; i++) {
        subtotal += order->items[i].line_total;
    }
    order->subtotal = subtotal;
    return subtotal;
}

void order_calculate_totals(Order *order, double discount_amount) {
    if (!order) return;
    order_calculate_subtotal(order);

    if (discount_amount > order->subtotal) {
        discount_amount = order->subtotal;
    }
    if (discount_amount < 0.0) {
        discount_amount = 0.0;
    }

    order->discount_amount = discount_amount;
    double taxable_amount = order->subtotal - order->discount_amount;
    order->tax_amount = taxable_amount * SALES_TAX_RATE;
    order->total_amount = taxable_amount + order->tax_amount;
}
