#ifndef ANALYTICS_H
#define ANALYTICS_H

#include "models.h"

typedef struct {
    double total_revenue;
    int transaction_count;
    double total_tax;
    double total_discount;
    int cash_count;
    int card_count;
    int mobile_count;
} SalesReport;

SalesReport analytics_compute_report(const char *transaction_log_path);
void analytics_print_report(const SalesReport *report);

#endif // ANALYTICS_H
