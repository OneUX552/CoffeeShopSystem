#include "storage.h"
#include "menu.h"
#include "inventory.h"
#include "loyalty.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

bool storage_ensure_data_dir(void) {
    #if defined(_WIN32)
        mkdir(DATA_DIR);
    #else
        mkdir(DATA_DIR, 0755);
    #endif
    return true;
}

bool storage_save_menu(const char *filepath) {
    FILE *fp = fopen(filepath, "w");
    if (!fp) return false;

    int count = 0;
    const MenuItem *items = menu_get_items(&count);

    // CSV Header
    fprintf(fp, "id,name,category,price,description,available,recipe_count\n");
    for (int i = 0; i < count; i++) {
        const MenuItem *it = &items[i];
        fprintf(fp, "%d,%s,%d,%.2f,%s,%d,%d\n",
                it->id, it->name, (int)it->category, it->base_price,
                it->description, it->available ? 1 : 0, it->recipe_count);
    }

    fclose(fp);
    return true;
}

bool storage_load_menu(const char *filepath) {
    FILE *fp = fopen(filepath, "r");
    if (!fp) return false;

    char line[512];
    // Skip header
    if (!fgets(line, sizeof(line), fp)) {
        fclose(fp);
        return false;
    }

    while (fgets(line, sizeof(line), fp)) {
        utils_trim(line);
        if (strlen(line) == 0) continue;

        MenuItem item;
        memset(&item, 0, sizeof(MenuItem));
        int cat_int = 0, avail_int = 1, recipe_cnt = 0;

        // Parse line: id,name,category,price,description,available,recipe_count
        char *token = strtok(line, ",");
        if (!token) continue;
        item.id = atoi(token);

        token = strtok(NULL, ",");
        if (!token) continue;
        strncpy(item.name, token, sizeof(item.name) - 1);

        token = strtok(NULL, ",");
        if (!token) continue;
        cat_int = atoi(token);
        item.category = (ItemCategory)cat_int;

        token = strtok(NULL, ",");
        if (!token) continue;
        item.base_price = atof(token);

        token = strtok(NULL, ",");
        if (!token) continue;
        strncpy(item.description, token, sizeof(item.description) - 1);

        token = strtok(NULL, ",");
        if (token) avail_int = atoi(token);
        item.available = (avail_int != 0);

        token = strtok(NULL, ",");
        if (token) recipe_cnt = atoi(token);
        item.recipe_count = recipe_cnt;

        menu_add_item(&item);
    }

    fclose(fp);
    return true;
}

bool storage_save_inventory(const char *filepath) {
    FILE *fp = fopen(filepath, "w");
    if (!fp) return false;

    int count = 0;
    const InventoryItem *items = inventory_get_items(&count);

    fprintf(fp, "id,name,unit,current_stock,min_threshold,unit_cost\n");
    for (int i = 0; i < count; i++) {
        const InventoryItem *it = &items[i];
        fprintf(fp, "%d,%s,%s,%.2f,%.2f,%.2f\n",
                it->id, it->name, it->unit, it->current_stock,
                it->min_threshold, it->unit_cost);
    }

    fclose(fp);
    return true;
}

bool storage_load_inventory(const char *filepath) {
    FILE *fp = fopen(filepath, "r");
    if (!fp) return false;

    char line[512];
    if (!fgets(line, sizeof(line), fp)) {
        fclose(fp);
        return false;
    }

    while (fgets(line, sizeof(line), fp)) {
        utils_trim(line);
        if (strlen(line) == 0) continue;

        InventoryItem item;
        memset(&item, 0, sizeof(InventoryItem));

        char *token = strtok(line, ",");
        if (!token) continue;
        item.id = atoi(token);

        token = strtok(NULL, ",");
        if (!token) continue;
        strncpy(item.name, token, sizeof(item.name) - 1);

        token = strtok(NULL, ",");
        if (!token) continue;
        strncpy(item.unit, token, sizeof(item.unit) - 1);

        token = strtok(NULL, ",");
        if (!token) continue;
        item.current_stock = atof(token);

        token = strtok(NULL, ",");
        if (!token) continue;
        item.min_threshold = atof(token);

        token = strtok(NULL, ",");
        if (!token) continue;
        item.unit_cost = atof(token);

        inventory_add_item(&item);
    }

    fclose(fp);
    return true;
}
