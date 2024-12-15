# 基本控制语句
创建表格
CREATE TABLE *表格名*(属性1,...,属性n)

插入数据
INSERT INTO *表格名*(VALUE1,...,VALUEn)

查找数据
SELECT *筛选目标*  FROM *表格名*  WHERE *条件* 
例
SELECT * FROM P
SELECT * FROM P WHERE row=1
SELECT * FROM P WHERE col1=1
目前只能实现筛选目标为*
条件可为行号( row=1)或某一属性的值(col1=1,即第一列的值为1)

更改数据
UPDATE *表格名* WHERE *条件* VALUES(*修改后该行的数据*)
例
UPDATE P WHERE row=1 VALUES(1,2,3,4)

删除数据
DELETE FROM *表格名* WHERE *条件*
例
DELETE FROM P WHERE row=1
DELETE FROM P WHERE col1=1
