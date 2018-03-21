
CREATE TABLE Position(user_id INTEGER, 
                      code TEXT NOT NULL,
                      date TEXT NOT NULL,
                      avaliable DOUBLE,
                      frozen    DOUBLE,
                      PRIMARY KEY(user_id, code, date));

// bug:
mock 时, 破位清仓引发 崩溃,因为 那个task 是临时的并未真正加入 启动时 的task 容器