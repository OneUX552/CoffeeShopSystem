#include "analytics.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

SalesReport analytics_compute_report(const char *transaction_log_path) {
    SalesReport report;
    memset(&report, 0, sizeof(SalesReport));

    FILE *fp = fopen(transaction_log_path, "r");
    if (!fp) return report;

    char line[1024];
    while (fgets(line, sizeof(line), fp)) {
        utils_trim(line);
        if (strlen(line) == 0) continue;

        char ts[32] = {0}, cashier[32] = {0}, phone[32] = {0}, method[32] = {0};
        int order_id = 0;
        double total = 0.0;

        char *t = strtok(line, "|");
        if (t) strncpy(ts, t, sizeof(ts) - 1);
        t = strtok(NULL, "|");
        if (t) order_id = atoi(t);
        (void)order_id;
        t = strtok(NULL, "|");
        if (t) strncpy(cashier, t, sizeof(cashier) - 1);
        t = strtok(NULL, "|");
        if (t) strncpy(phone, t, sizeof(phone) - 1);
        t = strtok(NULL, "|");
        if (t) total = atof(t);
        t = strtok(NULL, "|");
        if (t) strncpy(method, t, sizeof(method) - 1);

        report.transaction_count++;
        report.total_revenue += total;

        if (utils_strcasecmp(method, "Cash") == 0) report.cash_count++;
        else if (utils_strcasecmp(method, "Credit/Debit Card") == 0 || utils_strcasecmp(method, "Card") == 0) report.card_count++;
        else if (utils_strcasecmp(method, "Mobile NFC / Pay") == 0 || utils_strcasecmp(method, "Mobile") == 0) report.mobile_count++;
    }

    fclose(fp);
    return report;
}

void analytics_print_report(const SalesReport *report) {
    if (!report) return;

    char rev_buf[16], avg_buf[16];
    double avg_ticket = report->transaction_count > 0 ? (report->total_revenue / report->transaction_count) : 0.0;

    utils_format_currency(report->total_revenue, rev_buf, sizeof(rev_buf));
    utils_format_currency(avg_ticket, avg_buf, sizeof(avg_buf));

    printf("\n" ANSI_BOLD ANSI_CYAN "=======================================================\n" ANSI_RESET);
    printf(ANSI_BOLD ANSI_YELLOW "           DAILY SALES & ANALYTICS REPORT              \n" ANSI_RESET);
    printf(ANSI_BOLD ANSI_CYAN "=======================================================\n" ANSI_RESET);
    printf("  Gross Revenue:               %s\n", rev_buf);
    printf("  Completed Orders:            %d\n", report->transaction_count);
    printf("  Average Order Value:         %s\n", avg_buf);
    printf("-------------------------------------------------------\n");
    printf(ANSI_BOLD "  Payment Method Breakdown:" ANSI_RESET "\n");
    printf("    • Cash Transactions:       %d (%.1f%%)\n",
           report->cash_count,
           report->transaction_count > 0 ? (100.0 * report->cash_count / report->transaction_count) : 0.0);
    printf("    • Card Transactions:       %d (%.1f%%)\n",
           report->card_count,
           report->transaction_count > 0 ? (100.0 * report->card_count / report->transaction_count) : 0.0);
    printf("    • Mobile Pay Transactions: %d (%.1f%%)\n",
           report->mobile_count,
           report->transaction_count > 0 ? (100.0 * report->mobile_count / report->transaction_count) : 0.0);
    printf(ANSI_BOLD ANSI_CYAN "=======================================================\n" ANSI_RESET);
}
