
CREATE TABLE Position(user_id INTEGER, 
                      code TEXT NOT NULL,
                      date TEXT NOT NULL,
                      avaliable DOUBLE,
                      frozen    DOUBLE,
                      PRIMARY KEY(user_id, code, date));

// bug:
mock ʱ, ��λ������� ����,��Ϊ �Ǹ�task ����ʱ�Ĳ�δ�������� ����ʱ ��task ����