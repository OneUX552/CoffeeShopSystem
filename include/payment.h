#ifndef PAYMENT_H
#define PAYMENT_H

#include "models.h"
#include <stddef.h>
#include <stdbool.h>

bool payment_process_cash(Order *order, double amount_tendered, double *out_change);
bool payment_process_card(Order *order, const char *card_last4);
bool payment_process_mobile(Order *order, const char *transaction_ref);
const char *payment_method_to_string(PaymentMethod method);
void payment_print_receipt(const Order *order);
void payment_format_receipt(const Order *order, char *buffer, size_t max_len);

#endif // PAYMENT_H
