

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "FreeRTOS.h"
#include "task.h"

#define STOCK_COUNT 50
#define MAX_TRADERS 7
#define MAX_STOCK_PRICE 50
#define MAX_STOCK_FLUCTUATION_PERCENTAGE 29
#define MAX_TRADES_PER_TRADER 10
#define MAX_TRADER_BALANCE 10000
#define STOCK_PRICE_UPDATE_INTERVAL pdMS_TO_TICKS(1000) // in milliseconds


typedef struct {
    char name[10];
    double openingPrice;
    double currentPrice;
} Stock;


typedef struct {
    char name[10];
    double balance;
    double startingBalance;
    int buyOrdersCount;
} Trader;


void updateStockPrice(Stock* stock) {
    double minPrice = stock->openingPrice - (stock->openingPrice * MAX_STOCK_FLUCTUATION_PERCENTAGE / 100);
    double maxPrice = stock->openingPrice + (stock->openingPrice * MAX_STOCK_FLUCTUATION_PERCENTAGE / 100);
    stock->currentPrice = ((double)rand() / RAND_MAX) * (maxPrice - minPrice) + minPrice;
}


void generateReport(Trader* traders, int traderCount) {
    printf("\n--------------------\n");
    printf("Simulation Report\n");
    printf("--------------------\n\n");

    for (int i = 0; i < traderCount; i++) {
        printf("Trader %s:\n", traders[i].name);
        printf("Starting Balance: $%.2f\n", traders[i].startingBalance);
        printf("Final Balance: $%.2f\n\n", traders[i].balance);
    }
}


void simulationTask(void* pvParameters) {
    Stock stocks[STOCK_COUNT];
    Trader traders[MAX_TRADERS];
    double totalTransactionTime = 0.0;
    int totalTransactions = 0;

    // Initialize stocks
    for (int i = 0; i < STOCK_COUNT; i++) {
        sprintf(stocks[i].name, "Stock %d", i + 1);
        stocks[i].openingPrice = ((double)rand() / RAND_MAX) * MAX_STOCK_PRICE + 1;
        stocks[i].currentPrice = stocks[i].openingPrice;
    }


    // Initialize traders
    for (int i = 0; i < MAX_TRADERS; i++) {
        sprintf(traders[i].name, "Trader %d", i + 1);
        traders[i].balance = MAX_TRADER_BALANCE;
        traders[i].buyOrdersCount = 0;
        traders[i].startingBalance = MAX_TRADER_BALANCE;
    }

    TickType_t startTime = xTaskGetTickCount();
    TickType_t currentTime = startTime;


    // Simulation loop
    while (currentTime - startTime < pdMS_TO_TICKS(1 * 60 * 1000)) {
        TickType_t loopStartTime = xTaskGetTickCount();


        // Update stock prices
        for (int i = 0; i < STOCK_COUNT; i++) {
            updateStockPrice(&stocks[i]);
        }


        // Place random buy orders for each trader
        for (int i = 0; i < MAX_TRADERS; i++) {
            Trader* trader = &traders[i];
            if (trader->buyOrdersCount < MAX_TRADES_PER_TRADER) {
                int stockIndex = rand() % STOCK_COUNT;
                double buyPrice = ((double)rand() / RAND_MAX) * stocks[stockIndex].currentPrice;


                if (buyPrice < stocks[stockIndex].currentPrice) {
                    // Buy the stock
                    double totalPrice = buyPrice;
                    trader->balance -= totalPrice;
                    trader->buyOrdersCount++;


                    // Sell the stock if the price increases
                    double sellPrice = stocks[stockIndex].currentPrice;
                    if (sellPrice > buyPrice) {
                        trader->balance += sellPrice;
                        trader->buyOrdersCount--;
                    }


                    printf("Trader %s: Bought stock %s at $%.2f and sold at $%.2f\n", trader->name, stocks[stockIndex].name, buyPrice, sellPrice);
                }
            }
        }


        // Update the current time
        currentTime = xTaskGetTickCount();


        // Calculate transaction time for this iteration
        TickType_t loopEndTime = xTaskGetTickCount();
        totalTransactionTime += (loopEndTime - loopStartTime) * portTICK_PERIOD_MS;
        totalTransactions += MAX_TRADERS;


        // Delay for the specified interval
        vTaskDelay(STOCK_PRICE_UPDATE_INTERVAL);
    }


    // Generate simulation report
    generateReport(traders, MAX_TRADERS);


    // Calculate and print average transaction time
    double averageTransactionTime = totalTransactions > 0 ? totalTransactionTime / totalTransactions : 0.0;
    printf("Average Transaction Time: %.2f milliseconds\n", averageTransactionTime);


    vTaskDelete(NULL);
}


int main() {
    srand(time(NULL));


    // Create a task for the simulation
    xTaskCreate(simulationTask, "SimulationTask", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);


    // Start the FreeRTOS scheduler
    vTaskStartScheduler();


    return 0;
}
