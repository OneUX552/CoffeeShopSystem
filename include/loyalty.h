#ifndef LOYALTY_H
#define LOYALTY_H

#include "models.h"
#include <stdbool.h>

void loyalty_init(void);
int loyalty_get_customer_count(void);
const Customer *loyalty_get_customers(int *out_count);
Customer *loyalty_find_customer(const char *phone);
bool loyalty_register_customer(const char *phone, const char *name);
LoyaltyTier loyalty_calculate_tier(int points);
double loyalty_get_tier_discount_rate(LoyaltyTier tier);
void loyalty_award_points(Customer *customer, double amount_spent);
const char *loyalty_tier_to_string(LoyaltyTier tier);
void loyalty_print_customers(void);

#endif // LOYALTY_H
