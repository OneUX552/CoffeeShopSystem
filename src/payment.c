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
