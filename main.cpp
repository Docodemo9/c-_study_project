#include <libpq-fe.h>
#include <iostream>

static void printResult(PGresult* res) {
    int rows = PQntuples(res);
    int cols = PQnfields(res);

    for (int j = 0; j < cols; j++) {
        std::cout << PQfname(res, j);
        if (j < cols - 1) std::cout << " | ";
    }
    std::cout << "\n----------------------------------------\n";

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            std::cout << PQgetvalue(res, i, j);
            if (j < cols - 1) std::cout << " | ";
        }
        std::cout << "\n";
    }
    std::cout << "(" << rows << " rows)\n\n";
}

int main() {
    PGconn* conn = PQconnectdb(
        "host=localhost port=5433 dbname=Aip_project user=postgres password=postgres"
    );

    if (PQstatus(conn) != CONNECTION_OK) {
        std::cerr << "Ошибка подключения: " << PQerrorMessage(conn);
        PQfinish(conn);
        return 1;
    }

    PGresult* r1 = PQexec(conn, "SELECT * FROM orders");
    if (PQresultStatus(r1) != PGRES_TUPLES_OK) {
        std::cerr << "Ошибка orders: " << PQerrorMessage(conn);
    } else {
        std::cout << "=== orders ===\n";
        printResult(r1);
    }
    PQclear(r1);

    PGresult* r2 = PQexec(conn, "SELECT * FROM assortiment");
    if (PQresultStatus(r2) != PGRES_TUPLES_OK) {
        std::cerr << "Ошибка assortiment: " << PQerrorMessage(conn);
    } else {
        std::cout << "=== assortiment ===\n";
        printResult(r2);
    }
    PQclear(r2);

    PQfinish(conn);
    return 0;
}