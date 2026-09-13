#include <libpq-fe.h>
#include <raylib.h>
#include <string>
#include <vector>

struct Table {
    std::string title;
    std::vector<std::string> headers;
    std::vector<std::vector<std::string>> rows;
};

Table loadTable(PGconn* conn, const std::string& title, const std::string& sql) {
    Table t;
    t.title = title;

    PGresult* res = PQexec(conn, sql.c_str());
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        t.headers.push_back("ERROR");
        t.rows.push_back({ PQerrorMessage(conn) });
        PQclear(res);
        return t;
    }

    int rows = PQntuples(res);
    int cols = PQnfields(res);

    for (int j = 0; j < cols; j++)
        t.headers.push_back(PQfname(res, j));

    for (int i = 0; i < rows; i++) {
        std::vector<std::string> row;
        for (int j = 0; j < cols; j++)
            row.push_back(PQgetvalue(res, i, j));
        t.rows.push_back(row);
    }

    PQclear(res);
    return t;
}

void drawTable(const Table& t, int startY) {
    int y = startY;
    int colW = 200;

    // Заголовок таблицы
    DrawText(t.title.c_str(), 20, y, 24, DARKBLUE);
    y += 35;

    // Заголовки колонок
    for (size_t j = 0; j < t.headers.size(); j++) {
        DrawText(t.headers[j].c_str(), 20 + (int)j * colW, y, 20, MAROON);
    }
    y += 28;

    // Строки
    for (size_t i = 0; i < t.rows.size() && i < 15; i++) {
        for (size_t j = 0; j < t.rows[i].size(); j++) {
            DrawText(t.rows[i][j].c_str(),
                     20 + (int)j * colW,
                     y + (int)i * 24, 18, BLACK);
        }
    }

    // Кол-во строк
    std::string cnt = "(" + std::to_string(t.rows.size()) + " rows)";
    DrawText(cnt.c_str(), 20, y + (int)t.rows.size() * 24 + 10, 18, DARKGRAY);
}

int main() {
    // 1. Подключение к БД
    PGconn* conn = PQconnectdb(
        "host=localhost port=5433 dbname=Aip_project user=postgres password=postgres"
    );
    if (PQstatus(conn) != CONNECTION_OK) {
        TraceLog(LOG_ERROR, "DB error: %s", PQerrorMessage(conn));
        PQfinish(conn);
        return 1;
    }

    // 2. Загружаем обе таблицы
    Table orders      = loadTable(conn, "=== orders ===",      "SELECT * FROM orders");
    Table assortiment = loadTable(conn, "=== assortiment ===", "SELECT * FROM assortiment");

    PQfinish(conn);

    // 3. Окно
    InitWindow(1000, 700, "PostgreSQL viewer");
    SetTargetFPS(60);

    int current = 0; // 0 = orders, 1 = assortiment

    while (!WindowShouldClose()) {
        // Переключение по Tab
        if (IsKeyPressed(KEY_TAB)) current = 1 - current;

        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Подсказка сверху
        DrawText("Press TAB to switch table", 20, 15, 18, GRAY);

        if (current == 0) drawTable(orders, 50);
        else              drawTable(assortiment, 50);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}