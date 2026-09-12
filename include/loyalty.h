#ifndef LOYALTY_H
#define LOYALTY_H

#include "models.h"
#include <stddef.h>
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

// Coupon promotional features
void coupon_init(void);
int coupon_get_count(void);
const Coupon *coupon_get_all(int *out_count);
bool coupon_add(const Coupon *coupon);
const Coupon *coupon_find(const char *code);
double coupon_validate_and_apply(const char *code, double subtotal, char *err_msg, size_t err_size);
void coupon_print_all(void);

#endif // LOYALTY_H
