#include "payment.h"
#include <stdio.h>
#include <string.h>

bool payment_process_cash(Order *order, double amount_tendered, double *out_change) {
    if (!order) return false;
    if (amount_tendered < order->total_amount) {
        return false;
    }
    double change = amount_tendered - order->total_amount;
    order->payment_method = PAYMENT_CASH;
    order->cash_tendered = amount_tendered;
    order->change_due = change;
    order->status = ORDER_COMPLETED;
    if (out_change) {
        *out_change = change;
    }
    return true;
}

bool payment_process_card(Order *order, const char *card_last4) {
    if (!order) return false;
    // Simulate card processor approval
    (void)card_last4;
    order->payment_method = PAYMENT_CARD;
    order->cash_tendered = order->total_amount;
    order->change_due = 0.0;
    order->status = ORDER_COMPLETED;
    return true;
}

bool payment_process_mobile(Order *order, const char *transaction_ref) {
    if (!order) return false;
    (void)transaction_ref;
    order->payment_method = PAYMENT_MOBILE;
    order->cash_tendered = order->total_amount;
    order->change_due = 0.0;
    order->status = ORDER_COMPLETED;
    return true;
}

const char *payment_method_to_string(PaymentMethod method) {
    switch (method) {
        case PAYMENT_CASH:   return "Cash";
        case PAYMENT_CARD:   return "Credit/Debit Card";
        case PAYMENT_MOBILE: return "Mobile NFC / Pay";
        default:             return "Unknown";
    }
}
#include "utils.h"

void payment_format_receipt(const Order *order, char *buffer, size_t max_len) {
    if (!order || !buffer || max_len == 0) return;

    char sub_buf[16], disc_buf[16], tax_buf[16], tot_buf[16], tend_buf[16], chg_buf[16];
    utils_format_currency(order->subtotal, sub_buf, sizeof(sub_buf));
    utils_format_currency(order->discount_amount, disc_buf, sizeof(disc_buf));
    utils_format_currency(order->tax_amount, tax_buf, sizeof(tax_buf));
    utils_format_currency(order->total_amount, tot_buf, sizeof(tot_buf));
    utils_format_currency(order->cash_tendered, tend_buf, sizeof(tend_buf));
    utils_format_currency(order->change_due, chg_buf, sizeof(chg_buf));

    int offset = snprintf(buffer, max_len,
        "====================================================\n"
        "                ARTISAN COFFEE HOUSE                \n"
        "             124 Roasted Bean Boulevard             \n"
        "                 Tel: (555) 019-2834                \n"
        "====================================================\n"
        "Order #: %-6d                    Date: %s\n"
        "Cashier: %-12s           Phone: %s\n"
        "----------------------------------------------------\n"
        "%-24s %-6s %-10s %-8s\n"
        "----------------------------------------------------\n",
        order->order_id,
        order->timestamp,
        order->cashier_username,
        strlen(order->customer_phone) > 0 ? order->customer_phone : "Walk-in",
        "Item", "Qty", "Price", "Total"
    );

    for (int i = 0; i < order->item_count && offset < (int)max_len; i++) {
        const OrderLineItem *item = &order->items[i];
        char u_p[16], l_t[16];
        utils_format_currency(item->unit_price, u_p, sizeof(u_p));
        utils_format_currency(item->line_total, l_t, sizeof(l_t));

        offset += snprintf(buffer + offset, max_len - offset,
            "%-24s %-6d %-10s %-8s\n",
            item->item_name, item->quantity, u_p, l_t);
    }

    snprintf(buffer + offset, max_len - offset,
        "----------------------------------------------------\n"
        "Subtotal:                                  %10s\n"
        "Discount:                                 -%10s\n"
        "Sales Tax (8.25%%):                         %10s\n"
        "TOTAL:                                     %10s\n"
        "Payment Method:                            %10s\n"
        "Amount Tendered:                           %10s\n"
        "Change Due:                                %10s\n"
        "====================================================\n"
        "       Thank you for visiting Artisan Coffee!       \n"
        "                Have a wonderful day!               \n"
        "====================================================\n",
        sub_buf, disc_buf, tax_buf, tot_buf,
        payment_method_to_string(order->payment_method),
        tend_buf, chg_buf
    );
}

void payment_print_receipt(const Order *order) {
    if (!order) return;
    char buffer[2048];
    payment_format_receipt(order, buffer, sizeof(buffer));
    printf(ANSI_CYAN "%s" ANSI_RESET, buffer);
}
